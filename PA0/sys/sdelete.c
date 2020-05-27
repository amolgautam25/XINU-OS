/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
 
 extern int flag_timer;
long ctr1000;
SYSCALL sdelete(int sem)
{if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[11]=process->count_sys[11]+1;
                 process->start_t[11] += ctr1000;
        }
	STATWORD ps;    
	int	pid;
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[11] += ctr1000;
        }
		return(SYSERR);
	}
	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[11] += ctr1000;
        }
	return(OK);
}
