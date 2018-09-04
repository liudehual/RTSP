#include "EventJob.h"
#include "Socket.h"
EventJob::EventJob(RTSPClient *rtspClient,Socket *tSocket):Job(rtspClient),fSocket(tSocket)
{
}
EventJob::~EventJob()
{
	if(fSocket!=NULL){
		delete fSocket;
		fSocket=NULL;
	}
}
Socket *EventJob::getSocket()
{
	return fSocket;
}
