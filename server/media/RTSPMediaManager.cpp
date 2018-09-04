#include "RTSPMediaManager.h"
#include "H264Rtp.h"
#include <unistd.h>
#include <sys/uio.h>
#include "TCPClientSession.h"
H264MediaManager *H264MediaManager::cManager[MAX_CHANNEL_NUM]={NULL};
unsigned int H264MediaManager::activeChannelNum=0;
int H264MediaManager::runChannelNum=0;

H264MediaManager::H264MediaManager():Task()
{
     for(int i=0;i<MAX_CONNECT;++i){
        cSession[i]=NULL;
     }
     for(int i=0;i<MAX_CONNECT;++i){
        tSession[i]=NULL;
     }
	 TCPClientSessionNum=0;
	 TCPClientActiveCSessionNum=0;
     
     clientSessionNum=0;
     activeCSessionNum=0;
     sps=NULL;
     spsSize=0;
     pps=NULL;
     ppsSize=0;
     channelID=-1;
     caManager=new CacheManager;
     fRTPPacket=new H264RtpBuffer;
     Signal(Task::kTimeoutEvent); //�����������
}
H264MediaManager::~H264MediaManager()
{

    if(fRTPPacket!=NULL){
        delete fRTPPacket;
        fRTPPacket=NULL;
    }
    if(sps!=NULL){
        delete[] sps;
        sps=NULL;
    }
    if(pps!=NULL){
        delete[] pps;
        pps=NULL;
    }
}
void H264MediaManager::killSelf()
{
	Signal(Task::kKillEvent);
}
unsigned short sCheck()
{
	unsigned short i=1;
	if(((char*)&i)[0]==0)	
		return 1;
	else 
		return 0;
}
unsigned short p_Htons(unsigned short x)
{
	if(!sCheck())
	{
		unsigned char *s = (unsigned char *)&x;
		return (unsigned short)(s[0] << 8 | s[1]);
	}
	return x;
}
SInt64 H264MediaManager::Run()
{
    static int sendPacketNum=0;
    static bool i=false;
    static int timeout=39;

	static  iovec theVec[2];
	static  unsigned char tcpHeader[4];
	
	
	EventFlags events = this->GetEvents();
	if (events & Task::kKillEvent){
        fprintf(stderr,"%s %d quit \n",__FUNCTION__,__LINE__);
		return -1;
	}
    do{
    	#if 0
        if(clientSessionNum==0 || activeCSessionNum==0){ //�ͻ���������Ϊ��򼤻��������Ϊ�㣬50��������
            timeout=39;
            break;
        }
        #endif

        if(activeCSessionNum==0  // udp������������Ϊ��
             && TCPClientActiveCSessionNum==0){ //tcp�����������Ϊ��
        timeout=39;
            break;
        }
        


       /*��֡������*/
        #if 1
        unsigned char *fData=NULL;
        unsigned int frameSize=0;
        int flag=caManager->readData(fData,frameSize);
		//fprintf(stderr,"H264MediaManager::Run %s %d channelId %d %d %d\n",__FUNCTION__,__LINE__,channelID,flag,frameSize);

        if(frameSize==0){ //δ��ȡ��֡����
           timeout=16;
           break;
        }
        #endif
        fRTPPacket->addFrameToBuffer(fData,frameSize); //��֡���ݴ����RTP����
        RtpPacket *packet=NULL;
    	int len;
		while((packet = fRTPPacket->getRtpPacket())!=NULL){ //�������л�ȡRTP��

/*TCP��������*/
#if 1
			tcpHeader[0]=0x24;
			tcpHeader[1]=0x00;
			unsigned short dataLen=packet->getPacketLen();
			unsigned char *s = (unsigned char *)&dataLen;
			dataLen=(unsigned short)(s[0] << 8 | s[1]);
			memcpy((char *)&tcpHeader[2],(char *)&dataLen,2);
			theVec[0].iov_base=(char *)tcpHeader;
			theVec[0].iov_len=4;
			theVec[1].iov_base=(char *)packet->getPacketBufStart();
			theVec[1].iov_len=packet->getPacketLen();
			
	        for(int i=0;i<MAX_CONNECT;++i){ //��TCP�ͻ��˷�������
				TCPClientSession *runSession=tSession[i]; 
                if(runSession==NULL){ //������
                    continue;
                }
                if(runSession->getTearDown()){ //��������
                //	fprintf(stderr,"TearDown %p\n",runSession);
                   // delSession(i);
                    continue;
                }
                if(runSession->getActive() 
                	&& runSession->getTCPSocket()!=NULL){//���񼤻�
					unsigned int outLen;
					if(!runSession->getTCPSocket()->IsConnected()){ /*���ӶϿ�*/
						runSession->setTearDown(TearDown);
						tSession[i]=NULL;
						subActiveTCPClientSessionNum();
						continue;
					}
					int flags=runSession->getTCPSocket()->WriteV(theVec,2,&outLen);
					//fprintf(stderr,"Send Data flags %d Len %d ConnectState %d\n",flags,outLen,runSession->getTCPSocket()->IsConnected());
				}
	        }
  #endif
  /*UDP��������*/
  #if 1
            for(int i=0;i<MAX_CONNECT;++i){ //��ͻ��˷�������
                ClientSession *runSession=cSession[i]; 
                if(runSession==NULL){ //������
                    continue;
                }
                if(runSession->getTearDown()){ //��������
                //	fprintf(stderr,"TearDown %p\n",runSession);
                    delSession(i);
                    continue;
                }
                if(runSession->getActive() && runSession->getRtpSocket()!=NULL){//���񼤻�
                    runSession->getRtpSocket()->SendTo(runSession->getAddr(), runSession->getRtpPort(),packet->getPacketBufStart(), packet->getPacketLen()); //������Ƶ����
                }
           }
   #endif
		}
       timeout=10;        
    }while(0);
    
    fRTPPacket->reset();
    return timeout;
}

