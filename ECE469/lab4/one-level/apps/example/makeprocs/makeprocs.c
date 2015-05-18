#include "usertraps.h"
#include "misc.h"

char * procs[] = { "test1.dlx.obj", "test3.dlx.obj", "test4.dlx.obj", "test5.dlx.obj", "test6.dlx.obj", "test2.dlx.obj" };

void main (int argc, char *argv[])
{
  int num = 6;             // Used to store number of processes to create
  int i;                               // Loop index variable
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  Printf("Starting %d testcases\n", num);
  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }
  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);

  for(i = 0; i < num; i++) {
    Printf("-------------------------------------------------------------------------------------\n");
    process_create(procs[i], s_procs_completed_str, NULL);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
    Printf("-------------------------------------------------------------------------------------\n");
  }
  
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
