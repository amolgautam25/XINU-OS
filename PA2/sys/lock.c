#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include "lock.h"

void print_wait_queue_lock(int lock_id);
void acquire_lock(int pid, int lock_id, int operation, int waiting_priority);
void wait_on_lock(int pid, int lock_id, int operation, int waiting_priority);
int prio_of_max_writer(int lock_id);
int print_proc(int proc_id);
void calculate_lprio(int lock_id);
void prio_inheritence_part(int lock_id);
void inherit_priority(int pid1, int pid2);

SYSCALL lock(int lock_id, int operation, int waiting_priority)
{

    STATWORD ps;
    disable(ps);

  //  kprintf("\n entered lock.c with pid %d, lock_id %d, operation %d, waiting_prio %d", currpid, lock_id, operation, waiting_priority);
  //  print_wait_queue_lock(lock_id);
    struct lockentry *lentry;
    lentry = &lockstab[lock_id];

    if(lentry->lock_state==-1)
    {
        restore(ps);
        return DELETED;
    }

    struct pentry *p;
    p = &proctab[currpid];

    if (lentry->lock_first_time == 0)
    {
        lentry->lock_first_time = 1;
        acquire_lock(currpid, lock_id, operation, waiting_priority);
    //    kprintf("\n exiting lock.c with pid %d, lock_id %d, operation %d, waiting_prio %d", currpid, lock_id, operation, waiting_priority);
    //    print_proc(currpid);
        restore(ps);
        return OK;
    }
    else
    {
        // old reader
        if (lentry->lock_type == 1)
        {
            //old read , new read
            if (operation == 1)
            {
                int pid_of_max_writer = -1;
                pid_of_max_writer = prio_of_max_writer(lock_id);
      //          kprintf("\n wait priority of max writer in queue is %d", pid_of_max_writer);

                if (pid_of_max_writer < waiting_priority)
                {
                    acquire_lock(currpid, lock_id, operation, waiting_priority);
        //            kprintf("\n exiting lock.c with pid %d, lock_id %d, operation %d, waiting_prio %d", currpid, lock_id, operation, waiting_priority);
          //          print_proc(currpid);
            //        print_wait_queue_lock(lock_id);
                    restore(ps);
                    return OK;
                }
                else
                {
                    wait_on_lock(currpid, lock_id, operation, waiting_priority);
          //          kprintf("\n exiting lock.c with pid %d, lock_id %d, operation %d, waiting_prio %d", currpid, lock_id, operation, waiting_priority);
            //        print_proc(currpid);
              //      print_wait_queue_lock(lock_id);
                    prio_inheritence_part(lock_id);
                    restore(ps);
                    resched();
                    return OK;
                }
            }

            //old reader , new write
            if (operation == 2)
            {
                wait_on_lock(currpid, lock_id, operation, waiting_priority);
            //    print_wait_queue_lock(lock_id);
            //    print_proc(currpid);
                calculate_lprio(lock_id);
                prio_inheritence_part(lock_id);
             //   kprintf("\n exiting lock.c");
                restore(ps);
                resched();
                return OK;
            }
        }

        // old write, anything comes , it has to bw added to the queue
        if (lentry->lock_type == 2)
        {
            wait_on_lock(currpid, lock_id, operation, waiting_priority);
        //    print_wait_queue_lock(lock_id);
        //    print_proc(currpid);
            calculate_lprio(lock_id);
            prio_inheritence_part(lock_id);
        //    kprintf("\n exiting lock.c");
            restore(ps);
            resched(); 
            return OK;
        }
    }

    restore(ps);
    return;
}

void print_wait_queue_lock(int lock_id)
{
    struct lockentry *temp;
    temp = &lockstab[lock_id];
  //  kprintf("\nQueue with lock _ id = %d \n", lock_id);
    int prev = q[temp->lock_tail].qprev;
    while (prev < NPROC)
    {
    //    kprintf(" | qprev %d , qkey %d , qnext %d", q[prev].qprev, q[prev].qkey, q[prev].qnext);
        prev = q[prev].qprev;
    }
}

