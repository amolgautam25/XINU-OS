/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL	recvtim(int maxwait)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[8]=process->count_sys[8]+1;
                 process->start_t[8] += ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;
	int	msg;

	if (maxwait<0 || clkruns == 0){
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[8] += ctr1000;
        }
	return(SYSERR);}
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait*1000);
		slnempty = TRUE;
		sltop = (int *)&q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[8] += ctr1000;
        }
	return(msg);
}
