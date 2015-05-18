#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{

  // process_create(BREAK_H2O, h_procs_sem_str, h_h2o_sem, h_o2_sem, num_break_h2o_str, NULL);
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  sem_t h_h2o_sem;
  sem_t h_o2_sem;
  int num_break_h2o;

  // Convert the command-line strings into integers for use as handles
  h_procs_sem = dstrtol(argv[1], NULL, 10);
  h_h2o_sem = dstrtol(argv[2], NULL, 10);
  h_o2_sem = dstrtol(argv[3], NULL, 10);
  num_break_h2o = dstrtol(argv[4], NULL, 10);

  while(num_break_h2o > 0) {
    if(sem_wait(h_h2o_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore h2o sem (%d) in ", h_h2o_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(sem_wait(h_h2o_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore h2o sem (%d) in ", h_h2o_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(sem_signal(h_o2_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore o2 sem (%d) in ", h_o2_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
    num_break_h2o--;

    Printf("PID %d: CREATED H2 molecule.\n", getpid());
    if(num_break_h2o == 0) {
      Printf("PID %d: FINAL H2 molecule.\n", getpid());
    }
    else {
      Printf("PID %d: CREATED H2 molecule.\n", getpid());
    }
  }
  
  // Signal the semaphore to tell the original process that we're done
  Printf("PID %d: BREAK H2O is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
