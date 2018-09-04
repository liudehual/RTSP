#include "RTSPClient.h"
#include <stdio.h>
#include <stdlib.h>
#include "RTSPSession.h"
#include "Engine.h"
#include "RTPSession.h"
#include "SocketHelper.h"
#include "Until.h"
#include "Log.h"
RTSPClient::RTSPClient(char *ipBuf,unsigned short port,char *streamName,unsigned long long mSec):TimeoutJob(this,mSec),
	initFlags(false),mPort(0)
{
	do{
		if(ipBuf==NULL || ipBuf[0]=='\0' || strlen(ipBuf)>sizeof(mIp)){
			fprintf_err("Ip is error\n");
			initFlags=false;
			break;
		}
		if(port<=0 || port>65535){
			fprintf_err("port is error\n");
			initFlags=false;
			break;
		}
		if(streamName==NULL || streamName[0]=='\0'){
			fprintf_err("streamName is error\n");
			initFlags=false;
			break;
		}
		sprintf(mIp,"%s",ipBuf);
		mPort=port;
		mStreamName=new char[128];
		sprintf(mStreamName,"%s",streamName);
		fprintf_msg("RTSPClient RemoteIp %s RemotePort %d mStreamName %s \n",mIp,mPort,mStreamName);
		initFlags=true;
	}while(0);
}

RTSPClient::~RTSPClient()
{
	//delete[] objectQueue;
	
}
Object *RTSPClient::getObjectFromQueue(unsigned int objectType)
{
	if(objectType<RTSPClientObjectType || objectType>=endType){
		return NULL;
	}
	return objectQueue[objectType];
}
int RTSPClient::addObjectToQueue(Object *obj,unsigned objectType)
{
	if(objectType<RTSPClientObjectType || objectType>=endType){
		return 0;
	}
	if(obj==NULL){
		return 0;
	}
	objectQueue[objectType]=obj;
	return 1;
}
int RTSPClient::Processor()
{
	//RTSPSession *session=(RTSPSession *)getObjectFromQueue(RTSPSessionObjectType);
	//if(!session->getRtspSessionStat()){
	//}
	//fprintf(stderr,SIMPLE_RTSP"Hello RTSPClient::Processor\n");

	return 1;
}
int  RTSPClient::Initialize()
{
	if(!initFlags){
		fprintf_err("RTSPClient Not Init,,You can't call Initialize Function\n");
		return 0;
	}

	for(int i=0;i<MAX_OBJECT_NUM;++i){
		objectQueue[i]=NULL;
	}
	
	SocketHelper::Initialize();
	this->addObjectToQueue(this,RTSPClientObjectType);
	this->addObjectToQueue(new Engine(),EngineObjectType);
	this->addObjectToQueue(new RTSPSession(this,mStreamName),RTSPSessionObjectType);
	((Engine *)getObjectFromQueue(EngineObjectType))->addElemToTimeoutQueue(this);

	return 1;
}
int RTSPClient::start()
{
	if(!initFlags){
		fprintf_err("RTSPClient Not Init,You can't call start Function\n");
		return 0;
	}
	#if 0
	this->Initialize();
	#endif
	((Engine *)getObjectFromQueue(EngineObjectType))->start(); //
	return 1;
}
int RTSPClient::sendOPTIONSCommand()
{

	if(!initFlags){
		fprintf_err("[simpleRTSP],RTSPClient Not Init,,You can't call sendOPTIONSCommand Function\n");
		return 0;
	}
	/**/
	SleepBymSec(1000);

	#if 0
	this->Initialize();
	#endif
	RTSPSession *session=(RTSPSession *)getObjectFromQueue(RTSPSessionObjectType);
	if(session==NULL){
		fprintf_err("Not find RTSPSession\n");
		return 0;
	}
	session->sendOPTIONSCommand();
	return 1;
}
int RTSPClient::sendDESCRIBESCommand()
{

	if(!initFlags){
		fprintf_err("[simpleRTSP],RTSPClient Not Init,,You can't call sendOPTIONSCommand Function\n");
		return 0;
	}
	/**/
	SleepBymSec(2000);

#if 0
	this->Initialize();
#endif
	RTSPSession *session=(RTSPSession *)getObjectFromQueue(RTSPSessionObjectType);
	if(session==NULL){
		fprintf_err("Not find RTSPSession\n");
		return 0;
	}
	session->sendDESCRIBECommand();
	return 1;
}
int RTSPClient::sendTEARDOWNCommand()
{
	if(!initFlags){
		fprintf_err("[simpleRTSP],RTSPClient Not Init,,You can't call sendOPTIONSCommand Function\n");
		return 0;
	}
	/**/
	//SleepBymSec(1000);

	#if 0
	this->Initialize();
	#endif
	RTSPSession *session=(RTSPSession *)getObjectFromQueue(RTSPSessionObjectType);
	if(session==NULL){
		fprintf_err("Not find RTSPSession\n");
		return 0;
	}
	session->sendTEARDOWNCommand();
	return 1;
}
int RTSPClient::connectToServer()
{

	if(!((RTSPSession *)getObjectFromQueue(RTSPSessionObjectType))->Initialize(mIp,mPort)){
		fprintf_err("Init RTSPSession is failer\n");
		return 0;
	}
	
	((Engine *)getObjectFromQueue(EngineObjectType))->addEvent((RTSPSession *)getObjectFromQueue(RTSPSessionObjectType),SOCKET_READABLE);

	return 1;
}
int RTSPClient::killAll()
{
	#if 1
	for(int i=0;i<MAX_OBJECT_NUM;++i){
		if(objectQueue[i]!=NULL && i!=RTSPClientObjectType){
			delete objectQueue[i];
			objectQueue[i]=NULL;
		}
	}
	#endif
	return 1;
}
int RTSPClient::stopEngine()
{
	((Engine *)getObjectFromQueue(EngineObjectType))->setRTSPClient(this);
	((Engine *)getObjectFromQueue(EngineObjectType))->end();
	return 1;
}
