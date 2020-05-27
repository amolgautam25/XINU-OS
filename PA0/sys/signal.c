/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL signal(int sem)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[16]=process->count_sys[16]+1;
                 process->start_t[16] += ctr1000;
        }
		
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[16] += ctr1000;
        }
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[16] += ctr1000;
        }
	return(OK);
}
