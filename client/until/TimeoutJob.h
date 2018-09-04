#ifndef __TIMEOUT_JOB_H__
#define __TIMEOUT_JOB_H__
#include "Job.h"
class TimeoutJob:public Job
{
	protected:
		TimeoutJob(RTSPClient *rtspClient,
				unsigned long long mSec=1000); /*���û��ָ����Ĭ������һ��*/
	public:
		~TimeoutJob();
		
		void setTimeout(unsigned long long mSec); /*�޸ĳ�ʱ*/
		unsigned long long getTimeout(); /*��ȡ��ʱ*/

		void addCurrentTimeout(unsigned long long t); /*���ӵ�ǰ�ѹ�ʱ��*/
		unsigned long long getCurrentTimeout();   /*��ȡ��ǰ�ѹ�ʱ��*/
		void refreshCurrentTimeout();	 /*�ص�ǰ�ѹ�ʱ��*/
	private:
		unsigned long long timeout; /*��ʱ*/

		unsigned long long currentTimeout;/*��ǰ�ѹ�ʱ��*/
};

#endif

