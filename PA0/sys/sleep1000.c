/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;


SYSCALL sleep1000(int n)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[21]=process->count_sys[21]+1;
                 process->start_t[21] += ctr1000;
        }
		
	STATWORD ps;    

	if (n < 0  || clkruns==0){
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[21] += ctr1000;
        }
	return(SYSERR);}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[21] += ctr1000;
        }
		
	return(OK);
}
