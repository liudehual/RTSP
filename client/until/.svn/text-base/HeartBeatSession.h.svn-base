#ifndef __HEART_BEAT_SESSION_H__
#define __HEART_BEAT_SESSION_H__
#include "TimeoutJob.h"
class HeartBeatSession:public TimeoutJob
{
	public:
		HeartBeatSession(RTSPClient *rtspClient,unsigned long long mSec=1000*30);
		~HeartBeatSession();
		virtual int Processor();
};

#endif

