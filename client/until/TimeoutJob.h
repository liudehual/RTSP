#ifndef __TIMEOUT_JOB_H__
#define __TIMEOUT_JOB_H__
#include "Job.h"
class TimeoutJob:public Job
{
	protected:
		TimeoutJob(RTSPClient *rtspClient,
				unsigned long long mSec=1000); /*如果没有指定则默认休眠一秒*/
	public:
		~TimeoutJob();
		
		void setTimeout(unsigned long long mSec); /*修改超时*/
		unsigned long long getTimeout(); /*获取超时*/

		void addCurrentTimeout(unsigned long long t); /*增加当前已过时间*/
		unsigned long long getCurrentTimeout();   /*获取当前已过时间*/
		void refreshCurrentTimeout();	 /*重当前已过时间*/
	private:
		unsigned long long timeout; /*超时*/

		unsigned long long currentTimeout;/*当前已过时间*/
};

#endif

