/*
*/
#ifndef __JOB_H__
#define __JOB_H__
#include "Object.h"
#include "ObjectQueue.h"
class RTSPClient;
class Job:public Object
{
	protected:
		Job(RTSPClient *rtspClient=NULL);
	public:
		virtual ~Job();
		virtual int Processor()=0;
		ObjectQueueElem *getJobElem(){return &jobElem;}
	protected:
		ObjectQueueElem jobElem;
	protected:
		RTSPClient *client;
};

#endif
