#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lock.h"
#include <q.h>



void linit()
{
    struct lockentry *lentry;
    int i=0;
    

    for (i=0;i<NLOCKS;i++)
    {
        lentry = &lockstab[i];

        lentry->lock_index=i;   
        lentry->lock_state=0;   //
        lentry->lock_first_time=0;  
        lentry->lock_type=0;    //
        lentry->reader_count=0; //
        lentry->writer_count=0; //
        lentry->lprio=MININT;

        int j=0; 
        for ( j=0 ; j<NPROC;j++ )
            lentry->current_holders[j]=-1;          // -1 , means that the lock is not being held right now

        lentry->lock_tail = 1 + ( lentry->lock_head = newqueue());
    }

}