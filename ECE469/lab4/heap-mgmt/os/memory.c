//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
// 1 bit per page. therefore num_pages / 32
static uint32 freemap[(MEM_MAX_SIZE >> MEM_L1FIELD_FIRST_BITNUM) / 32];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}

inline void MemorySetFreemap(int page, int state) {
  uint32 wd = page / 32;
  uint32 bitnum = page % 32;
  freemap[wd] = (freemap[wd] & invert(1 << bitnum)) | (state << bitnum);
  dbprintf('m', "Set freemap entry %d to 0x%x\n", wd, freemap[wd]);
}

//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int i;
  int curpage;
  int maxpage = MemoryGetSize() / MEM_PAGESIZE;

  pagestart = (lastosaddress + MEM_PAGESIZE - 4) / MEM_PAGESIZE;
  freemapmax = (maxpage + 31) / 32;
  dbprintf('m', "Map has %d entries, memory size is 0x%x.\n", freemapmax, maxpage);
  dbprintf('m', "Free pages start with page  # 0x%x.\n", pagestart);
  for(i = 0; i < freemapmax; i++) {
    // All pages are considered in use initially
    // so that there no partially initialized freemap entries for sure
    freemap[i] = 0;
  }
  nfreepages = 0;
  for(curpage = pagestart; curpage < maxpage; curpage++) {
    nfreepages += 1;
    MemorySetFreemap(curpage, 1);
  }
  dbprintf('m', "Initialized %d free pages.\n", nfreepages);
}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  int page    =   MEM_ADDRESS_TO_PAGE(addr);
  int offset  =   MEM_ADDRESS_TO_OFFSET(addr);

  // dbprintf('m', "MemoryTranslateUserToSystem (%d): addr 0x%x access\n", GetPidFromAddress(pcb), addr);
  if(pcb->pagetable[page] & MEM_PTE_VALID) {
    return ((pcb->pagetable[page] & MEM_PTE_MASK) | offset);
  }
  else {
    dbprintf('m', "MemoryTranslateUserToSystem (%d): invalid page %d access", GetPidFromAddress(pcb), page);
    return 0;
  }

}

//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser; // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);

    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference.
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  uint32 usrsp = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER]; // user stack pointer
  uint32 faddr = pcb->currentSavedFrame[PROCESS_STACK_FAULT]; // fault address
  int pg_faddr = MEM_ADDRESS_TO_PAGE(faddr);
  int pg_usrsp = MEM_ADDRESS_TO_PAGE(usrsp);
  int vpage;
  int newPage;

  dbprintf('m', "MemoryPageFaultHandler (%d): usrsp=0x%x pgusrsp=%d faddr=0x%x pgfaddr=%d\n", GetPidFromAddress(pcb), usrsp, pg_usrsp, faddr, pg_faddr);
  if(pg_faddr < pg_usrsp) {
    printf("Exiting PID %d: Segfaulted\n", GetPidFromAddress(pcb));
    dbprintf ('m', "MemoryPageFaultHandler (%d): Forcing a user exit faddr=0x%x\n", GetPidFromAddress(pcb), faddr);
    ProcessKill(pcb);
    return MEM_FAIL;
  }
  else {
    vpage = pg_faddr;
    newPage = MemoryAllocPage();
    if(newPage == MEM_FAIL) {
      printf("FATAL: not enough free pages for %d\n", GetPidFromAddress(pcb));
      ProcessKill();
    }
    pcb->pagetable[vpage] = MemorySetupPte(newPage);
    pcb->npages += 1;
    return MEM_SUCCESS;
  }
}

// searches the freemap for an empty slot in memory
// returns the free pagenumber
int MemoryAllocPage() {
  static int mapnum = 0;
  int bitnum;
  uint32 vector;

  dbprintf('m', "MemoryAllocPage: function started nfreepages=%d\n", nfreepages);
  if(nfreepages == 0) {
    return MEM_FAIL;
  }
  while(freemap[mapnum] == 0) {
    mapnum += 1;
    if(mapnum >= freemapmax) {
      mapnum = 0;
    }
  }
  vector = freemap[mapnum];
  for(bitnum = 0; (vector & (1 << bitnum)) == 0; bitnum++){}
  freemap[mapnum]  &= invert(1 << bitnum);
  vector = (mapnum * 32) + bitnum; // use same var to store free page number
  dbprintf('m', "MemoryAllocPage: allocated memory from map=%d, page=%d\n", mapnum, vector);
  nfreepages -= 1;
  return vector; // page number on memory space
}

