#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  mbox_t h_procs_mbox;  // Semaphore to signal the original process that we're done
  mbox_t h_n_mbox;
  mbox_t h_o2_mbox;
  mbox_t h_no2_mbox;
  int run_count = 0;
  int total_run_count;

  // Convert the command-line strings into integers for use as handles
  h_procs_mbox = dstrtol(argv[1], NULL, 10);
  h_n_mbox = dstrtol(argv[2], NULL, 10);
  h_o2_mbox = dstrtol(argv[3], NULL, 10);
  h_no2_mbox = dstrtol(argv[4], NULL, 10);
  total_run_count = dstrtol(argv[5], NULL, 10);
  
  if (mbox_open(h_n_mbox) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_recv(h_n_mbox, 0, NULL) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  if (mbox_open(h_o2_mbox) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_recv(h_o2_mbox, 0, NULL) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  if (mbox_open(h_no2_mbox) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_recv(h_no2_mbox, sizeof(int), &run_count) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  run_count++;
  if(run_count == total_run_count) {
    Printf("------PID %d: CREATE LAST NO2 molecule.\n", getpid());
  }
  else {
    Printf("------PID %d: CREATE NO2 molecule.\n", getpid());
  }
  if (mbox_send(h_no2_mbox, sizeof(int), &run_count) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  
  // Signal the mbox to tell the original process that we're done
  if (mbox_open(h_procs_mbox) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_send(h_procs_mbox, 0, NULL) == MBOX_FAIL) {
    Printf("make_no2 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  // Printf("*****%s ended\n", argv[0]);
}
