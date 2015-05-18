#include "usertraps.h"
#include "misc.h"
// just for demonstration purposes
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  // maximum virtual address 0xfffff
  // minus a page size

  int* faultaddr = (int*)(MEM_MAX_VIRTUAL_ADDRESS + 1 - MEM_PAGESIZE) - 4;
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("Testcase 3: Segfault case\n");
  Printf("test3 (%d): First print about argc\n", getpid());
  Printf("test3 (%d):     &argc = %d\n", getpid(), &argc);
  Printf("test3 (%d):     argc  = %d\n", getpid(), argc);
  Printf("test3 (%d): User stack pagenumber= %d\n", getpid(), MEM_ADDRESS_TO_PAGE(MEM_MAX_VIRTUAL_ADDRESS + 1 - MEM_PAGESIZE));
  Printf("test3 (%d): Faultaddr  pagenumber= %d\n", getpid(), MEM_ADDRESS_TO_PAGE((int)faultaddr));
  Printf("test3 (%d): Location before %d (page before user stack page)\n", getpid(), (MEM_MAX_VIRTUAL_ADDRESS + 1 - MEM_PAGESIZE));
  Printf("test3 (%d):     faultaddr = %d\n", getpid(), faultaddr);

  Printf("test3 (%d): Signal main to continue before crashing this program\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test3 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test3 (%d):     *faultaddr= %d\n", getpid(), *faultaddr);
  Printf("test3 (%d): Done!\n", getpid());
}
