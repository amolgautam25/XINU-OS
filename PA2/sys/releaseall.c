#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include "lock.h"

void reset_lock(int lock_id);
void print_wait_queue_release(int lock_id);
void critical_section_release(int lock_id, int proc_id, int operation);

SYSCALL releaseall(int number_of_args, int args1, ...)
{
    STATWORD ps;
    disable(ps);

    int args;
    int return_flag = 1;
    int final_flag = 0;
    unsigned long *a = (unsigned long *)(&args1);

    int iterator;
    for (iterator = 0; iterator < number_of_args; iterator++)
    {
        int args = *a++;
        return_flag = releaseall2(number_of_args, args);
        if (return_flag != 1)
        {
            final_flag = 1;
        }
    }

    restore(ps);
    resched();
    if (final_flag == 1)
        return SYSERR;
    else
        return OK;
}

int releaseall2(int number_of_args, int lock_id)
{
 //   kprintf("\nentering releaseall.c with nargs : %d, lock_id : %d", number_of_args, lock_id);
  //  kprintf("\n currpid for releaseall.c : %d", currpid);
    //  STATWORD(ps);
    //  disable(ps);

    struct lockentry *templ4;
    templ4 = &lockstab[lock_id];

    struct pentry *tempp4;
    tempp4 = &proctab[currpid];

    int last_read_or_write = 0;
    int number_of_reader_wait = 0;
    int number_of_writer_wait = 0;
    number_of_reader_wait = calculate_num_read(lock_id);
    number_of_writer_wait = calculate_num_write(lock_id);
 //   kprintf("\n nor %d , now %d , lock_id %d", number_of_reader_wait, number_of_writer_wait, lock_id);

    // edge condition, for locks not being held by the currpid  
    if (tempp4->locks_held[lock_id] == 0)
    {
        return SYSERR;
    }
    // find if the process is a read or write process that is exiting

    // this is a read process

    if (tempp4->locks_held[lock_id] == 1)
    {
        templ4->reader_count--;
        templ4->current_holders[currpid] = -1;
        tempp4->locks_held[lock_id] = 0;
        tempp4->pinh = 0;
        // calculate the lock_prio , no need as the current process exited
        if (templ4->reader_count == 0)
            last_read_or_write = 1;
    }

    // this is a write process
    if (tempp4->locks_held[lock_id] == 2)
    {

        templ4->writer_count--;
        tempp4->locks_held[lock_id] = 0;
        templ4->current_holders[currpid] = -1;
        tempp4->pinh = 0;
        // calculate the lock_prio, no need as the current process exited
        if (templ4->writer_count == 0)
            last_read_or_write = 1;
    }

    if (last_read_or_write == 0)
    {
      //  kprintf("\nRelealock.c is not in critical condition\n");
        // restore(ps);
        return 1;
    }

    if (last_read_or_write == 1)
    {
      //  kprintf("\n releaseall.c  is in critical section");
    }

    if (number_of_reader_wait == 0 && number_of_writer_wait == 0)
    {

        reset_lock(lock_id);
        //restore(ps);
      //  kprintf("Reset complete , check in debugger");
        return 1;
    }
    else
    {
        int prev = q[templ4->lock_tail].qprev;
        if (proctab[prev].waiting_type == 1)
        {

            while (q[prev].qkey != MININT && proctab[prev].waiting_type == 1)
            {
                critical_section_release(lock_id, prev, 1);
                dequeue(prev);
                ready(prev, 0);
                prev = q[templ4->lock_tail].qprev; // this is the crash part, ->lock tail + 1 , try lock->tail-1 or the code that is given in dequeue , solved by hit and trial
            }
       //     kprintf("\n after all the read process had been removed , wait queue : ");
        //    print_wait_queue_release(lock_id);
        //    kprintf("\n exiting function (above line can be blank if there are no process in the queue )");
            //      restore(ps);
            //      resched();
            return 1;
        }
        if (proctab[prev].waiting_type == 2)
        {
            critical_section_release(lock_id, prev, 2);
            dequeue(prev);
            ready(prev, 0);
        //    kprintf("\n writer process with pid %d has been dequed and put in ready queue ", prev);
        //    kprintf("\n exiting releaseall.c ");
            print_wait_queue_release(lock_id);

            //now we need to decrease the inherited priority

            //restore(ps);
            //resched();
            return 1;
        }
    }

//    kprintf("\nexiting releaseall.c with nargs : %d, lock_id : %d", number_of_args, lock_id);
    //restore(ps);
    return 1;
}

void reset_lock(int lock_id)
{
 //   kprintf("\n entered reset_lock , lock_id %d", lock_id);
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
 //   kprintf("\n Exiting reset_lock , lock_id %d", lock_id);
    return;
}

// this function is for debiugging purpose only
void print_wait_queue_release(int lock_id)
{
    struct lockentry *templ6;
    templ6 = &lockstab[lock_id];
  //  kprintf("\nQueue with lock _ id = \n", lock_id);
    int prev = q[templ6->lock_tail].qprev;
    while (prev < NPROC)
    {
 //       kprintf(" | qprev %d , qkey %d , qnext %d", q[prev].qprev, q[prev].qkey, q[prev].qnext);
        prev = q[prev].qprev;
    }
    return;
}

int calculate_num_read(int lock_id)
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

int calculate_num_write(int lock_id)
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

// this function is not being used anywhere , but i am keeping it , if need arises later,
int pid_of_highest_writer(int lock_id)
{
    struct lockentry *lentry;
    lentry = &lockstab[lock_id];

    int high_prio = MININT;
    int process_id;
    int prev;

    prev = q[lentry->lock_tail].qprev;
    while (q[prev].qkey != MININT)
    {
        if (q[prev].qkey > high_prio && proctab[prev].waiting_type == 2)
        {
            high_prio = q[prev].qkey;
            process_id = prev;
        }

        prev = q[prev].qprev;
    }

    kprintf("\nhighest write pid in queue %d, and wait prio of write is %d\n", process_id, high_prio);
    return (process_id);
}

void critical_section_release(int lock_id, int proc_id, int operation)
{
    proctab[proc_id].pstate = PRREADY;
    proctab[proc_id].waiting_on_lock_id = -1;
    lockstab[lock_id].current_holders[proc_id] = 1;
    proctab[proc_id].waiting_type = 0;
    proctab[currpid].pinh=0;


    if (operation == 1)
    {
        proctab[proc_id].locks_held[lock_id] = 1;
        lockstab[lock_id].lock_type = 1;
        lockstab[lock_id].reader_count++;
    }

    if (operation == 2)
    {
        proctab[proc_id].locks_held[lock_id] = 2;
        lockstab[lock_id].lock_type = 2;
        lockstab[lock_id].writer_count++;
    }
}