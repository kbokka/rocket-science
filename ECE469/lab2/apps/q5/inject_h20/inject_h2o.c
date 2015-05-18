#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  sem_t h_h2o_sem;
  int num_h2o;

  // Convert the command-line strings into integers for use as handles
  h_procs_sem = dstrtol(argv[1], NULL, 10);
  h_h2o_sem = dstrtol(argv[2], NULL, 10);
  num_h2o = dstrtol(argv[3], NULL, 10);

  while(num_h2o > 0) {
    if(sem_signal(h_h2o_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore h2o sem (%d) in ", h_h2o_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
    Printf("PID %d: CREATED H2O molecule\n", getpid());
    num_h2o--;
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("PID %d: INJECT_H2O is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
