/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL	recvclr()
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[7]=process->count_sys[7]+1;
                 process->start_t[7] += ctr1000;
        }
	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[7] += ctr1000;
        }
	return(msg);
}