int H264MediaManager::addSession(ClientSession *newSession)
{
	//fprintf(stderr,"____________ %s %d %d __________\n",__FUNCTION__,__LINE__,clientSessionNum);
    if(clientSessionNum>=MAX_CONNECT){ //ͨ������������
        return -1;
    }
    for(int i=0;i<MAX_CONNECT;++i){//���������б�
        if(cSession[i]!=NULL){
            continue;
        }
        newSession->addMasterChannel(this); //��ȡ����������ͨ��
        newSession->setActiveSetUp(notActive); //����������δ����
        newSession->setTearDown(notTearDown); //��������������
        cSession[i]=newSession;
        clientSessionNum++; //��������񣬿ͻ�����������һ
        return 1;
    }
    return 0;
}
int H264MediaManager::addTcpClientSession(TCPClientSession *newSession)
{
	//fprintf(stderr,"_ %s %d %d _\n",__FUNCTION__,__LINE__,clientSessionNum);
    if(TCPClientSessionNum>=MAX_CONNECT){ //ͨ������������
        return -1;
    }
    for(int i=0;i<MAX_CONNECT;++i){//���������б�
        if(tSession[i]!=NULL){
            continue;
        }
        newSession->addMasterChannel(this); //��ȡ����������ͨ��
        newSession->setActiveSetUp(notActive); //����������δ����
        newSession->setTearDown(notTearDown); //��������������
        tSession[i]=newSession;
        TCPClientSessionNum++; //��������񣬿ͻ�����������һ
        return 1;
    }
    return 0;
}

int H264MediaManager::delSession(int index)
{
    if(index<0){
        fprintf(stderr,"[NET H264MediaManager::%s----- error index >1 && idex <=32]\n",__FUNCTION__);
        return -1;
    }
    ClientSession *dSession=cSession[index];
    cSession[index]=NULL;
    if(dSession->getActive()==active){ //����������Ǽ���ģ�����ټ���������� 
       subActiveNum();
    }
    delete dSession;
    clientSessionNum--;
}

int H264MediaManager::delSession(ClientSession *toDelSession,int index)
{
    if(index==0){
        fprintf(stderr,"[NET H264MediaManager::%s ----- error index >1 && idex <=32]\n",__FUNCTION__);
        return -1;
    }
    if(toDelSession->getActive()==active){  
        subActiveNum();
    }
    delete toDelSession;
    cSession[index]=NULL;
    clientSessionNum--;
}

/*������ɾ������*/
int H264MediaManager::delTCPClientSession(int index)
{
	 if(index<0){
        fprintf(stderr,"[NET H264MediaManager::%s----- error index >1 && idex <=32]\n",__FUNCTION__);
        return -1;
    }
    TCPClientSession *dSession=tSession[index];
    tSession[index]=NULL;
    if(dSession->getActive()==active){ //����������Ǽ���ģ�����ټ���������� 
       subActiveTCPClientSessionNum();
    }
    delete dSession;
    TCPClientSessionNum--;

}
int H264MediaManager::delTCPClientSession(ClientSession *toDelSession,int index)
{
	 if(index==0){
        fprintf(stderr,"[NET H264MediaManager::%s ----- error index >1 && idex <=32]\n",__FUNCTION__);
        return -1;
    }
    if(toDelSession->getActive()==active){  
        subActiveTCPClientSessionNum();
    }
    delete toDelSession;
    tSession[index]=NULL;
    TCPClientSessionNum--;
}

CacheManager *H264MediaManager::getCacheManager(void)
{
	return caManager;
}

H264MediaManager ** H264MediaManager::getChannel()
{
    return cManager;
}

void H264MediaManager::addActiveChannelNum()
{
    activeChannelNum++;
}

void H264MediaManager::subActiveChannelNum()
{
    if(activeChannelNum > 0){
        activeChannelNum--;
    }
}

int H264MediaManager::getActiveChannelNum()
{
    return activeChannelNum;
}

void H264MediaManager::addActiveNum()
{
     activeCSessionNum++;
}

void H264MediaManager::subActiveNum()
{
    if(activeCSessionNum>0){
        activeCSessionNum--;
    }
}

void H264MediaManager::addActiveTCPClientSessionNum()
{
    TCPClientActiveCSessionNum++;
}

void H264MediaManager::subActiveTCPClientSessionNum()
{
    if(TCPClientActiveCSessionNum>0){
        TCPClientActiveCSessionNum--;
    }
}

char* H264MediaManager::getTrack1()
{
   return "track1";
}

char* H264MediaManager::getTrack2()
{
   return "track=1";
}



int H264MediaManager::setH264SPS(unsigned char *data,int framesize)
{
    if(sps==NULL){
        sps=new uint8_t[framesize];
        memcpy(sps,data,framesize);
        spsSize=framesize;
    }
   // fprintf(stderr,"SPS Len %d\n",spsSize);
    return 1;
}

int H264MediaManager::setH264PPS(unsigned char *data,int framesize)
{
    if(pps==NULL){
        pps=new uint8_t[framesize];
        memcpy(pps,data,framesize);
        ppsSize=framesize;
    //    fprintf(stderr,"PPS Len %d\n",ppsSize);
    }
   return 1;
}
bool H264MediaManager::initH264SPS()
{
	if(sps==NULL){
		return false;
	}
	return true;
}
bool H264MediaManager::initH264PPS()
{
	if(pps==NULL){
		return false;
	}
	return true;
}
