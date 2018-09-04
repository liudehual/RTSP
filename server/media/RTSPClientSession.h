#ifndef _CLIENTSESSION_H
#define _CLIENTSESSION_H

#include <netinet/in.h>
#include "OSHeaders.h"
#include "UDPSocket.h"
class H264MediaManager;

enum //激活状态
{
    notActive=0,
    active=1
};

enum //销毁状态
{
    notTearDown=0,
    TearDown=1
};

class ClientSession
{
    public:
        ClientSession();
        ~ClientSession();

        /*获取任务所属通道类的指针*/
        void addMasterChannel(H264MediaManager *Manager);
        
        /*设置命令*/
        void setCommand(int comm);
        
        /*设置任务ID*/
        void setSessionId(char *sId);
        
        /*设置激活状态*/
        void setActive(unsigned short activeStatus);
        
        /*在setUp阶段设置激活状态*/
        void setActiveSetUp(unsigned short activeStatus);

        /*设置销毁状态*/
        void setTearDown(unsigned short tearDown);
        
        /*设置RTP 远端地址信息*/
        void setRtpRemoteAddr();
        
        /*设置RTCP 远端地址信息*/
        void setRtcpRemoteAddr();
        
        /*设置远端IP地址*/
        void setAddr(UInt32 addr);
        
        /*设置远端rtp端口*/
        void setRtpPort(UInt16 rPort);
        
        /*设置远端rtp端口*/
        void setRtcpPort(UInt16 rcPort);

        /*获取远端rtp端口*/
        UInt16 getRtpPort();
        
        /*获取远端rtcp端口*/
        UInt16 getRtcpPort();
        
        /*获取远端IP地址*/
        UInt32 getAddr();
        
        /*获取RTP 远端地址信息*/
        struct sockaddr_in *getRtpRemoteAddr();
        
        /*获取RTCP 远端地址信息*/
        struct sockaddr_in *getRtcpRemoteAddr();
        
        /*设置RTCP socket*/
        void setRtpSocket(UDPSocket *rSocket);
        
        /*设置RTCP socket*/
        void setRtcpSocket(UDPSocket* rcSocket);
        
        /*获取RTP socket*/
        UDPSocket *getRtpSocket();
        
        /*获取RTP socket*/
        UDPSocket *getRtcpSocket();
        
        /*获取命令*/
        int getCommand(void);
        
        /*获取任务ID*/
        char *getSessionId(void);
        
        /*获取激活状态*/
        unsigned short getActive(void);
        
        /*获取销毁状态*/
        unsigned short getTearDown();
        
    private:
         struct sockaddr_in rtpRemoteAddr;
         struct sockaddr_in rtcpRemoteAddr;
         UInt32 remoteAddr; //远端IP地址
         UInt16 rtpRemotePort;  //远端 rtp端口
         UInt16 rtcpRemotePort; //远端 rtcp端口
         UDPSocket *rtpSocket;
         UDPSocket *rtcpSocket;
         char sessionId[20]; //任务ID
         int command;   //命令
         unsigned short ifActive; //激活状态
         unsigned short ifTearDown;//销毁状态
         H264MediaManager *masterChannel; //所属通道
};
#endif


