#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{

  // process_create(MAKE_NO2, h_procs_sem_str, h_n_sem_str, h_o2_sem_str, num_make_no2_str, NULL);
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  sem_t h_n_sem;
  sem_t h_o2_sem;
  int num_make_no2;

  // Convert the command-line strings into integers for use as handles
  h_procs_sem = dstrtol(argv[1], NULL, 10);
  h_n_sem = dstrtol(argv[2], NULL, 10);
  h_o2_sem = dstrtol(argv[3], NULL, 10);
  num_make_no2 = dstrtol(argv[4], NULL, 10);

  while(num_make_no2 > 0) {
    if(sem_wait(h_n_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore n sem (%d) in ", h_n_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(sem_wait(h_o2_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore o2 sem (%d) in ", h_o2_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    num_make_no2--;

    if(num_make_no2 == 0) {
      Printf("PID %d: FINAL NO2 molecule.\n", getpid());
    }
    else {
      Printf("PID %d: CREATED NO2 molecule.\n", getpid());
    }
  }
  
  // Signal the semaphore to tell the original process that we're done
  Printf("PID %d: MAKE NO2 is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
