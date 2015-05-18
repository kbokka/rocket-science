#include "usertraps.h"
#include "misc.h"
// just for demonstration purposes
#include "os/memory_constants.h"

static int first_n_page = -1;

int recurse(int n) {
  int naddr = (int)&n;
  char padder[512] = "  hello i pad";
  padder[0] = 'a';

  Printf("test4 (%d): Run(%d) &n=%d page_of_n=%d\n", getpid(), n, &n, MEM_ADDRESS_TO_PAGE(naddr));
  if(first_n_page == -1) {
    first_n_page = MEM_ADDRESS_TO_PAGE(naddr);
    Printf("test4 (%d): First recurse run(%d) &n=%d page_of_n=%d\n", getpid(), n, &n, MEM_ADDRESS_TO_PAGE(naddr));
  }
  else if(first_n_page != MEM_ADDRESS_TO_PAGE(naddr)) {
    Printf("test4 (%d): Finishing recurse run(%d) &n=%d page_of_n=%d\n", getpid(), n, &n, MEM_ADDRESS_TO_PAGE(naddr));
    return 0;
  }
  return recurse(n + 1);
}

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int n = 100;
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  // Now print a message to show that everything worked
  Printf("Testcase 4: Calculate fib(%d)\n", n);
  Printf("test4 (%d): starting recursive function\n", getpid());
  recurse(1);

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test4 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test4 (%d): Done!\n", getpid());
}