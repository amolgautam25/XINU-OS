/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include "lab1.h"

unsigned long currSP; /* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

// definning helper function here, declaration at the end of the file
void calculate_quantum();
int next_pid();
void calculate_quantum_exit(int pid);
void reset_all_quantum();

int resched()
{
	
	if (scheduling_class == 1)
	{

		//if(currpid!=0)
		//printf(" ---\n\nentering with id %d\n ", currpid);
		register struct pentry *optr; /* pointer to old process entry */
		register struct pentry *nptr; /* pointer to new process entry */

		register struct pentry *temptr; // for iterating through proctab to calculate sum and find state etc

		int elig_proc = 0;
		int proctab_counter = 0;

		for (proctab_counter = 0; proctab_counter < NPROC; proctab_counter++)
		{
			temptr = &proctab[proctab_counter];
			if ((temptr->pstate == PRREADY) || (temptr->pstate == PRCURR))
			{
				elig_proc = elig_proc + 1;
			}
		}

		// execute only if eligible processor is more than 2 ( handles the null case scenario)
		if (elig_proc > 2)
		{
			// inserting current process in ready queue

			optr = &proctab[currpid];
			if (optr->pstate == PRCURR)
			{
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}

			// calculating SUM of priority
			int sum_of_pprio = 0;
			for (proctab_counter = 0; proctab_counter < NPROC; proctab_counter++)
			{
				temptr = &proctab[proctab_counter];
				if ((temptr->pstate == PRREADY) || (temptr->pstate == PRCURR))
				{
					//	kprintf("%d , " , temptr->pprio);
					sum_of_pprio += temptr->pprio;
				}
			}

			// calculating random number
			int rand_num = rand();
			if (rand_num >= sum_of_pprio)
				rand_num = ((rand_num) % (sum_of_pprio));
			if (rand_num == 0)
			{
				// rand _num should not be zero
				rand_num = rand_num + 1;
			}

			//	kprintf("\n\n\nRand: %d SUM : %d",rand_num,sum_of_pprio);

			// iterating loop from back
			int prev;
			/*	prev=q[rdytail].qprev;
		kprintf("\n Queue: \n");
		while (q[prev].qkey != MININT )
		{
			kprintf(" | ");
			kprintf("%d , %d , %d",q[prev].qnext,q[prev].qkey,q[prev].qprev);
			prev = q[prev].qprev;
		}
	*/
			int prev2;
			int item_to_be_dequeued;
			prev2 = q[rdytail].qprev;
			while (q[prev2].qkey != MININT)
			{
				if (rand_num < q[prev2].qkey)
				{
					item_to_be_dequeued = q[q[prev2].qnext].qprev;
					break;
				}
				else
				{
					rand_num = rand_num - q[prev2].qkey;
				}
				prev2 = q[prev2].qprev;
			}

			/*
		kprintf("\nsent to DEQUEUE : %d ", q[q[item_to_be_dequeued].qnext].qprev );
		kprintf("\ncurrpid : %d",currpid );
		kprintf("\nnewpid %d",q[q[item_to_be_dequeued].qnext].qprev);
*/

			// now dequeing

			nptr = &proctab[currpid = item_to_be_dequeued];
			dequeue(item_to_be_dequeued);

			// setting new process to current
			nptr->pstate = PRCURR; /* mark it currently running	*/

			#ifdef RTCLOCK
			preempt = QUANTUM; /* reset preemption counter	*/
			#endif

			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

			/* The OLD process returns here when resumed. */
			return OK;
		}

		// ------------------------------------------------------------------------//
		/* no switch needed if current process priority higher than next*/

		if (((optr = &proctab[currpid])->pstate == PRCURR) &&
			(lastkey(rdytail) < optr->pprio))
		{
			return (OK);
		}

		/* force context switch */

		if (optr->pstate == PRCURR)
		{
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[(currpid = getlast(rdytail))];
		nptr->pstate = PRCURR; /* mark it currently running	*/
		#ifdef RTCLOCK
		preempt = QUANTUM; /* reset preemption counter	*/
		#endif

		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;
	}

	
	
	// #####################################################################
	//######################################################################
	// linux scheuler part
	// right now combined with random scheduler
	else if(scheduling_class==2)
	{
		register struct pentry *optr; /* pointer to old process entry */
		register struct pentry *nptr; /* pointer to new process entry */

		int next_proc = -1;

		if (is_epoch_running == 0)
		{
			//chheck if there are any valid process that we may use in this epoch
			calculate_quantum();
			next_proc = next_pid();

			// there is a condition where there is no next process in ready queue  ( next_proc == -1 )
			if (next_proc == -1)
			{	
				if(currpid==0)
				{
					preempt=QUANTUM;
					return OK;
				}
				else
				{
					kprintf("\ncondition void with process %d\n",currpid);
					// basically this is an error condition in my knowledge
				}
			}
			// if there is any other process to run
			else
			{
				optr = &proctab[currpid];
				nptr = &proctab[next_proc];

				if (optr->pstate == PRCURR)
				{
					optr->pstate = PRREADY;
					insert(currpid, rdyhead, optr->pprio);
				}


				dequeue(next_proc);
				currpid = next_proc;
				nptr->pstate = PRCURR;
				is_epoch_running = 1;
				//printf("---");
				#ifdef RTCLOCK
				preempt = nptr->ticks_remaining;
				#endif

				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				
				return OK;
			}
		}

		// when an epoch is running
		if (is_epoch_running == 1)
		{
			// no need to calculate quantum again
			// if it's not the null process , then we update the quantum of whatever process just came in
			if(currpid!=0)
			calculate_quantum_exit(currpid);
			next_proc = next_pid();

			if (next_proc == -1)
			{
				optr = &proctab[currpid];
				nptr = &proctab[0];

				if (optr->pstate == PRCURR)
				{
					optr->pstate = PRREADY;
					insert(currpid, rdyhead, optr->pprio);
				}
				dequeue(0);
				is_epoch_running = 0;
				//printf("*");
				currpid = 0;
				nptr->pstate=PRCURR;
				//reset_all_quantum();
				#ifdef RTCLOCK
				preempt = QUANTUM;
				#endif
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}
			// other process to run in epoch
			//else if (next_proc != -1)
			
			else{
				optr = &proctab[currpid];
				nptr = &proctab[next_proc];

				if (optr->pstate == PRCURR)
				{
					optr->pstate = PRREADY;
					insert(currpid, rdyhead, optr->pprio);
				}

				dequeue(next_proc);
				currpid = next_proc;
				nptr->pstate=PRCURR;

				#ifdef RTCLOCK
				preempt = nptr->ticks_remaining; /* reset preemption counter	*/
				#endif
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}
		}
	}

	// else go back to the default scheduler
	else 
	{


	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
	}
}

