/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
extern int flag_timer;
long ctr1000;

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[17]=process->count_sys[17]+1;
                 process->start_t[17] += ctr1000;
        }
	
	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[17] += ctr1000;
        }
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[17] += ctr1000;
        }
	return(OK);
}
