/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include "lock.h"

void print_wait_queue_kill(int lock_id);
void all_kill_formalities(int proc_id, int lock_id);
void decrease_lprio(int lock_id);
void calculate_lprio_kill(int lock_id);

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;
	struct pentry *pptr; /* points to proc. table for pid*/
	int dev;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE)
	{
		restore(ps);
		return (SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->pdevs[1];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->ppagedev;
	if (!isbaddev(dev))
		close(dev);

	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate)
	{

	case PRCURR:
		pptr->pstate = PRFREE; /* suicide */
		resched();

	case PRWAIT:
		if (pptr->waiting_on_lock_id == -1)
			semaph[pptr->psem].semcnt++;
		else
		{
			int waiting_on_lock_id = pptr->waiting_on_lock_id;
		//	kprintf("\n Before dequeuing %d", pid);
			dequeue(pid);
		//	kprintf("\n after Dequeueing pid %d", pid);
			all_kill_formalities(pid, waiting_on_lock_id);
			decrease_lprio(waiting_on_lock_id);
		}

	case PRREADY:
		dequeue(pid);
		pptr->pstate = PRFREE;
		break;

	case PRSLEEP:
	case PRTRECV:
		unsleep(pid);
		/* fall through	*/
	default:
		pptr->pstate = PRFREE;
	}
	restore(ps);
	return (OK);
}

void print_wait_queue_kill(int lock_id)
{
	struct lockentry *temp;
	temp = &lockstab[lock_id];
//	kprintf("\nQueue with lock _ id = %d \n", lock_id);
	int prev = q[temp->lock_tail].qprev;
	while (prev < NPROC)
	{
//		kprintf(" | qprev %d , qkey %d , qnext %d", q[prev].qprev, q[prev].qkey, q[prev].qnext);
		prev = q[prev].qprev;
	}
}

void all_kill_formalities(int proc_id, int lock_id)
{
//	kprintf("\n Entered all kill formalities with pid %d lock_id %d ", proc_id, lock_id);
	proctab[proc_id].waiting_type = 0;
	proctab[proc_id].waiting_time = 0;
	proctab[proc_id].pinh = 0;
	proctab[proc_id].waiting_on_lock_id = -1; // -1 , that means , not waititng on any lock right now
	proctab[proc_id].pinh = 0;

	int j;
	for (j = 0; j < NLOCKS; j++)
	{
		proctab[proc_id].locks_held[j] = 0;
	}
//	kprintf("\n exiting , all kill formalities");
}

void decrease_lprio(int lock_id)
{
	calculate_lprio_kill(lock_id);

	struct lockentry *templ5;
	templ5 = &lockstab[lock_id];

	int i;
	for (i = NPROC - 1; i > 0; i--)
	{
		if (templ5->current_holders[i] == 1)
		{
			proctab[i].pinh=templ5->lprio;
			if(proctab[i].pinh==MININT)
			proctab[i].pinh=0;

		//	if (proctab[i].waiting_on_lock_id != -1)
		//	{
		//		prio_inheritence_part(proctab[i].waiting_on_lock_id);
		//	}
		}
	}
}

void calculate_lprio_kill(int lock_id)
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
//	kprintf("\nlargest scheduling priority in wAit queue of lock %d is %d", lock_id, templ4->lprio);
}