// sets the valid bit
// returns the PTE [memaddress | flags]
uint32 MemorySetupPte (uint32 page) {
  return ((page * MEM_PAGESIZE) | MEM_PTE_VALID);
}

// converts PTE to page and call MemoryFreePage
void MemoryFreePte(uint32 pte) {
  MemoryFreePage((pte & MEM_PTE_MASK) / MEM_PAGESIZE);
}

// book keeping
void MemoryFreePage(uint32 page) {
  MemorySetFreemap(page, 1);
  nfreepages += 1;
  dbprintf('m', "MemoryFreePage: freedpage=0x%x nfreepages=%d\n", page, nfreepages);
}

//-------------------------------------------------------------------
// Heap management implementation
// Heap size is 4KB (one page of 4KB)
// smallest possible block (order 0) = 32 bytes
// Note: max # of order 0 blocks is 128
// Order 0: 32 Bytes 
// Order 1: 64 Bytes
// Order 2: 128 bytes
// Order 3: 256 bytes
// Order 4: 512 bytes
// Order 5: 1024 bytes
// Order 6: 2048 bytes
// Order 7: 4096 bytes
//-------------------------------------------------------------------
int s_alloc(Node * node, int memsize) {
  int temp;
  // Not a node
  if (node == NULL) {
    return -1;
  }
  // available leaf node
  if ((node->cleft == NULL) && (node->inuse == 0)) {
    // Correct size
    if ((memsize <= node->size) && (memsize > (node->size / 2))) {
      // Stop searching return addr
      node->inuse = 1;
      printf("Allocated the block: order = %d, addr = %d, requested mem size = %d, block size = %d\n", node->order, node->addr, memsize, node->size);
      return node->addr;
    } else {
      // Go back
      return -1;
    }
  }
  // Search left first
  temp = s_alloc(node->cleft, memsize);
  if (temp >= 0) {
    // found something 
    return temp;
  } else {
    // Keep searching on the right
    return s_alloc(node->crght, memsize);
  }
}

int create(Node * node, PCB* pcb, int memsize) {
  int temp;
  Node * lc;
  Node * rc;
  // Not a node
  if (node == NULL) {
    return -1;
  }
  // available leaf node
  if ((node->cleft == NULL) && (node->inuse == 0)) {
    // Correct size
    if ((memsize <= node->size) && (memsize > (node->size / 2))) {
      // Stop creating. Allocated and return addr
      node->inuse = 1;    
      printf("Allocated the block: order = %d, addr = %d, requested mem size = %d, block size = %d\n", node->order, node->addr, memsize, node->size);
      return node->addr;
    } 
    // can't divide more
    if ((node->size / 2) < memsize) {
      // go back
      return -1;
    } else {
      // Lowest order can't create more
      if (node->order == 0) {
        // go back
        return -1;
      } else {
        // Split free memory 
        // create left child
        lc = &(pcb->htree_array[2 * node->index]);
        lc->parent = node;
        lc->size = node->size / 2;
        lc->addr = node->addr;
        lc->order = node->order - 1;
        printf("Created a left child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n", lc->order, lc->addr, lc->size, node->order, node->addr, node->size);
        // creat right child
        rc = &(pcb->htree_array[2 * node->index + 1]);
        rc->parent = node;
        rc->size = node->size / 2;
        rc->addr = node->addr + rc->size;
        rc->order = node->order - 1;
        printf("Created a right child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n", rc->order, rc->addr, rc->size, node->order, node->addr, node->size);
        // link them
        node->cleft = lc;
        node->crght = rc;
      }
    }
  }
  // Try left first
  temp = create(node->cleft, pcb, memsize);
  if (temp >= 0) {
    // found something 
    return temp;
  } else {
    // Keep trying on the right
    return create(node->crght, pcb, memsize);
  }
}