// this function defines the next process to run
int next_pid()
{
	register struct pentry *temptr;
	int proctab_counter = 0;
	long max_goodness = 0;
	int pid = -1;
	for (proctab_counter = NPROC; proctab_counter > 0; proctab_counter--)
	{
		temptr = &proctab[proctab_counter];
		if (((temptr->pstate == PRREADY) ||(temptr->pstate== PRCURR) ) && (temptr->goodness > max_goodness))
		{
			pid = proctab_counter;
		}
	}
	return pid;
}
// return condition will either be -1 , 0 , or the PID

// this will be called in every epoch starting
void calculate_quantum()
{
	register struct pentry *temptr;
	int proctab_counter = 0;
	
	for (proctab_counter = NPROC; proctab_counter > 0; proctab_counter--)
	{
		temptr = &proctab[proctab_counter];
		if (temptr->pstate != PRFREE)
		{
				temptr->goodness = temptr->ticks_remaining + temptr->pprio;
				temptr->proc_quantum = ((temptr->ticks_remaining) / 2) + temptr->pprio;
				temptr->ticks_remaining = temptr->proc_quantum;	
		}
	}
}

// this process calculates the quantum and goodness for the processt that is about to go out
void calculate_quantum_exit(int pid)
{
	register struct pentry *temptr;
	temptr = &proctab[pid];
	temptr->goodness=temptr->goodness-(temptr->proc_quantum-preempt);
	temptr->proc_quantum = preempt;
	temptr->ticks_remaining = temptr->proc_quantum;
	//below is just a sanity check
	if (temptr->ticks_remaining <= 0)
	{
		temptr->goodness = 0;
		temptr->ticks_remaining = 0;
		temptr->proc_quantum = 0;
	}
	if(temptr->goodness<=0)
	temptr->goodness=0;
}

void reset_all_quantum()
{
	register struct pentry *temptr;
	int proctab_counter = 0;
	
	for (proctab_counter = 1; proctab_counter < NPROC; proctab_counter++)
	{
		temptr = &proctab[proctab_counter];
		if (temptr->pstate != PRFREE)
		{
				temptr->goodness = 0;
				temptr->proc_quantum = 0;
				temptr->ticks_remaining = 0;
		}
	}
}