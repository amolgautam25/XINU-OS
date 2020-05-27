/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
 
extern int flag_timer;
long ctr1000;
SYSCALL chprio(int pid, int newprio)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[1]=process->count_sys[1]+1;
                 process->start_t[1] += ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[1] += ctr1000;
        }
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[1] += ctr1000;
        }
	return(newprio);
}
