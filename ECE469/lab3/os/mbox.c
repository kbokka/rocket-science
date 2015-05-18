#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static mbox mboxes[MBOX_NUM_MBOXES];
static mbox_message mbox_buffers[MBOX_NUM_BUFFERS];
// static sem_t s_buff_emptyslots;
static lock_t l_buff;
//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
	int i;
	int j;
	for(i = 0; i < MBOX_NUM_MBOXES; i++) {
		for(j = 0; j < PROCESS_MAX_PROCS; j++) {
			mboxes[i].opened_pids[j] = false;
		}

		for(j = 0; j < MBOX_MAX_BUFFERS_PER_MBOX; j++) {
			mboxes[i].cbobi[j] = -1; // invalid
		}

		mboxes[i].opened_pids_count = 0;
		mboxes[i].head_cbobi = 0;
		mboxes[i].tail_cbobi = 0;
		mboxes[i].inuse = false;
		mboxes[i].mbox_id = i;
	}
	for(i = 0; i < MBOX_NUM_BUFFERS; i++) {
		mbox_buffers[i].inuse = false;		
	}
	// if ((s_buff_emptyslots = SemCreate(MBOX_NUM_BUFFERS - 1)) == SYNC_FAIL) {
	// 	printf("Bad sem_create in MboxModInit");
	// 	exitsim();
	// }
	if((l_buff = LockCreate()) == SYNC_FAIL) {
		printf("Bad lock_create in MboxModInit");
		exitsim();
	}
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
	mbox_t imbox;
	uint32 intrval;

	// grabbing a Mbox should be an atomic operation
	intrval = DisableIntrs();
	for(imbox = 0; imbox < MBOX_NUM_MBOXES; imbox++) {
		if(mboxes[imbox].inuse == false) {
			mboxes[imbox].inuse = true;
			break;
		}
	}
	RestoreIntrs(intrval);
	if(imbox == MAX_SEMS) return MBOX_FAIL;
	if ((mboxes[imbox].s_mbox_emptyslots = SemCreate(MBOX_MAX_BUFFERS_PER_MBOX - 1)) == SYNC_FAIL) {
		printf("Bad sem_create in MboxOpen");
		exitsim();
	}
	if ((mboxes[imbox].s_mbox_fillslots = SemCreate(0)) == SYNC_FAIL) {
		printf("Bad sem_create in MboxOpen");
		exitsim();
	}
	if((mboxes[imbox].l_mbox = LockCreate()) == SYNC_FAIL) {
		printf("Bad lock_create in MboxModInit");
		exitsim();
	}
	return imbox;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
	int cpid = GetCurrentPid();
	uint32 intrval;

	if(handle > MBOX_NUM_MBOXES || handle < 0) {
		return MBOX_FAIL;
	}
	if(mboxes[handle].inuse == false) {
		return MBOX_FAIL;
	}
	if(mboxes[handle].opened_pids[cpid] == false) {
		mboxes[handle].opened_pids[cpid] = true;
		intrval = DisableIntrs();
		mboxes[handle].opened_pids_count += 1;
		RestoreIntrs(intrval);
	}
	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
	int cpid = GetCurrentPid();
	uint32 intrval;
	int j;
	mbox* xbox = &mboxes[handle];
	if(xbox->opened_pids[cpid] == false) {
		return MBOX_FAIL;
	}
	xbox->opened_pids[cpid] = false;
	intrval = DisableIntrs();
	xbox->opened_pids_count -= 1;
	if(xbox->opened_pids_count == 0) {
		// reset the mbox	
		for(j = xbox->head_cbobi; j <= xbox->tail_cbobi; (j = (j+1) % MBOX_MAX_BUFFERS_PER_MBOX)) {
			mbox_buffers[xbox->cbobi[j]].inuse = false;
			xbox->cbobi[j] = -1;
		}
		//sems[xbox->s_mbox_emptyslots].inuse = false;
		//sems[xbox->s_mbox_fillslots].inuse = false;
		//locks[xbox->l_mbox].inuse = false;
		xbox->head_cbobi = 0;
		xbox->tail_cbobi = 0;
		xbox->inuse = false;
	}
	RestoreIntrs(intrval);
	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
	int ibuff;
	int i;
	int cpid = GetCurrentPid();
	char* cmessage = (char*) message;
	mbox* xbox = &mboxes[handle];
	if(xbox->opened_pids[cpid] == false) {
		return MBOX_FAIL;
	}
	if(length > MBOX_MAX_MESSAGE_LENGTH) {
		return MBOX_FAIL;
	}
	// printf("***%d Waiting on mbox empty slot\n", cpid);
	SemHandleWait(xbox->s_mbox_emptyslots);
	// printf("******%d Done waiting on mbox empty slot\n", cpid);
	while(true) {
		// SemHandleWait(s_buff_emptyslots);
		while(LockHandleAcquire(l_buff) != SYNC_SUCCESS);
		for(ibuff = 0; ibuff < MBOX_NUM_BUFFERS; ibuff++) {
			if(mbox_buffers[ibuff].inuse == false) {
				break;
			}
		}
		if(ibuff != MBOX_NUM_BUFFERS) {
			// buffers are not full
			mbox_buffers[ibuff].inuse = true;
			LockHandleRelease(l_buff);
			break;
		}
		LockHandleRelease(l_buff);
	}
	// printf("=%d is waiting on mbox lock\n", cpid);
	while(LockHandleAcquire(xbox->l_mbox) != SYNC_SUCCESS);
	// printf("==%d has acquired mbox lock\n", cpid);
	xbox->cbobi[xbox->tail_cbobi] = ibuff;
	xbox->tail_cbobi = (xbox->tail_cbobi + 1) % MBOX_MAX_BUFFERS_PER_MBOX;
	for(i = 0; i < length; i++) {
		mbox_buffers[ibuff].msg[i] = cmessage[i];
	}
	mbox_buffers[ibuff].length = length;
	mbox_buffers[ibuff].inuse = true;
	SemHandleSignal(xbox->s_mbox_fillslots);
	LockHandleRelease(xbox->l_mbox);
	// printf("===%d has released mbox lock\n", cpid);

	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
	int ibuff;
	int cpid = GetCurrentPid();
	int i;
	mbox* xbox = &mboxes[handle];
	char* cmessage = (char*) message;

	if(xbox->opened_pids[cpid] == false) {
		return MBOX_FAIL;
	}
	// printf("***%d Waiting on mbox filled slot\n", cpid);
	SemHandleWait(xbox->s_mbox_fillslots);
	// printf("******%d Done waiting on mbox filled slot\n", cpid);
	// printf("=%d is waiting on mbox lock\n", cpid);
	while(LockHandleAcquire(xbox->l_mbox) != SYNC_SUCCESS);
	// printf("==%d has acquired mbox lock\n", cpid);
	ibuff = xbox->cbobi[xbox->head_cbobi];
	if(ibuff < 0) {
		printf("Invalid message buffer index from cbobi.head: %d\n", xbox->head_cbobi);
		exitsim();
	}
	if(mbox_buffers[ibuff].inuse == false) {
		LockHandleRelease(xbox->l_mbox);
		return MBOX_FAIL;
	}
	if(mbox_buffers[ibuff].length > maxlength) {
		LockHandleRelease(xbox->l_mbox);
		return MBOX_FAIL;
	}

	for(i = 0; i < mbox_buffers[ibuff].length; i++) {
		cmessage[i] = mbox_buffers[ibuff].msg[i];
	}
	xbox->cbobi[xbox->head_cbobi] = -1; // invalidate the cbobi
	mbox_buffers[ibuff].inuse = false;
	xbox->head_cbobi = (xbox->head_cbobi + 1) % MBOX_MAX_BUFFERS_PER_MBOX;
	// SemHandleSignal(s_buff_emptyslots);
	SemHandleSignal(xbox->s_mbox_emptyslots);
	LockHandleRelease(xbox->l_mbox);
	// printf("===%d has released mbox lock\n", cpid);

	return mbox_buffers[ibuff].length;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
	int i;
	int j;
	uint32 intrval;
	for(i = 0; i < MBOX_NUM_MBOXES; i++) {
		mbox* xbox = &mboxes[i];
		if(xbox->opened_pids[pid] == true) {	
			xbox->opened_pids[pid] = false;
			xbox->opened_pids_count -= 1;
			intrval = DisableIntrs();
			if(xbox->opened_pids_count == 0) {
				// reset the mbox	
				for(j = xbox->head_cbobi; j <= xbox->tail_cbobi; (j = (j+1) % MBOX_MAX_BUFFERS_PER_MBOX)) {
					mbox_buffers[xbox->cbobi[j]].inuse = false;
					xbox->cbobi[j] = -1;
				}
				//sems[xbox->s_mbox_emptyslots].inuse = false;
				//sems[xbox->s_mbox_fillslots].inuse = false;
				//locks[xbox->l_mbox].inuse = false;
				xbox->head_cbobi = 0;
				xbox->tail_cbobi = 0;
				xbox->inuse = false;
			}
			RestoreIntrs(intrval);
		}
	}
	return MBOX_SUCCESS;
}
