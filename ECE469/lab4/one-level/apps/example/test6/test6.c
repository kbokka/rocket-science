#include "usertraps.h"
#include "misc.h"

#define SPAWN_ME "spawn_me.dlx.obj"

void main (int argc, char *argv[])
{
  int num = 30; // Used to store number of processes to create
  int i; // Loop index variable
  sem_t s_main;
  sem_t s_procs_completed; // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  s_main = dstrtol(argv[1], NULL, 10);
  // -----------------------
  // Testcase: 6
  // -----------------------
  // Convert string from ascii command line argument to integer number
  Printf("Testcase 6: Call spawn_me program %d times\n", num);
  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(-(num - 1))) == SYNC_FAIL) {
    Printf("test6 (%d): Bad sem_create\n", getpid());
    Exit();
  }
  ditoa(s_procs_completed, s_procs_completed_str);
  Printf("test6 (%d): Creating %d simulatneous spawn_me's in a row\n", getpid(), num);
  for(i = 0; i< num; i++) {
    Printf("test6 (%d): Creating spawn_me #%d\n", getpid(), i);
    process_create(SPAWN_ME, s_procs_completed_str, NULL);
  }

  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test6(%d): Bad semaphore s_procs_completed (%d) in %s\n", getpid(), s_procs_completed);
    Exit();
  }

  if(sem_signal(s_main) != SYNC_SUCCESS) {
    Printf("test6(%d): Bad semaphore s_main (%d)!\n", getpid(), s_main);
    Exit();
  }

  Printf("Testcase 6 (%d): done\n", getpid());
}
