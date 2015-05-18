#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int counter;
  int countmax = 0x7fffffff >> 11; // largest positive value to be stored in int
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("spawn_me (%d): ++++ I'll be counting a lot to %d, get ready!\n", getpid(), countmax);
  for(counter = 0; counter < countmax; counter++) {}

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("spawn_me(%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("spawn_me (%d): ---- I'm done counting a lot to %d, you happy!\n", getpid(), countmax);
}
