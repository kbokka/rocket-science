#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  sem_t h_n2_sem;
  int num_n2;

  // Convert the command-line strings into integers for use as handles
  h_procs_sem = dstrtol(argv[1], NULL, 10);
  h_n2_sem = dstrtol(argv[2], NULL, 10);
  num_n2 = dstrtol(argv[3], NULL, 10);

  while(num_n2 > 0) {
    if(sem_signal(h_n2_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore n2 sem (%d) in ", h_n2_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
    Printf("PID %d: CREATED N2 molecule\n", getpid());
    num_n2--;
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("PID %d: INJECT_N2 is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
