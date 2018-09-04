#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__
#include "TimeoutJob.h"
//objectQueue中最大对象数量
#define MAX_OBJECT_NUM endType

typedef void (*videoCallBack)(unsigned char * frame,int frameLen);

class RTSPClient:public TimeoutJob
{
	public:
		RTSPClient(char *ipBuf,unsigned short port,char *streamName,unsigned long long mSec=1000);
		virtual ~RTSPClient();
		Object *getObjectFromQueue(unsigned int objectType);
		int addObjectToQueue(Object *obj,unsigned int objectType);

		virtual int Processor();
		virtual int Initialize();
		int start();

		int sendOPTIONSCommand();	
        int sendTEARDOWNCommand();
		int sendDESCRIBESCommand();

		int connectToServer();

		int killAll();
		int stopEngine();
				
			
	private:
	private:
		Object *objectQueue[MAX_OBJECT_NUM];
		char mIp[32];
		unsigned short mPort;

		bool initFlags;

		int mChannel;
		char *mStreamName;

};

#endif
