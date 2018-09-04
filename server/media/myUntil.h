#ifndef _MY_UNTIL_
#define _MY_UNTIL_

#include "OSMutex.h"
#include <stdio.h>
#include <stdlib.h>

#define MY_LOG_DEBUG 0 /**/

extern OSMutex tmpMutex;
extern FILE *myfp;
char*  myDate(void);
void myLog(const char *fmt,...);
unsigned int  _our_random32();


#endif
