#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  int numprocs;
  const int num_molecules = 7;
  mbox_t h_procs_mbox;              // Semaphore used to wait until all spawned processes have completed
  mbox_t h_n2_mbox;
  mbox_t h_n_mbox;
  mbox_t h_h2o_mbox;
  mbox_t h_h2_mbox;
  mbox_t h_o2_mbox;
  mbox_t h_no2_mbox;
  mbox_t* mail_boxes[7] = { &h_procs_mbox, &h_n2_mbox, &h_n_mbox, &h_h2o_mbox, &h_h2_mbox, &h_o2_mbox, &h_no2_mbox };
  char h_procs_mbox_str[10];       // Used as command-line argument to pass page_mapped handle to new processes
  char h_n2_mbox_str[10];
  char h_n_mbox_str[10];
  char h_h2o_mbox_str[10];
  char h_h2_mbox_str[10];
  char h_o2_mbox_str[10];
  char h_no2_mbox_str[10];
  unsigned int num_n2;
  unsigned int num_h2o;
  unsigned int num_break_n2;
  unsigned int num_break_h2o;
  unsigned int num_make_no2;
  char num_break_h2o_str[10];
  char num_make_no2_str[10];
  int i;
  int count_h2 = 0;
  int count_no2 = 0;

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of N2 molecules> <number of H2O molecules> \n");
    Exit();
  }

  // mboxes
  // mail_boxes[num_molecules] = { &h_n2_mbox, &h_n_mbox, h_h2o_mbox, &h_h2_mbox, &h_o2_mbox, &h_no2_mbox };

  for(i = 0; i < num_molecules; i++) {
    if ((*(mail_boxes[i]) = mbox_create()) == MBOX_FAIL) {
      Printf("makeprocs (%d): ERROR: could not allocate mailbox!", getpid());
      Exit();
    }
    // Open mailbox to prevent deallocation
    if (mbox_open(*(mail_boxes[i])) == MBOX_FAIL) {
      Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), *(mail_boxes[i]));
      Exit();
    }
  }

  if (mbox_send(h_h2_mbox, sizeof(int), (void*) &count_h2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  if (mbox_send(h_no2_mbox, sizeof(int), (void*) &count_no2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_n2_mbox, h_n2_mbox_str);
  ditoa(h_n_mbox, h_n_mbox_str);
  ditoa(h_h2o_mbox, h_h2o_mbox_str);
  ditoa(h_h2_mbox, h_h2_mbox_str);
  ditoa(h_o2_mbox, h_o2_mbox_str);
  ditoa(h_no2_mbox, h_no2_mbox_str);

  num_n2 = dstrtol(argv[1], NULL, 10);
  num_h2o = dstrtol(argv[2], NULL, 10);
  num_break_n2 = num_n2;
  num_break_h2o = num_h2o / 2;
  // make NO2 is limited by the min of ( # O2 molecules , # N atoms )
  num_make_no2 = num_break_h2o < (num_break_n2 * 2) ? num_break_h2o : (num_break_n2 * 2);

  ditoa(num_break_h2o, num_break_h2o_str);
  ditoa(num_make_no2, num_make_no2_str);

  numprocs = num_n2 + num_h2o + num_break_n2+ num_break_h2o + num_make_no2;
  Printf("Creating %d processes\n", numprocs);

  ditoa(h_procs_mbox, h_procs_mbox_str);
  
  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  for(i = 0; i < num_n2; i++) {
    process_create(INJECT_N2, 0, 0, h_procs_mbox_str, h_n2_mbox_str, NULL);
  }
  for(i = 0; i < num_h2o; i++) {
    process_create(INJECT_H2O, 0, 0, h_procs_mbox_str, h_h2o_mbox_str, NULL);
  }
  for(i = 0; i < num_break_n2; i++) {
    process_create(BREAK_N2, 0, 0, h_procs_mbox_str, h_n2_mbox_str, h_n_mbox_str, NULL);
  }
  for(i = 0; i < num_break_h2o; i++) {
    process_create(BREAK_H2O, 0, 0, h_procs_mbox_str, h_h2o_mbox_str, h_h2_mbox_str, h_o2_mbox_str, num_break_h2o_str, NULL);
  }
  for(i = 0; i < num_make_no2; i++) {
    process_create(MAKE_NO2, 0, 0, h_procs_mbox_str, h_n_mbox_str, h_o2_mbox_str, h_no2_mbox_str, num_make_no2_str, NULL);
  }
  Printf("All Processes created\n");

  // And finally, wait until all spawned processes have finished.
  for(i = 0; i < numprocs; i++) {
    if (mbox_recv(h_procs_mbox, 0, NULL) == MBOX_FAIL) {
      Printf("makeprocs (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
    }
  }
  Printf("All other processes completed, exiting main process.\n");
}
