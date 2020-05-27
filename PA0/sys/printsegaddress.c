//printsegaddress.c

#include<stdio.h>

extern int etext,edata,end;

void printsegaddress()
{
	kprintf("\n");
	kprintf("void printsegaddress()");
	kprintf("\n");
	kprintf("Current: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x00010000]=0x%08x",&etext, etext, &edata, edata, &end, end);
	kprintf("\n");
	kprintf("Preceding: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x00010000]=0x%08x", &etext-1, *(&etext-1), &edata-1, *(&edata-1), &end-1, *(&end-1));
	kprintf("\n");
	kprintf("After: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x00030000]=0x%08x",&etext+1, *(&etext+1), &edata+1, *(&edata+1), &end+1, *(&end+1));
	kprintf("\n");
	return ;
}
