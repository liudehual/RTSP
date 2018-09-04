#include "HeartBeatSession.h"
#include "RTSPSession.h"
#include "RTSPClient.h"
#include "Log.h"
HeartBeatSession::HeartBeatSession(RTSPClient *rtspClient,unsigned long long mSec):TimeoutJob(rtspClient,mSec)
{

}
HeartBeatSession::~HeartBeatSession()
{

}
int HeartBeatSession::Processor()
{
#if 0
	static bool first=true;
	if(first){
		first=false;
		return 1;
	}
#endif
	RTSPSession * session=((RTSPSession *)(client->getObjectFromQueue(RTSPSessionObjectType)));
	if(session==NULL){
		fprintf_warn("HeartBeatSession not find RTSPSession\n");
		return 1;
	}
	session->sendGET_PARAMETERCommand();

	return 1;
}
