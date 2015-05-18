#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  mbox_t h_procs_mbox;  // Semaphore to signal the original process that we're done
  mbox_t h_h2o_mbox;
  // Convert the command-line strings into integers for use as handles
  h_procs_mbox = dstrtol(argv[1], NULL, 10);
  h_h2o_mbox = dstrtol(argv[2], NULL, 10);
  
  if (mbox_open(h_h2o_mbox) == MBOX_FAIL) {
    Printf("inject_h2o (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  Printf("--PID %d: INJECT H2O molecule\n", getpid());
  if (mbox_send(h_h2o_mbox, 0, NULL) == MBOX_FAIL) {
    Printf("inject_h2o (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Signal the mbox to tell the original process that we're done
  if (mbox_open(h_procs_mbox) == MBOX_FAIL) {
    Printf("inject_h2o (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_send(h_procs_mbox, 0, NULL) == MBOX_FAIL) {
    Printf("inject_h2o (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  // Printf("*****%s ended\n", argv[0]);
}
