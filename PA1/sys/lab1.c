#include <kernel.h>
#include <q.h>
#include <proc.h>
#include "lab1.h"


int getschedclass(){
    return scheduling_class;
}
void setschedclass (int num)
{
    scheduling_class=num;
}