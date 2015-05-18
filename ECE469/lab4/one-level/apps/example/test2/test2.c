#include "usertraps.h"
#include "misc.h"
// just for demonstration purposes
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  // maximum virtual address 0xfffff
  // max virtual address + 1 to byte algin
  // that + 4 go beyond max address space for sure
  int* faultaddr = (int*)(MEM_MAX_VIRTUAL_ADDRESS + 1);
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("Testcase 2: Access memory beyond maximum virtual address\n");
  Printf("Testcase 2: Running this at last since it will exitsim() \n");
  Printf("test2 (%d): First print about argc\n", getpid());
  Printf("test2 (%d):    &argc = %d\n", getpid(), &argc);
  Printf("test2 (%d):    argc  = %d\n", getpid(), argc);
  Printf("test2 (%d): Location after %d (a memory location beyond max virtual address)\n", getpid(), MEM_MAX_VIRTUAL_ADDRESS);
  Printf("test2 (%d):    faultaddr = %d\n", getpid(), faultaddr);

  Printf("test2 (%d): Signal main to continue before crashing this program\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test2 (%d):    *faultaddr= %d\n", getpid(), *faultaddr);
  Printf("test2 (%d): Done!\n", getpid());
}
