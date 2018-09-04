/* 
============================================================================================
	跨平台锁实现

	add 2016.09.23 by GCT

	Copyright (c), ...	

=============================================================================================
*/

#ifndef _OSMUTEX_LOCKER_H
#define _OSMUTEX_LOCKER_H

#if defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif
	class OSMutex
	{
	public:
		OSMutex();
		~OSMutex();

		// 加锁 不是同一线程枷锁会阻塞
		void Lock();

		// 解锁
		void Unlock();

	protected:
#if defined(__WIN32__) || defined(_WIN32)
		CRITICAL_SECTION	m_critical_section;	// 临界区
#else
		pthread_mutex_t 	m_mutex;			// 互斥锁
#endif
};


#endif
