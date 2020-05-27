/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;
SYSCALL	wait(int sem)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[26]=process->count_sys[26]+1;
                 process->start_t[26] += ctr1000;
        }
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[26] += ctr1000;
        }
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[26] += ctr1000;
        }
		return pptr->pwaitret;
	}
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[26] += ctr1000;
        }
	return(OK);
}
