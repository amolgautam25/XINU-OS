/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
 
 extern int flag_timer;
long ctr1000;

SYSCALL sleep100(int n)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[20]=process->count_sys[20]+1;
                 process->start_t[20] += ctr1000;
        }
	STATWORD ps;    

	if (n < 0  || clkruns==0){
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[20] += ctr1000;
        }
	return(SYSERR);}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[20] += ctr1000;
        }
	return(OK);
}
