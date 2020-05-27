// lock.h
// agautam6
// reference source : taking hints from 'sem.h' file in XINU code base
// code between this file and sem.h might be flagged for similarity , as the structure is the same

#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS 50

#define READ 1
#define WRITE 2

#define LOCKFREE 0               /* this lock is free		*/
#define LOCKUSED 1               /* this lock is used		*/
#define LOCKASSIGNEDBUTNOTUSED 2 // for the purpose of if the lock hags been assigned but not used ( using for testing only )

struct lockentry
{

    int lock_index;      // index of the lock that we need to create,
    int lock_state;      // to see if it is free or not     (decide between char / int )
    int lock_first_time; // to check if it is being locked for the first time or not
    int lock_type;       // to see if it is waiting on readers or writers

    int writer_count; // for the number of writer waiitng on it
    int reader_count; // for the numebr of READER waiting on it

    // not sure, if two queues are needed, or only queue is needed here

    int lock_head; // this is the loch head
    int lock_tail; // this is the lock tail

    int lprio;                          //
    int current_holders[NPROC];         // 1 , -1 
};

extern struct lockentry lockstab[];
extern int nextlock;

#define isbadlock(lock) (lock < 0 || lock >= NLOCKS)
#endif