void acquire_lock(int pid, int lock_id, int operation, int waiting_priority)
{

  //  kprintf("\n entered acquire_lock with pid %d, lock_id %d, operation %d, waiting_prio %d", pid, lock_id, operation, waiting_priority);
    struct lockentry *templ;
    templ = &lockstab[lock_id];

    struct pentry *tempp;
    tempp = &proctab[currpid];

    if (operation == 1)
    {
        templ->reader_count++;
        templ->lock_type = 1;
        tempp->locks_held[lock_id] = 1;
        templ->current_holders[currpid] = 1;
    //    kprintf("\nExiting acquire_lock ");
        return;
    }

    if (operation == 2)
    {
        templ->writer_count++;
        templ->lock_type = 2;
        tempp->locks_held[lock_id] = 2;
        templ->current_holders[currpid] = 1;
      //  kprintf("\nExiting acquire_lock ");
        return;
    }

    return;
}

void wait_on_lock(int pid, int lock_id, int operation, int waiting_priority)
{
  //  kprintf("\n entered wait_on_lock with pid %d, lock_id %d, operation %d, waiting_prio %d", pid, lock_id, operation, waiting_priority);
    struct lockentry *templ2;
    templ2 = &lockstab[lock_id];

    struct pentry *tempp2;
    tempp2 = &proctab[currpid];

    if (operation == 1)
    {
        tempp2->waiting_type = 1;
    }
    if (operation == 2)
    {
        tempp2->waiting_type = 2;
    }

    tempp2->pstate = PRWAIT;
    tempp2->waiting_on_lock_id = lock_id; // for priority inheritence part

   // kprintf("\n executing insert.c with the following currpid : %d, head : %d, waiting_prio : %d", currpid, templ2->lock_head, waiting_priority);
    insert(currpid, templ2->lock_head, waiting_priority);
   // kprintf("\nExiting wait_on_lock");
    return;
}

// this function returns the maximun prio of the writer process in the queue
int prio_of_max_writer(int lock_id)
{
    struct lockentry *templ3;
    templ3 = &lockstab[lock_id];

    int prev = q[templ3->lock_tail].qprev;
    int max_prio = MININT;
    while (prev < NPROC)
    {
        if ((q[prev].qkey > max_prio) && (proctab[prev].waiting_type == 2))
        {
            max_prio = q[prev].qkey;
        }
        prev = q[prev].qprev;
    }
    return (max_prio);
}

// for debugging only
int print_proc(int proc_id)
{
  //  kprintf("\n pid = %d, waiting_type %d, pstate %c ", proc_id, proctab[proc_id].waiting_type, proctab[proc_id].pstate);
}

void calculate_lprio(int lock_id)
{
    struct lockentry *templ4;
    templ4 = &lockstab[lock_id];

    int max_prio = MININT;

    int prev = q[templ4->lock_tail].qprev;

    while (prev < NPROC)
    {
        if (proctab[prev].pinh != 0)
        {
            if (proctab[prev].pinh > max_prio)
                max_prio = proctab[prev].pinh;
        }
        else
        {
            if (proctab[prev].pprio > max_prio)
                max_prio = proctab[prev].pprio;
        }
        prev = q[prev].qprev;
    }

    templ4->lprio = max_prio;
   // kprintf("\nlargest scheduling priority in wAit queue of lock %d is %d", lock_id, templ4->lprio);
}

void prio_inheritence_part(int lock_id)
{
    calculate_lprio(lock_id);
    struct lockentry *templ5;
    templ5 = &lockstab[lock_id];

    int i;
    for (i = NPROC - 1; i > 0; i--)
    {
        if (templ5->current_holders[i] == 1)
        {
            if (proctab[i].pinh >= templ5->lprio || proctab[i].pprio >= templ5->lprio)
            {
                // then do nothing
            }
            else
            { // then change prio
                proctab[i].pinh = templ5->lprio;
            }

            if (proctab[i].waiting_on_lock_id != -1)
            {
                prio_inheritence_part(proctab[i].waiting_on_lock_id);
            }
        }
    }

    return;
}