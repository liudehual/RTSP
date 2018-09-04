/*
	server 0.1 (nvr 实测可用 )
	作者:gct
	时间:1608101057
	功能:实现h264视频rtp打包,并实现网络传输
	
*/

#include "RTSPListener.h"
#include "RTSPParserSession.h"
Task*   RtspListener::GetSessionTask(TCPSocket** outSocket)
{
    Task * p = new RtspSession;
    *outSocket = ((RtspSession *)p)->GetSocket();
    return p;
}
OS_Error  RtspListener::Initialize(UInt32 addr, UInt16 port)
{
	TCPListenerSocket::Initialize(addr,port);
}
