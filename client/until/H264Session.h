#ifndef __TIMEOUT_SESSION_H__
#define __TIMEOUT_SESSION_H__
#include "TimeoutJob.h"
class RTPSession;
class CacheManager;
class H264Session:public TimeoutJob
{
	public:
		H264Session(RTSPClient *rtspClient,unsigned long long mSec);
		~H264Session();
		virtual int Processor();
		int setRTPSession(RTPSession *session);
		CacheManager *getCacheManager(){return cManager;}
	private:
		RTPSession *fSession;
		CacheManager *cManager;
};
#endif
