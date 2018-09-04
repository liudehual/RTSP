/*
	屏蔽平台差异头文件
*/
#ifndef _NET_COMMON_H
#define _NET_COMMON_H

//@@@@@@@@@@@@@@@@@@@@@@@@ add by gct 1607131648 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#if defined(__WIN32__) || defined(_WIN32)
#include <time.h>
extern "C" int initializeWinsockIfNecessary();
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#include <process.h>

#else
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#define initializeWinsockIfNecessary() 1
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#if defined(IMN_PIM)
#include "IMN_PIMNetCommon.h"

#elif defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
/* Windows */
//#if defined(WINNT) || defined(_WINNT) || defined(__BORLANDC__) || defined(__MINGW32__) || defined(_WIN32_WCE)
#define _MSWSOCK_
#include <winsock2.h>
#include <ws2tcpip.h>
//#endif
#include <windows.h>
#include <string.h>
#define closeSocket closesocket
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEWOULDBLOCK
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR

#if defined(_WIN32_WCE)
#define NO_STRSTREAM 1
#endif

/* Definitions of size-specific types: */
typedef __int64 int64_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

#elif defined(VXWORKS)
/* VxWorks */
#include <time.h>
#include <timers.h>
#include <sys/times.h>
#include <sockLib.h>
#include <hostLib.h>
#include <resolvLib.h>
#include <ioLib.h>

typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

#else
/* Unix */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <ctype.h>
#if defined(_QNX4)
#include <sys/select.h>
#include <unix.h>
#endif

#define closeSocket close

#ifdef SOLARIS
#define u_int64_t uint64_t
#define u_int32_t uint32_t
#define u_int16_t uint16_t
#define u_int8_t uint8_t
#endif
#endif

#ifndef SOCKLEN_T
#define SOCKLEN_T int
#endif

#endif
