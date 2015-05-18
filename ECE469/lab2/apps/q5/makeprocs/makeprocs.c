#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create

  sem_t h_procs_sem;              // Semaphore used to wait until all spawned processes have completed
  sem_t h_n2_sem;
  sem_t h_h2o_sem;
  sem_t h_n_sem;
  sem_t h_o2_sem;
  char h_procs_sem_str[10];       // Used as command-line argument to pass page_mapped handle to new processes
  char h_n2_sem_str[10];
  char h_h2o_sem_str[10];
  char h_n_sem_str[10];
  char h_o2_sem_str[10];

  uint32 num_n2;
  uint32 num_h2o;
  uint32 num_break_n2;
  uint32 num_break_h2o;
  uint32 num_make_no2;
  char num_n2_str[10];
  char num_h2o_str[10];
  char num_break_n2_str[10];
  char num_break_h2o_str[10];
  char num_make_no2_str[10];

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of N2 molecules> <number of H2O molecules> \n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  numprocs = 5;
  Printf("Creating %d processes\n", numprocs);

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((h_procs_sem = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Semaphores
  // N2 molecules
  if ((h_n2_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  // H2O molecules
  if ((h_h2o_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // N atoms
  if ((h_n_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // O2 molecules
  if ((h_o2_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_procs_sem, h_procs_sem_str);
  ditoa(h_n2_sem, h_n2_sem_str);
  ditoa(h_h2o_sem, h_h2o_sem_str);
  ditoa(h_n_sem, h_n_sem_str);
  ditoa(h_o2_sem, h_o2_sem_str);

  num_n2 = dstrtol(argv[1], NULL, 10);
  num_h2o = dstrtol(argv[2], NULL, 10);
  num_break_n2 = num_n2;
  num_break_h2o = num_h2o / 2;
  // make NO2 is limited by the min of ( # O2 molecules , # N atoms )
  num_make_no2 = num_break_h2o < (num_break_n2 * 2) ? num_break_h2o : (num_break_n2 * 2);

  ditoa(num_n2, num_n2_str);
  ditoa(num_h2o, num_h2o_str);
  ditoa(num_break_n2, num_break_n2_str);
  ditoa(num_break_h2o, num_break_h2o_str);
  ditoa(num_make_no2, num_make_no2_str);

  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  process_create(INJECT_N2, h_procs_sem_str, h_n2_sem_str, num_n2_str, NULL);
  process_create(INJECT_H2O, h_procs_sem_str, h_h2o_sem_str, num_h2o_str, NULL);
  process_create(BREAK_N2, h_procs_sem_str, h_n2_sem_str, h_n_sem_str, num_break_n2_str, NULL);
  process_create(BREAK_H2O, h_procs_sem_str, h_h2o_sem_str, h_o2_sem_str, num_break_h2o_str, NULL);
  process_create(MAKE_NO2, h_procs_sem_str, h_n_sem_str, h_o2_sem_str, num_make_no2_str, NULL);
  Printf("5 Processes created\n");

  // And finally, wait until all spawned processes have finished.
  if (sem_wait(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("All other processes completed, exiting main process.\n");
}
