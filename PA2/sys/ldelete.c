#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include "lock.h"


int calculate_wait_write(int lock_id);
int calculate_wait_read(int lock_id);
void remove_all_the_process_from_wait_queue( int lock_id);
void dequeue_formalities(int lock_id, int proc_id);
void reset_lock_kill(int lock_id);


SYSCALL ldelete (int lock_id)
{
    STATWORD ps;
    disable (ps);

    if(lockstab[lock_id].writer_count + lockstab[lock_id].reader_count + calculate_wait_write(lock_id)+calculate_wait_read(lock_id)==0)
    {
       // kprintf("\n lock is good to go");
        reset_lock_kill(lock_id);
        restore(ps);
        return;
    }

    else
    {
        // remove all the process from the wait queue
        lockstab[lock_id].lock_state=-1;
        remove_all_the_process_from_wait_queue(lock_id);
        remove_all_current_holders(lock_id);
        restore(ps);
        resched();
        return;
    }
    

}

int calculate_wait_write(int lock_id)
{
  struct lockentry *lentry;
    lentry = &lockstab[lock_id];

    int num = 0;
    int prev;

    prev = q[lentry->lock_tail].qprev;
    while (q[prev].qkey != MININT)
    {
        if (proctab[prev].waiting_type == 2)
        {
            num++;
        }

        prev = q[prev].qprev;
    }

    return (num);
}

int calculate_wait_read(int lock_id)
{
  struct lockentry *lentry;
    lentry = &lockstab[lock_id];

    int num = 0;
    int prev;

    prev = q[lentry->lock_tail].qprev;
    while (q[prev].qkey != MININT)
    {
        if (proctab[prev].waiting_type == 1)
        {
            num++;
        }

        prev = q[prev].qprev;
    }

    return (num);
}

void remove_all_the_process_from_wait_queue( int lock_id)
{
    struct lockentry *lentry;
    lentry = &lockstab[lock_id];

    int prev=q[lentry->lock_tail].qprev;
    while(prev<NPROC)
    {
        dequeue(prev);
        dequeue_formalities(lock_id,prev);
        ready(prev,0);
        prev=q[lentry->lock_tail].qprev;
    }

}

void dequeue_formalities(int lock_id, int proc_id)
{
    proctab[proc_id].waiting_type=0;
    proctab[proc_id].waiting_on_lock_id=-1;
    proctab[proc_id].waiting_time=0;

}

void remove_all_current_holders(int lock_id)
{

    lockstab[lock_id].writer_count=0;
    lockstab[lock_id].reader_count=0;
    lockstab[lock_id].lprio=0;
    lockstab[lock_id].lock_type=0;
    lockstab[lock_id].lock_first_time=0;

    int i =0 ; 
    for (i=0;i<NPROC;i++)
    {
        if(lockstab[lock_id].current_holders[i]!=-1)
        {
            lockstab[lock_id].current_holders[i]=-1;
            proctab[i].locks_held[lock_id]=0;

        }
    }
}


void reset_lock_kill(int lock_id)
{
  //  kprintf("\n entered reset_lock , lock_id %d", lock_id);
    struct lockentry *templ5;
    templ5 = &lockstab[lock_id];

    templ5->lock_first_time = 0;
    templ5->lock_state = 0;
    templ5->lock_type = 0;
    templ5->writer_count = 0;
    templ5->reader_count = 0;
    templ5->lprio = MININT;
    int j = 0;
    for (j = 0; j < NPROC; j++)
        templ5->current_holders[j] = -1;
    //kprintf("\n Exiting reset_lock , lock_id %d", lock_id);
    return;
}
