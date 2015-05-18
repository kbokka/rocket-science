#ifndef __MBOX_OS__
#define __MBOX_OS__

// From process.h too lazy to include
#define	PROCESS_MAX_PROCS	32	// Maximum number of active processes

#define MBOX_NUM_MBOXES 16           // Maximum number of mailboxes allowed in the system
#define MBOX_NUM_BUFFERS 50          // Maximum number of message buffers allowed in the system
#define MBOX_MAX_BUFFERS_PER_MBOX 10 // Maximum number of buffer slots available to any given mailbox
#define MBOX_MAX_MESSAGE_LENGTH 100   // Buffer size of 100 for each message

#define MBOX_FAIL -1
#define MBOX_SUCCESS 1

//---------------------------------------------
// Define your mailbox structures here
//--------------------------------------------

typedef struct mbox_message {
	char msg[MBOX_MAX_MESSAGE_LENGTH];
	int length;		// length of the msg
	char inuse;
} mbox_message;

typedef struct mbox {
	char opened_pids[PROCESS_MAX_PROCS];
	int opened_pids_count;
	int cbobi[MBOX_MAX_BUFFERS_PER_MBOX]; 	// circ buffer that stores the indexes to the MboxBuffer
	int head_cbobi;
	int tail_cbobi;
	char inuse;
	sem_t s_mbox_emptyslots;
	sem_t s_mbox_fillslots;
	lock_t l_mbox;
	int mbox_id;
} mbox;

typedef int mbox_t; // This is the "type" of mailbox handles

//-------------------------------------------
// Prototypes for Mbox functions you have to write
//-------------------------------------------

void MboxModuleInit();
mbox_t MboxCreate();
int MboxOpen(mbox_t m);
int MboxClose(mbox_t m);
int MboxSend(mbox_t m, int length, void *message);
int MboxRecv(mbox_t m, int maxlength, void *message);
int MboxCloseAllByPid(int pid);

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif
