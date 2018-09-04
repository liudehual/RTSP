#include "ClientInterface.h"
#include <stdio.h>
#include <stdlib.h>
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Socket.h"
#include "Engine.h"
#include "RTSPClient.h"
#include "Until.h"
#include "H264Session.h"
#include "SocketHelper.h"
#include "H264FramesManager.h"
#if defined(__WIN32__) || defined(_WIN32)
#else
#include <pthread.h>
#endif

#include "Log.h"

/*
	注:live555 server 存在bug 
		特征:当客户端被强制关闭而没能发送tearDown命令时，live555中的资源不会被释放
		仍然想客户端推送数据，会造成客户端十分奇怪的的问题，在此做一记录

*/

static RTSPClient *rClient=NULL;
static bool clientState=false;// false 关闭 true 启动

#if defined(__WIN32__) || defined(_WIN32)
    HANDLE          fThreadID;
#else
    pthread_t       fThreadID;
#endif

#if defined(__WIN32__) || defined(_WIN32)
unsigned int WINAPI _Entry(LPVOID newClient)
#else
void* _Entry(void *newClient)  //static
#endif
{
		
		RTSPClient *client=(RTSPClient *)newClient;
     	//	SleepBymSec(1000);
	    client->start();

    	delete rClient;
    	rClient=NULL;
    	client=NULL;
	    fprintf_debug("Thread is end and we have delete RTSPClient\n");
		//SocketHelper::Initialize();
 		return NULL;
}



void Start(void *client)
{
#if defined(__WIN32__) || defined(_WIN32)
    unsigned int theId = 0; // We don't care about the identifier
    fThreadID = (HANDLE)_beginthreadex( NULL,   // Inherit security
                                        0,      // Inherit stack size
                                        _Entry, // Entry function
                                        (void*)client,    // Entry arg
                                        0,      // Begin executing immediately
                                        &theId );
#else
    pthread_attr_t* theAttrP;
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
    //theAttrP = &sThreadAttr;
    theAttrP = 0;
#else
    theAttrP = NULL;
#endif
    int err = pthread_create((pthread_t*)&fThreadID, theAttrP, _Entry, (void*)client);
#endif
}




int initClient(char *ipBuf,unsigned short port,char *streamName)
{
	   if(ipBuf==NULL || ipBuf[0]=='\0' || strlen(ipBuf)>32){
			fprintf_err("Ip is error\n");
			return 0;
		}
		if(port<=0 || port>65535){
			fprintf_err("port is error\n");
			return 0;
		}
		if(streamName==NULL || streamName[0]=='\0'){
			fprintf_err("streamName is error\n");
			return 0;
		}
		/*创建 RtspClient 实例,并传入参数
			arg1 服务端ip地址
			arg2 服务端端口
			arg3 流名
		*/
		rClient=new RTSPClient(ipBuf,port,streamName);
		if(!rClient->Initialize()){
			fprintf_debug("Initialize RTSPClient Error\n");
			return 0;
		}

		Start((void *)rClient);
		return 1;
}

int startClient(char *ipBuf,unsigned short port,char *streamName)
{
	SocketHelper::Initialize();
	for(int i=0;i<SocketHelper::GetNumIPAddrs();++i){
		fprintf_msg(" local Ip %s\n",SocketHelper::GetIPAddrStr(i)->GetAsCString());
	}
	if(!initClient(ipBuf,port,streamName)){
		return 0;
	}

	if(clientState){
		return 1;
	}
	if(!rClient->connectToServer()){
		fprintf_err("Connect To Server Failer\n");
		return 0;

	}
	clientState=true;

	rClient->sendDESCRIBESCommand();
	return 1;
}
int stopClient()
{
	if(!clientState){
		return 1;
	}
	
	rClient->sendTEARDOWNCommand();
	clientState=false;
#if defined(__WIN32__) || defined(_WIN32)
    DWORD theErr = ::WaitForSingleObject(fThreadID, INFINITE);
#else
    void *retVal;
    pthread_join((pthread_t)fThreadID, &retVal);
#endif	
return 1;
}
int readData(unsigned char **frameBuf,unsigned int *frameLen)
{
	if(frameLen==NULL){
		fprintf_err("Frame Len is error\n");
		return -2;
	}
	if(rClient==NULL){
		return -1;
	}
	H264Session *hSession=((H264Session *)(rClient->getObjectFromQueue(H264ObjectType)));
	if(hSession==NULL){
		return 0;
	}
	hSession->getCacheManager()->readData(*frameBuf,*frameLen);
	return 1;
}

