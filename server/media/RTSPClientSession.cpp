#include "RTSPClientSession.h"
#include <string.h>
#include "SafeStdLib.h"
#include "RTSPMediaManager.h"

ClientSession::ClientSession()
{
    ifActive=notActive;
    ifTearDown=notTearDown;
    rtpSocket=NULL;
    rtcpSocket=NULL;
}

ClientSession::~ClientSession()
{
   if(rtcpSocket!=NULL){
		 rtcpSocket->Cleanup();
		//fprintf(stderr,"%s %d Delete rtcpSocket\n",__FUNCTION__,__LINE__);

        delete rtcpSocket;
        rtcpSocket=NULL;
   }
   if(rtpSocket!=NULL){
		//fprintf(stderr,"%s %d Delete rtpSocket\n",__FUNCTION__,__LINE__);
	     rtpSocket->Cleanup();

        delete rtpSocket;
        rtpSocket=NULL;
   }
}

void ClientSession::addMasterChannel(H264MediaManager *Manager)
{
   if(Manager!=NULL){
        masterChannel=Manager;
   }     
}
void ClientSession::setCommand(int comm)
{
    command=comm;
}

void ClientSession::setSessionId(char * sId)
{
    if(strlen(sId)>19){
        return;
    }
    memcpy(sessionId,sId,strlen(sId)+1);
}

int ClientSession::getCommand(void)
{
    return command;
}

char *ClientSession::getSessionId(void)
{
    return sessionId;
}

void ClientSession::setActive(unsigned short activeStatus)
{
    ifActive=activeStatus;
    if(ifActive==active){
    	if(masterChannel==NULL){
			return;
    	}
        masterChannel->addActiveNum();  //任务启动，增加通道管理类中的激活任务数
    }
    if(ifActive==notActive){
    	if(masterChannel==NULL){
			return;
    	}
        masterChannel->subActiveNum();  //任务启动，增加通道管理类中的激活任务数
    }
}
void ClientSession::setActiveSetUp(unsigned short activeStatus) 
{
    ifActive=activeStatus;
}
unsigned short ClientSession::getActive(void)
{
    return ifActive;
}

void ClientSession::setTearDown(unsigned short tearDown)
{
    ifTearDown=tearDown;
}

unsigned short ClientSession::getTearDown()
{
    return ifTearDown;
}

void ClientSession::setRtpSocket(UDPSocket *rSocket)
{
    rtpSocket=rSocket;
}

void ClientSession::setRtcpSocket(UDPSocket* rcSocket)
{
    rtcpSocket=rcSocket;       
}

UDPSocket *ClientSession::getRtpSocket()
{
    return rtpSocket;
}

UDPSocket *ClientSession::getRtcpSocket()
{
    return rtcpSocket;
}

void ClientSession::setRtpRemoteAddr()
{
    socklen_t len = sizeof(rtpRemoteAddr);
    ::memset(&rtpRemoteAddr, 0, sizeof(rtpRemoteAddr));
    rtpRemoteAddr.sin_family = AF_INET;
    rtpRemoteAddr.sin_port = htons(rtpRemotePort);
    rtpRemoteAddr.sin_addr.s_addr = htonl(remoteAddr);
}

void ClientSession::setRtcpRemoteAddr(void)
{
    socklen_t len = sizeof(rtcpRemoteAddr);
    ::memset(&rtcpRemoteAddr, 0, sizeof(rtcpRemoteAddr));
    rtcpRemoteAddr.sin_family = AF_INET;
    rtcpRemoteAddr.sin_port = htons(rtcpRemotePort);
    rtcpRemoteAddr.sin_addr.s_addr = htonl(remoteAddr);
}

struct sockaddr_in *ClientSession::getRtpRemoteAddr()
{
    return &rtpRemoteAddr;
}

struct sockaddr_in *ClientSession::getRtcpRemoteAddr()
{
    return &rtcpRemoteAddr;
}

void ClientSession::setAddr(UInt32 addr)
{
    remoteAddr=addr;
}

UInt32 ClientSession::getAddr()
{
    return remoteAddr;
}

void ClientSession::setRtpPort(UInt16 rPort)
{
    rtpRemotePort=rPort;
}
void ClientSession::setRtcpPort(UInt16 rcPort)
{
    rtcpRemotePort=rcPort;
}

UInt16 ClientSession::getRtpPort()
{
    return rtpRemotePort;
}
UInt16 ClientSession::getRtcpPort()
{
    return rtcpRemotePort;
}
