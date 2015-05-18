#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{

  // process_create(BREAK_N2, h_procs_sem_str, h_n2_sem, h_n_sem, num_break_n2_str, NULL);
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  sem_t h_n2_sem;
  sem_t h_n_sem;
  int num_break_n2;

  // Convert the command-line strings into integers for use as handles
  h_procs_sem = dstrtol(argv[1], NULL, 10);
  h_n2_sem = dstrtol(argv[2], NULL, 10);
  h_n_sem = dstrtol(argv[3], NULL, 10);
  num_break_n2 = dstrtol(argv[4], NULL, 10);

  while(num_break_n2 > 0) {
    if(sem_wait(h_n2_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore n2 sem (%d) in ", h_n2_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(sem_signal(h_n_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore n sem (%d) in ", h_n_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(sem_signal(h_n_sem) != SYNC_SUCCESS) {
      Printf("Bad semaphore n sem (%d) in ", h_n_sem); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    num_break_n2--;
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("PID %d: BREAK_N2 is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
