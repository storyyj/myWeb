#include "util.h"
#include<stdlib.h>
#include<stdio.h>
void errif(bool condition,const char* errormsg)
{
    if(condition)
    {
        perror(errormsg);
        exit(1);
    }
}