/*
	server 0.1 (nvr ʵ����� )
	����:gct
	ʱ��:1608101057
	����:ʵ��h264��Ƶrtp���,��ʵ�����紫��
	
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
