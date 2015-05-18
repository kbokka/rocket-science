#include "usertraps.h"
#include "misc.h"

#define HELLO_WORLD "hello_world.dlx.obj"

void main (int argc, char *argv[])
{
  int num_hello_world = 100;             // Used to store number of processes to create
  int i;                               // Loop index variable
  sem_t s_main;
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes
  s_main = dstrtol(argv[1], NULL, 10);

  Printf("Testcase 5: Call Hello world program %d times one after the other\n", num_hello_world);
  Printf("test5 (%d): Creating %d hello_world processes\n", getpid(), num_hello_world);
  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("test5 (%d): Bad sem_create\n", getpid());
    Exit();
  }
  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  // Create Hello World processes
  Printf("test5 (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), num_hello_world);
  for(i = 1; i < num_hello_world + 1; i++) {
    Printf("test5 (%d): Creating hello world #%d\n", getpid(), i);
    process_create(HELLO_WORLD, s_procs_completed_str, NULL);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
  }

  if(sem_signal(s_main) != SYNC_SUCCESS) {
    Printf("test6 (%d): Bad semaphore s_main (%d)!\n", getpid(), s_main);
    Exit();
  }

  Printf("Testcase 5 (%d): done\n", getpid());
}
