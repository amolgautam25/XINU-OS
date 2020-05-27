//printtos.c

#include<stdio.h>

unsigned long *ebp;
unsigned long *esp;

void printtos ()
{
	
	kprintf("\n");
	asm("movl %ebp, ebp");
	asm("movl %esp, esp");
	kprintf("void printtos()\n");
	kprintf("Before[0x%08x]: 0x%08x\n",esp+2,*(ebp+2));
	kprintf("After [0x%08x]: 0x%08x\n",ebp,*ebp);
	
	int counter; 
	for (counter = 0 ; counter <4 ; counter++)
		kprintf("	element[0x%08x]: 0x%08x\n",esp+counter,*(esp+counter));

kprintf("\n");
	return 0;
}