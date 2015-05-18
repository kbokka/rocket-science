#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so 
// that the grader knows they are defined in this file.
//--------------------------------------------------------

#define MEM_L1FIELD_FIRST_BITNUM 12
#define MEM_MAX_VIRTUAL_ADDRESS 0xfffff
#define MEM_MAX_SIZE 0x200000
#define	MEM_PTE_VALID 0x1
#define	MEM_PTE_DIRTY 0x2
#define	MEM_PTE_READONLY 0x4

#define	MEM_PAGESIZE (1 << MEM_L1FIELD_FIRST_BITNUM)
// MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the "offset" portion of an address.
#define	MEM_ADDRESS_OFFSET_MASK (MEM_PAGESIZE - 1)
// Number of page table entries
#define	MEM_PAGE_TBL_SIZE ((MEM_MAX_VIRTUAL_ADDRESS + 1) >> MEM_L1FIELD_FIRST_BITNUM)
#define	MEM_PTE_MASK (~(MEM_PTE_VALID | MEM_PTE_DIRTY | MEM_PTE_READONLY))

#define MEM_ADDRESS_TO_PAGE(address) ((address) >> MEM_L1FIELD_FIRST_BITNUM)
#define MEM_ADDRESS_TO_OFFSET(address) ((address) & MEM_ADDRESS_OFFSET_MASK)
#define MEM_HEAP_MAX_NODES 255
// Heap memory structure
typedef struct Node {
  struct Node *parent;
  struct Node *cleft;
  struct Node *crght;
  int index;
  int size;
  int addr;
  int inuse;
  int order;
} Node;

#endif	// _memory_constants_h_
