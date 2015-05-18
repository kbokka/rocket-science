#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "procon.h"

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create
  int i;                          // Loop index variable
  circular_buffer *cb;            // Used to get address of shared memory page
  uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t h_procs_sem;              // Semaphore used to wait until all spawned processes have completed
  lock_t h_cb_lock; // LOCK handles
  cond_t h_nFull_cv, h_nEmpty_cv;   // Condition variables
  char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char h_procs_sem_str[10];       // Used as command-line argument to pass page_mapped handle to new processes
  char h_cb_lock_str[10];
  char h_nFull_cv_str[10];         // Used as command-line arg
  char h_nEmpty_cv_str[10];        // Used as command-line arg


  if (argc != 2) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of processes to create>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  numprocs = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  Printf("Creating %d processes\n", 2*numprocs);

  // Allocate space for a shared memory page, which is exactly 64KB
  // Note that it doesn't matter how much memory we actually need: we 
  // always get 64KB
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Map shared memory page into this process's memory space
  if ((cb = (circular_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // Put some values in the shared memory, to be read by other processes
  cb->head = 0;
  cb->tail = 0;

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((h_procs_sem = sem_create(-(numprocs*2-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  if((h_cb_lock = lock_create()) == SYNC_FAIL) {
    Printf("Bad lock_create in ");
    Exit();
  }

  if((h_nFull_cv = cond_create(h_cb_lock)) == SYNC_FAIL) {
    Printf("Bad cond_create in ");
    Exit();
  }  

  if((h_nEmpty_cv = cond_create(h_cb_lock)) == SYNC_FAIL) {
    Printf("Bad cond_create in ");
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_mem, h_mem_str);
  ditoa(h_procs_sem, h_procs_sem_str);
  ditoa(h_cb_lock, h_cb_lock_str);
  ditoa(h_nFull_cv, h_nFull_cv_str);
  ditoa(h_nEmpty_cv, h_nEmpty_cv_str);

  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  for(i=0; i < numprocs; i++) {
    process_create(CONSUMER_TO_RUN, h_mem_str, h_procs_sem_str, h_cb_lock_str, h_nFull_cv_str, h_nEmpty_cv_str, NULL);
    process_create(PRODUCER_TO_RUN, h_mem_str, h_procs_sem_str, h_cb_lock_str, h_nFull_cv_str, h_nEmpty_cv_str, NULL);
    Printf("Process %d created\n", i);
  }

  // And finally, wait until all spawned processes have finished.
  if (sem_wait(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("All other processes completed, exiting main process.\n");
}
