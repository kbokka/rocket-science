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
static char pagerefs[(MEM_MAX_SIZE >> MEM_L1FIELD_FIRST_BITNUM)];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;
static int physicalpgmax;

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
  physicalpgmax = MemoryGetSize() / MEM_PAGESIZE;
  pagestart = (lastosaddress + MEM_PAGESIZE - 4) / MEM_PAGESIZE;
  freemapmax = (physicalpgmax + 31) / 32;
  dbprintf('m', "Map has %d entries, memory size is 0x%x.\n", freemapmax, physicalpgmax);
  dbprintf('m', "Free pages start with page  # 0x%x.\n", pagestart);
  for(i = 0; i < physicalpgmax; i++) {
    pagerefs[i] = 0;
  }
  for(i = 0; i < freemapmax; i++) {
    // All pages are considered in use initially
    // so that there no partially initialized freemap entries for sure
    freemap[i] = 0;
  }
  nfreepages = 0;
  for(curpage = 0; curpage < pagestart; curpage++) {
    pagerefs[curpage] = 1;
  }
  for(curpage = pagestart; curpage < physicalpgmax; curpage++) {
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

void MemoryRopAccessHandler(PCB* pcb) {
  uint32 faddr = pcb->currentSavedFrame[PROCESS_STACK_FAULT]; // fault address
  int vpage = MEM_ADDRESS_TO_PAGE(faddr);
  int ppage = MEM_ADDRESS_TO_PAGE(pcb->pagetable[vpage] & MEM_PTE_MASK);
  int npage;

  dbprintf('m', "MemoryRopAccessHandler: page=%d pagerefs=%d\n", ppage, pagerefs[ppage]);
  printf("\nIn MemoryRopAccessHandler: [virtual address=0x%x page=%d] [physical page=%d pagerefs=%d]\n", faddr, vpage, ppage, pagerefs[ppage]);
  printf("----- Page table of process PID: %d before handling RopAccess ------\n", GetPidFromAddress(pcb));
  ProcessForkTestPrints(pcb);

  if(pagerefs[ppage] > 1) {
    npage = MemoryAllocPage();
    printf("****** I'm copying page %d to %d ******\n\n", ppage, npage);
    pcb->pagetable[vpage] = MemorySetupPte (npage);
    bcopy((char *)(faddr), (char *)(npage * MEM_PAGESIZE), MEM_PAGESIZE);
    pagerefs[ppage] -= 1;
  }
  else {
    pcb->pagetable[vpage] &= invert(MEM_PTE_READONLY); 
  }

  printf("----- Page table of process PID: %d after handling RopAccess -----\n", GetPidFromAddress(pcb));
  ProcessForkTestPrints(pcb);
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
  pagerefs[vector] = 1;
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

void MemorySharePage(uint32 pte) {
  int page = ((pte & MEM_PTE_MASK) / MEM_PAGESIZE);
  pagerefs[page] += 1;
  dbprintf('m', "MemorySharePage: page=%d refs=%d\n", page, pagerefs[page]);
  return;
}

// book keeping
void MemoryFreePage(uint32 page) {
  pagerefs[page] -= 1;
  dbprintf('m', "MemoryFreePage: pagerefs[%d]=%d (after decrementing refs)\n", page, pagerefs[page]);
  // printf("MemoryFreePage: pagerefs[%d]=%d (after decrementing refs)\n", page, pagerefs[page]);
  if(pagerefs[page] > 0) {
    return;
  }
  pagerefs[page] = 0;
  MemorySetFreemap(page, 1);
  nfreepages += 1;
  dbprintf('m', "MemoryFreePage: freedpage=0x%x nfreepages=%d\n", page, nfreepages);
}

void* malloc(PCB* pcb, int memsize) {
  return NULL;
}

int mfree(PCB* pcb, void* ptr) {
  return -1;
}