void* malloc(PCB* pcb, int memsize) {
  int haddr;
  int size;
  int vaddr;
  int paddr;
  // Check for failures
  if ((memsize <= 0) || (memsize > MEM_PAGESIZE)) {
    return NULL;
  }

  // Look for block of size _memsize_ (in multiples of 4 bytes)
  haddr = s_alloc(&(pcb->htree_array[1]), memsize);
  // Found suitable block allocate
  if (haddr >= 0) {
    size = pcb->htree_array[haddr].size;
    vaddr = ((MEM_PAGESIZE * 4) | haddr);
    paddr = MemoryTranslateUserToSystem(pcb, vaddr);
    printf("Created a heap block of size %d bytes: virtual address %d, physical address %d\n", size, vaddr, paddr);
    // return pointer to starting virtual address.
    return (void *)vaddr;
  }
  // Try to create suitable block
  haddr = create(&(pcb->htree_array[1]), pcb, memsize);
  if (haddr >= 0) {
    size = pcb->htree_array[haddr].size;
    vaddr = ((MEM_PAGESIZE * 4) | haddr);
    paddr = MemoryTranslateUserToSystem(pcb, vaddr);
    printf("Created a heap block of size %d bytes: virtual address %d, physical address %d\n", size, vaddr, paddr);
    // return pointer to starting virtual address.
    return (void *)vaddr;
  } // Failure to find block
  return NULL;
}

void s_free(Node * node) {
  // No parent
  if (node == NULL) {
    return;
  }
  // Free block
  node->inuse = 0;
  node->cleft = NULL;
  node->crght = NULL;
  if (node->parent != NULL)
  {
    // Check if buddy is free
    if (node->parent->cleft == node) {
      if (node->parent->crght->inuse == 0) {
        printf("Coalesced buddy nodes (order = %d, addr = %d, size = %d) & (order = %d, addr = %d, size = %d)\n", node->order, node->addr, node->size, node->parent->crght->order, node->parent->crght->addr, node->parent->crght->size);
        printf("into the parent node (order = %d, addr = %d, size = %d)\n", node->parent->order, node->parent->addr, node->parent->size);
        s_free(node->parent);
      }
    } else {
      if (node->parent->cleft->inuse == 0) {
        printf("Coalesced buddy nodes (order = %d, addr = %d, size = %d) & (order = %d, addr = %d, size = %d)\n", node->parent->cleft->order, node->parent->cleft->addr, node->parent->cleft->size, node->order, node->addr, node->size);
        printf("into the parent node (order = %d, addr = %d, size = %d)\n", node->parent->order, node->parent->addr, node->parent->size);
        s_free(node->parent);
      }
    }
  }
  return;
}

int mfree(PCB* pcb, void* ptr) {
  int i;
  int size; 
  int order;
  int haddr;
  int vaddr;
  int paddr;
  Node * node;
  // Check for failures
  if (ptr == NULL) {
    return MEM_FAIL;
  }
  if ((((int)ptr >= (5 * MEM_PAGESIZE)) || ((int)ptr < (4 * MEM_PAGESIZE))))
  {
    return MEM_FAIL;
  }
  haddr = ((int)ptr & (MEM_ADDRESS_OFFSET_MASK));
  // root = &(pcb->htree_array[1]);
  // Find node with addr
  for (i = 1; i <= MEM_HEAP_MAX_NODES; i++)
  {
      if (pcb->htree_array[i].addr == haddr) {
        node = &(pcb->htree_array[i]);
      }
  }
  size = node->size;
  order = node->order;
  vaddr = (int)ptr;
  paddr = MemoryTranslateUserToSystem(pcb, vaddr);
  // Attempt to free heap block
  printf("Freeing heap block of size %d bytes: virtual address %d, physical address %d.\n", size, vaddr, paddr);
  s_free(node);
  // On success print msg and return # of bytes freed
  printf("Freed the block: order = %d, addr = %d, size = %d\n", order, haddr, size);
  return size;
}
