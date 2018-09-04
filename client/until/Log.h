#ifndef __LOG_H__
#define __LOG_H__

/***************************************************************
* 该文件主要用于 开启全部注释或开启/关闭某一个级别的注释
*	使用本log只需包含该头文件即可
****************************************************************/

#define ERR 		
#define WARN 		
#define MSG 		
#define BUG 

#ifdef ERR
#define fprintf_err(fmt,...) \
	do \
		fprintf(stderr,"[SIMPLE_ERR] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_err(fmt,...)
#endif

#ifdef WARN
#define fprintf_warn(fmt,...) \
	do \
		fprintf(stderr,"[SIMPLE_WARN] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_warn(fmt,...)
#endif

#ifdef MSG
#define fprintf_msg(fmt,...) \
	do \
		fprintf(stderr,"[SIMPLE_MSG] "fmt,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_msg(fmt,...)
#endif

#ifdef BUG
#define fprintf_debug(fmt,...) \
	do \
		fprintf(stderr,"[SIMPLE_DEBUG] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_debug(fmt,...)
#endif

#endif
