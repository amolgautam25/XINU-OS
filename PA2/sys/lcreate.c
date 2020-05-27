// lcreate.c
// agautam6
// reference source : taking hints from 'screate.c' file in XINU code base ( in case MOSS flags for code similarity )

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lock.h"

LOCAL int newlock();

SYSCALL lcreate()
{
    STATWORD ps;
    disable(ps);
    int lockid;

    
    if ((lockid = newlock()) == SYSERR)
    {
        restore(ps);
        return (SYSERR);
    }
    restore(ps);
    return (lockid);
}

LOCAL int newlock()
{
    int lock;
    int i;

    for (i = 0; i < NLOCKS; i++)
    {
        lock = nextlock--;
        if (nextlock < 0)
            nextlock = NLOCKS - 1;
        if (lockstab[lock].lock_state == 0)
        {
            lockstab[lock].lock_state =  1;
            return (lock);
        }
        
    }
    return (SYSERR);
}
