/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL	sleep10(int n)
{if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[19]=process->count_sys[19]+1;
                 process->start_t[19] += ctr1000;
        }
		
	STATWORD ps;    
	if (n < 0  || clkruns==0){
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[19] += ctr1000;
        }
	return(SYSERR);}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
		
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[19] += ctr1000;
        }
	return(OK);
}
