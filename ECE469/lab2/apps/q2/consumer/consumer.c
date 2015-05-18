#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "procon.h"

void main (int argc, char *argv[])
{
  circular_buffer *cb;      // Used to access missile codes in shared memory page
  uint32 h_mem;             // Handle to the shared memory page
  sem_t h_procs_sem;  // Semaphore to signal the original process that we're done
  lock_t h_cb_lock;
  int i;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <lock handle>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  h_procs_sem = dstrtol(argv[2], NULL, 10);
  h_cb_lock = dstrtol(argv[3], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((cb = (circular_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  
  for(i = 0; i < THE_MSG_LENGTH; i++) {
    while(1) {
      while(lock_acquire(h_cb_lock) != SYNC_SUCCESS);
      if(cb->head == cb->tail) { // buffer empty
        lock_release(h_cb_lock);
        continue;
      }
      else {
        break;
      }
    }
    // CRITICAL SECTION
    Printf("Consumer %d ", getpid()); Printf("removed: %c\n", cb->buffer[cb->head]);
    cb->head = (cb->head + 1) % BUFFER_SIZE;    
    while(lock_release(h_cb_lock) != SYNC_SUCCESS);
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("Consumer: PID %d is complete.\n", getpid());
  if(sem_signal(h_procs_sem) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", h_procs_sem); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
