#ifndef __TCP_CLIENT_SESSION_H__
#define __TCP_CLIENT_SESSION_H__
#include "TCPSocket.h"
class H264MediaManager;

class TCPClientSession
{
    public:
        TCPClientSession();
        ~TCPClientSession();

        /*��ȡ��������ͨ�����ָ��*/
        void addMasterChannel(H264MediaManager *Manager){masterChannel=Manager;}
         
        /*���ü���״̬*/
        void setActive(unsigned short activeStatus);
		/*��setUp�׶����ü���״̬*/
        void setActiveSetUp(unsigned short activeStatus){ifActive=activeStatus;}

        /*��������״̬*/
        void setTearDown(unsigned short tearDown){ifTearDown=tearDown;}

        /*����RTCP socket*/
        void setTCPSocket(TCPSocket *tSocket){mSocket=tSocket;}
          
        /*��ȡRTP socket*/
        TCPSocket *getTCPSocket(){return mSocket;}
        
        /*��ȡ����*/
        int getCommand(void){return command;}
        
        /*��ȡ����ID*/
        char *getSessionId(void){return sessionId;}

		void setSessionId(char *sId)
		{
			if(sId==NULL || strlen(sId)>sizeof(sessionId)){
				return;
			}
			memset(sessionId,0,sizeof(sessionId));
			memcpy(sessionId,sId,strlen(sId));
		}
        /*��ȡ����״̬*/
        unsigned short getActive(void){return ifActive;}
        
        /*��ȡ����״̬*/
        unsigned short getTearDown(){return ifTearDown;}
        
    private:

         TCPSocket *mSocket;
         char sessionId[20]; //����ID
         int command;   //����
         unsigned short ifActive; //����״̬
         unsigned short ifTearDown;//����״̬
         H264MediaManager *masterChannel; //����ͨ��
};

#endif
