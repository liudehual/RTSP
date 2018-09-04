/*


*/
#ifndef __UNTIL_H__
#define __UNTIL_H__

#include "NetCommon.h"
char* strDup(char const* str);
char* strDupSize(char const* str);
void SleepBymSec(unsigned int inMsec);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(IMN_PIM)
// For Windoze, we need to implement our own gettimeofday()
	int gettimeofday(struct timeval*, int*);
#endif
#endif
