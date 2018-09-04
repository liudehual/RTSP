/*

*/
#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include "Object.h"
#include "NetCommon.h"
#include "ObjectQueue.h"

class Timeval;
class TimeManager;
class RTSPClient;
class Engine:public Object
{
	public:
		Engine();
		~Engine();
		void start();
		void end();

		int addEvent(void *object,unsigned int conditionSet);
		int delEvent(void *object,unsigned int conditionSet=0);

		int addElemToTimeoutQueue(void *object);
		int delElemFromTimeoutQueue(void *object);
		void setRTSPClient(RTSPClient *tClient){client=tClient;}
	private:
		void dispatch();
	private:
		fd_set fReadSet;
		fd_set fWriteSet;
		fd_set fExceptionSet;

		int fMaxNumSockets;
		bool runFlags;

		ObjectQueue eventQueue;
		TimeManager *mManager;
		RTSPClient *client;
};


#endif
