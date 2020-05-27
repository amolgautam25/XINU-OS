/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL	receive()
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[6]=process->count_sys[6]+1;
                 process->start_t[6] += ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[6] += ctr1000;
        }
	return(msg);
}
