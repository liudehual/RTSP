#ifndef __TCP_CLIENT_SESSION_H__
#define __TCP_CLIENT_SESSION_H__
#include "TCPSocket.h"
class H264MediaManager;

class TCPClientSession
{
    public:
        TCPClientSession();
        ~TCPClientSession();

        /*获取任务所属通道类的指针*/
        void addMasterChannel(H264MediaManager *Manager){masterChannel=Manager;}
         
        /*设置激活状态*/
        void setActive(unsigned short activeStatus);
		/*在setUp阶段设置激活状态*/
        void setActiveSetUp(unsigned short activeStatus){ifActive=activeStatus;}

        /*设置销毁状态*/
        void setTearDown(unsigned short tearDown){ifTearDown=tearDown;}

        /*设置RTCP socket*/
        void setTCPSocket(TCPSocket *tSocket){mSocket=tSocket;}
          
        /*获取RTP socket*/
        TCPSocket *getTCPSocket(){return mSocket;}
        
        /*获取命令*/
        int getCommand(void){return command;}
        
        /*获取任务ID*/
        char *getSessionId(void){return sessionId;}

		void setSessionId(char *sId)
		{
			if(sId==NULL || strlen(sId)>sizeof(sessionId)){
				return;
			}
			memset(sessionId,0,sizeof(sessionId));
			memcpy(sessionId,sId,strlen(sId));
		}
        /*获取激活状态*/
        unsigned short getActive(void){return ifActive;}
        
        /*获取销毁状态*/
        unsigned short getTearDown(){return ifTearDown;}
        
    private:

         TCPSocket *mSocket;
         char sessionId[20]; //任务ID
         int command;   //命令
         unsigned short ifActive; //激活状态
         unsigned short ifTearDown;//销毁状态
         H264MediaManager *masterChannel; //所属通道
};

#endif
