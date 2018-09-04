#ifndef __EVENT_JOB_H__
#define __EVENT_JOB_H__
#include "Job.h"
class Socket;
class EventJob:public Job
{
	protected:
		EventJob(RTSPClient *rtspClient,Socket *tSocket);
	public:
		virtual ~EventJob();
		virtual Socket *getSocket();

	private:
		Socket *fSocket;
};
#endif
