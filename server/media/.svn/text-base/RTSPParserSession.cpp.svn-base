#include "RTSPParserSession.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "TCPSocket.h"
#include <netinet/in.h>
#include <netinet/tcp.h> // for TCP_NODELAY
#include "RTSPMediaManager.h"
#include "UDPSocket.h"
#include "strDup.h"
#include <stdint.h>
#include "base64.h"
#include "OSMutex.h"
#include "myMutex.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>  
#include <ifaddrs.h>  
#include <arpa/inet.h> 
#include "strDup.h"
#include "myUntil.h"
#include "netInfo.h"
#include "TCPClientSession.h"
int RtspSession::RtspSessionLiveNum=0;

RtspSession::RtspSession():Task()
{
    initRtspSession();
    char netmask[32]={0};
    char mac[32]={0};
    sprintf(localName,"%s","eth0");
    GetLocalNetInfo(localName,localIp,netmask,mac);
}
RtspSession::~RtspSession()
{
  for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
    ClientSession *session=cSession[i];
    if(session!=NULL){
        session->setTearDown(TearDown);
    }
  }
 // fprintf(stderr,"%s %d Delete tcpSocket %d\n",__FUNCTION__,__LINE__,fsocket->GetSocketFD());

  fsocket->Cleanup();
  delete fsocket;


  fsocket=NULL;
}
void RtspSession::initRtspSession(void)
{
    fsocket=new TCPSocket(NULL,0); //初始化socket 此处在RTSP作为服务器时使用

    connectNumbers=0;
    for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
        cSession[i]=NULL;
    }  
    for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
        tSession[i]=NULL;
    }  
    cManager=H264MediaManager::getChannel();//初始化指向通道列表的指针
    resetRequestBuffer();   //重置接收缓冲区
    SetSeed(PickSeed()); //初始化随机函数

    Socket=0;
    timeInterval=0;     //任务调用时间间隔(因为任务是事件任务，所以返回零，等待事件)  

}
void RtspSession::setServerInfo(char *ip,int port)
{
    memcpy(serverIp,ip,strlen(ip));
    serverPort=port;
}
void RtspSession::killSelf()
{
	Signal(Task::kKillEvent);
}

SInt64 RtspSession::Run()
{
   static int flag=0;
   EventFlags events = this->GetEvents();
   if (events & Task::kKillEvent){
        fprintf(stderr,"%s %d quit Socket FD %d\n",__FUNCTION__,__LINE__,fsocket->GetSocketFD());
		return -1;
   }
       
   UInt32 rOutLen=0;
   int theErr=fsocket->Read(&requestBuffer[fRequestBytesAlreadySeen],fRequestBufferBytesLeft,&rOutLen);
   //fprintf(stderr,"%s %d tcpSocket %d\n",__FUNCTION__,__LINE__,fsocket->GetSocketFD());

	fprintf(stderr,"Read OutLen %d\n",rOutLen);
   if(theErr != OS_NoErr && !fsocket->IsConnected()){ //连接断开
      killSelf();
   }
   handleRequest(rOutLen); //处理rtsp命令
  
   fsocket->RequestEvent(EV_RE); //设置事件   
   return 0;
}

bool RtspSession::handleRequest(int rOutLen)
{
     int numBytesRemaining = 0;
     do{
        //请求缓冲剩余空间不足
        if ((unsigned)rOutLen >= fRequestBufferBytesLeft){break;}
        bool endOfMsg = false;

        //ptr指向本次接收到的数据的开始位置
        unsigned char* ptr = &requestBuffer[fRequestBytesAlreadySeen];

        //查找请求头结尾: <CR><LF><CR><LF>
        unsigned char *tmpPtr = fLastCRLF + 2;
        if (tmpPtr < requestBuffer){
            tmpPtr = requestBuffer;
        }
        while (tmpPtr < &ptr[rOutLen-1]){
            if (*tmpPtr == '\r' && *(tmpPtr+1) == '\n'){
                if (tmpPtr - fLastCRLF == 2){
                    //查找到请求头结尾
                    endOfMsg = true;
                    break;
                   }
                fLastCRLF = tmpPtr;
            }
            ++tmpPtr;
       }

      //修改请求缓冲剩余空间及后续接收到的消息的开始位置
      fRequestBufferBytesLeft -= rOutLen;
      fRequestBytesAlreadySeen += rOutLen;

      //请求头尚未接收完成，后续的请求字节将完成该部分
      if (!endOfMsg){
          break;
        }

      //解析请求指令头中的指令名和CSeq,并处理相应指令
      requestBuffer[fRequestBytesAlreadySeen] = '\0';
         
      char cmdName[RTSP_PARAM_STRING_MAX]={0};
      char urlPreSuffix[RTSP_PARAM_STRING_MAX]={0};
      char urlSuffix[RTSP_PARAM_STRING_MAX]={0};
      char cseq[RTSP_PARAM_STRING_MAX]={0};
      char sessionIdStr[RTSP_PARAM_STRING_MAX]={0};
      char playBackDate[RTSP_PARAM_STRING_MAX]={0};
      int type;
      int setDvsChannel;
      unsigned contentLength = 0;
      fLastCRLF[2] = '\0'; //临时修改为0，解析用
      
 	  fprintf(stderr,"%s\n",requestBuffer);

      //解析RTSP指令请求头中各域
      bool parseSucceeded = parseRTSPRequestString((char*)requestBuffer, fLastCRLF+2 - requestBuffer,
                                                   cmdName, sizeof cmdName,
                                                   urlPreSuffix, sizeof urlPreSuffix,
                                                   urlSuffix, sizeof urlSuffix,
                                                   cseq, sizeof cseq,
                                                   sessionIdStr, sizeof sessionIdStr,
                                                   contentLength,type,playBackDate,setDvsChannel);
      fLastCRLF[2] = '\r';
      if(parseSucceeded){
            if (ptr + rOutLen< tmpPtr + 2 + contentLength){
	                //当前接收到的请求信息小于头域中contentLength指示的长度,
                    //继续等待读取后续请求信息
	                break;
            }
            fCurrentCSeq=cseq;
            //查找相关命令
            if (strcmp(cmdName, "OPTIONS") == 0){ 
                    handleCmd_OPTIONS();
            }else if (strcmp(cmdName, "DESCRIBE") == 0){
            		fprintf(stderr,"<<<<<<<<<<< urlPreSuffix %s urlSuffix %s >>>>>>>>>>>>>>>>\n",
            							urlPreSuffix,
            							urlSuffix);
		            handleCmd_DESCRIBE(urlPreSuffix,urlSuffix,(char *)requestBuffer);
            }else if (strcmp(cmdName, "SETUP") == 0){
		            handleCmd_SETUP(urlPreSuffix,urlSuffix,(char *)requestBuffer);
            }else if (strcmp(cmdName, "PLAY") == 0){
                   int channel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号
		           handleCmd_PLAY(channel,sessionIdStr,(char *)requestBuffer);
            }else if (strcmp(cmdName, "TEARDOWN") == 0){
		            handleCmd_TEARDOWN(sessionIdStr);
            }else if (strcmp(cmdName, "PAUSE") == 0){
          //  fprintf(stderr,"Session ID is %s\n",sessionIdStr);
		            handleCmd_PAUSE(sessionIdStr);
            }else if (strcmp(cmdName, "YTCRTL") == 0){
                    int channel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号
		            handleCmd_YTCTRL(channel,type,sessionIdStr);
            }else if (strcmp(cmdName, "GETDVS") == 0){
                    int channel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号
                    handleCmd_GETDVS(channel, cmdName,cseq,sessionIdStr);
            }else if (strcmp(cmdName, "SETDVS") == 0){
                 //   int channel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号
                    if(playBackDate[0]=='\0'){
                        handleCmd_SETDVS(setDvsChannel,sessionIdStr,type);
                    }else{
                        handleCmd_SETDVS_PlayBack(setDvsChannel,sessionIdStr,type,playBackDate);
                    }
            }else if (strcmp(cmdName, "GET_PARAMETER") == 0){ 
		            handleCmd_GET_PARAMETER();
            }else{
		            handleCmd_notSupported();
            }
      }
      
      //响应包组建完毕，发送数据
      UInt32 sOutLen;
      fprintf(stderr,"---> %s\n",responseBuffer);

      int theErr=fsocket->Send((char *)responseBuffer,strlen((char *)responseBuffer),&sOutLen);
     if(!fsocket->IsConnected() && theErr != OS_NoErr){
	      killSelf();
	      return false;
     }
     memset(responseBuffer,0,RESPONSE_BUFFER_SIZE); //发送完毕，清空响应buffer

      //检查客户端请求缓冲中是否有剩余字节,如果有,将剩余字节移动到缓冲区开始位置
      unsigned requestSize = (fLastCRLF+4-requestBuffer) + contentLength;
      numBytesRemaining = fRequestBytesAlreadySeen - requestSize;
      resetRequestBuffer();

      if (numBytesRemaining > 0){ //将请求缓冲区内的数据移动至开头
          memmove(requestBuffer, &requestBuffer[requestSize], numBytesRemaining);
          rOutLen= numBytesRemaining;
        }
      }while(numBytesRemaining>0);
     return true;
}

//重置请求缓冲
void RtspSession::resetRequestBuffer()
{
  fRequestBytesAlreadySeen = 0; //
  fRequestBufferBytesLeft =REQUEST_BUFFER_SIZE;
  fLastCRLF = &requestBuffer[-3];
}

void RtspSession::handleCmd_OPTIONS()
{
    char options[]="OPTIONS,DESCRIBE,SETUP,TEARDOWN,PLAY,PAUSE,GET_PARAMETER,SET_PARAMETER";

    snprintf((char *)responseBuffer,RESPONSE_BUFFER_SIZE,"RTSP/1.0 200 OK\r\n"
    													 "CSeq: %s\r\n"
    													 "%s"
    													 "Public: %s\r\n\r\n",
    													 fCurrentCSeq,
    													 this->dateHeader(),
    													 allowedCommandNames());
}

void  RtspSession::handleCmd_GET_PARAMETER()
{
    char responseStr[]="200 OK";
    snprintf((char *)responseBuffer,RESPONSE_BUFFER_SIZE,"RTSP/1.0 %s\r\n"
    													 "CSeq:%s\r\n"
    													 "%s\r\n",
    													 responseStr,
    													 fCurrentCSeq,
    													 dateHeader());
}

void RtspSession::handleCmd_DESCRIBE(char * urlPreSuffix,char * urlSuffix, char * fullRequestStr)
{
    unsigned int pos=0;
    char content[10*1024]={0};
    char fix[20]={0};
    char *ptr=content;
    int sdpLineNumbers=0;
    int channel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号
    if(channel>=0 && channel<MAX_CHANNEL_NUM){
        if(!findChannelByIndex(channel)){ //对应通道未开启，出错返回
            handleCmd_notFound();
            return;
        }
        if(!getSDP(getChannelByIndex(channel),channel,content)) return;
    }
    sprintf((char *)responseBuffer,"RTSP/1.0 200 OK\r\n"
    							   "CSeq:%s\r\n"
    							   "%s"
    							   "ChannelNum:%d\r\n"
    							   "Content-Length: %d\r\n\r\n"
    							   "%s",
    							   fCurrentCSeq,
    							   dateHeader(),
    							   H264MediaManager::getActiveChannelNum(),
    							   strlen(content),
    							   content);
}

bool RtspSession::findChannelByIndex(int index)
{
	if(cManager[index]==NULL) 
		return false;
	return true;
}

H264MediaManager *RtspSession::getChannelByIndex(int index)
{
	return cManager[index];
}

int RtspSession::getSDP(H264MediaManager *manager,int channel,char *sdp)
{
        char *ptr=sdp;
               
        unsigned int spsLen=0;
        unsigned int ppsLen=0;
        unsigned char *sps=NULL;
        //sps=manager->getSPS(spsLen); //获取sps信息
        unsigned char *pps=NULL;
        //pps=manager->getPPS(ppsLen); //获取pps信息
        if(sps==NULL || pps==NULL){
            sps=(unsigned char *)"";
            spsLen=strlen("");
            pps=(unsigned char *)"";
            ppsLen=strlen("");
        }
        char base64SPS[1024]={0};
        char base64PPS[1024]={0};
        uint32_t profileId=getProfileId(sps,spsLen);;
#if 0

        Base64encode(base64SPS,(char *)&sps[4],(int)spsLen-4); //编码
        Base64encode(base64PPS,(char *)&pps[4],(int)ppsLen-4); //编码
        uint32_t profileId=getProfileId(sps,spsLen);
        fprintf(stderr," base64SPS %d base64PPS %d\n",strlen(base64SPS),strlen(base64PPS));
       #endif
        //SDP信息获取
        char *SDP="v=0\r\n"
        		  "s=H.264 Video, streamed by the SimpleRTSP Media Server\r\n"
        		  "a=tool:SimpleRTSP Streaming Media v2016.07.13\r\n"
        		  "a=control:*\r\n"
        		  "a=baseurl:rtsp://%s/live%d.264\r\n"
        		  "a=range:npt=0-\r\n"
        		  "m=video 0 RTP/AVP 96\r\n"
        		  "c=IN IP4 0.0.0.0\r\n"
        		  "a=rtpmap:96 H264/90000\r\n"
        		  "a=fmtp:96 packetization-mode=1;profile-level-id=%d;sprop-parameter-sets=%s,%s\r\n"
        		  "a=control:track1\r\n";
        sprintf(ptr,SDP,
        			localIp,
        			channel,
        			profileId,
        			base64SPS,
        			base64PPS);//组建sdp信息
        return 1;
}

void RtspSession::handleCmd_SETUP(char * urlPreSuffix,char * urlSuffix, char * fullRequestStr)
{
      // 查找SETUP请求头中的"Transport:"头,并提取客户端参数信息
      StreamingMode streamingMode;
      char* streamingModeString = NULL; // set when RAW_UDP streaming is specified
      unsigned short int clientRTPPortNum;
      unsigned char rtpChannelId, rtcpChannelId;
      unsigned channel;
      unsigned int channelIndex;
      char* track = NULL;
      int off = 0;
      int tmp;
      char *sessionIdStr = NULL;
      int resOff = 0;//待写入应答报文偏移
      bool err=false;
      char fmtTmp[2048]={0};
      int pos=0;
     
       while (1){ //循环解析transport头域
          tmp = off;
          //解析"Transport:"头
          parseTransportHeader((char *)requestBuffer, streamingMode, streamingModeString,
                               clientRTPPortNum,
                               rtpChannelId, rtcpChannelId,
                               channel, track, off);
          if (tmp == off){//所有子媒体流"Transport"头解析完,跳出循环
              break;
          }
          mSM=streamingMode; /*获取流模式*/
          unsigned ch=extractChannelNumber(urlPreSuffix,urlSuffix);
          channel=ch;
          if(!findChannelByIndex((int)channel)){ //未找到,报错
               handleCmd_notFound();err=true;
               break;
           }else if(findChannelByIndex((int)channel)){

           	H264MediaManager *chManager=getChannelByIndex((int)channel);

			if(RTP_TCP==streamingMode){

				TCPClientSession *tClientSession=new TCPClientSession();
				fprintf(stderr,"--------------> RTP_TCP <---------------------\n");
				tClientSession->setTCPSocket(fsocket);
				int i=0;
                for(;i<MAX_CLIENT_SESSION_NUM;++i){
                    if(tSession[i]==NULL){
                        tSession[i]=tClientSession;
                        break;
                    }
                }
                if(i==MAX_CLIENT_SESSION_NUM){ //超出通道最大容纳数，报错
                    handleCmd_notFound();
                    tClientSession->setTearDown(TearDown);
                    tSession[i]=NULL;
                    err=true;
                    break;;
                 }   
                 //提取track
                 track=extrackTrack(urlPreSuffix,urlSuffix);
                 bool notSupport=false; //判断是否支持
                 if(track!=NULL && track[0]!='\0'){
                    if(strcmp(track,chManager->getTrack1())!=0 
                    	&& strcmp(track,chManager->getTrack2())!=0){//支持 track1或track=1两种格式
                          notSupport=true;
                    }
                 }else{ //为空值是默认track1
                    static char tTrack[20];
                    sprintf(tTrack,"track1");
                    track=tTrack;
                 }
                 if(notSupport){ //不支持格式
                    handleCmd_notFound();
                    tClientSession->setTearDown(TearDown);
                    tSession[i]=NULL;
                    err=true;
                    break;
                 }
                 unsigned int sessionId = GetRandom32();
    			 char *sessionIdStr = new char[20];
    			 sprintf(sessionIdStr, "%08X", sessionId);
    			 tClientSession->setSessionId(sessionIdStr);
				 delete[] sessionIdStr;
                 chManager->addTcpClientSession(tClientSession);
                 char transport[200];
                   sprintf(transport,"Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n"); //
				   char fmt[200]={0};
           		   snprintf(fmt, sizeof(fmt),
                   "RTSP/1.0 200 OK\r\n"
                   "CSeq: %s\r\n"
                   "%s"
                   "%s"
                   "Session:%s\r\n",
                   fCurrentCSeq, 
                   dateHeader(),
                   transport,
                   tClientSession->getSessionId());
                   
            		snprintf((char*)responseBuffer, sizeof responseBuffer,"%s\r\n", fmt);
                 	fprintf(stderr,"======================== %s \n",fmt);
                 	return;
			}else if(RTP_UDP==streamingMode){
                ClientSession *session=new ClientSession;
                {
                    MyMutex mutex;
                    chManager->addSession(session); //设置完毕后将客户端任务加入到通道类中
                }
                int i=0;
                for(;i<MAX_CLIENT_SESSION_NUM;++i){
                    if(cSession[i]==NULL){
                        cSession[i]=session;
                        break;
                    }
                }
                if(i==MAX_CLIENT_SESSION_NUM){ //超出通道最大容纳数，报错
                    handleCmd_notFound();
                    session->setTearDown(TearDown);
                    cSession[i]=NULL;
                    err=true;
                    break;;
                 }   
                 //提取track
                 track=extrackTrack(urlPreSuffix,urlSuffix);
                 bool notSupport=false; //判断是否支持
                 if(track!=NULL && track[0]!='\0'){
                    if(strcmp(track,chManager->getTrack1())!=0 && strcmp(track,chManager->getTrack2())!=0){//支持 track1或track=1两种格式
                          notSupport=true;
                    }
                 }else{ //为空值是默认track1
                    static char tTrack[20];
                    sprintf(tTrack,"track1");
                    track=tTrack;
                 }
                 if(notSupport){ //不支持格式
                    handleCmd_notFound();
                    session->setTearDown(TearDown);
                    cSession[i]=NULL;
                    err=true;
                    break;
                 }
                    
                    //设置服务器端的一些参数
                 this->setClientSessionID(session); //添加任务ID
                 UInt32 localAddr=fsocket->GetLocalAddr();
                 UDPSocket *rtpSocket=new UDPSocket(NULL,0);
                 UDPSocket *rtcpSocket=new UDPSocket(NULL,0);
                 rtpSocket->Open(); //打开rtp socket
                 rtcpSocket->Open(); //打开rtcp socket
                 //rtpSocket->SetSocketBufSize(32*1024); //设置rtp socket至32K
				 //UInt32 localAddr=ntohl(inet_addr(localIp));
                 UInt16 rtpPort=15500;   //最小端口为15500，最大端口为65535
                 UInt16 rtcpPort;
                 {
                    // MyMutex mutex;
                    //绑定端口                       
                    for(;rtpPort<65535;++rtpPort){
                        if(rtpSocket->Bind((UInt32)ntohl(inet_addr(localIp)), rtpPort)==OS_NoErr){ 
                            rtcpPort=rtpPort+1;
                            if(rtcpSocket->Bind((UInt32)ntohl(inet_addr(localIp)),rtcpPort)==OS_NoErr){
                            session->setRtpSocket(rtpSocket);
                            session->setRtcpSocket(rtcpSocket);
                            break;
                           }
                          }
                       }
                   }
                   //设置远端地址信息
                   UInt32 remoteAddr=fsocket->GetRemoteAddr();
                   session->setAddr(remoteAddr);//设置远端IP地址
                   session->setRtpPort(clientRTPPortNum); //设置远端RTPPort 
                   session->setRtpRemoteAddr();
                   //只支持 RAW_UDP模式(只发送RTP数据，不发送RTCP数据)
                   snprintf(fmtTmp+pos, sizeof fmtTmp-pos,
                   "Transport: RTP/AVP/UDP;unicast;client_port=%d;server_port=%d\r\nSession:%s\r\n",
                   clientRTPPortNum,
                   rtpPort,
                   session->getSessionId());
                   pos=strlen(fmtTmp);
           }
         }
        }

      if(!err){  //解析处理过程中是否出错
            char aggreStr[20]={0};
      
            int curChannel=extractChannelNumber(urlPreSuffix,urlSuffix);//获取通道号

            char fmt[200]={0};
            snprintf(fmt, sizeof(fmt),
                   "RTSP/1.0 200 OK\r\n"
                   "CSeq: %s\r\n"
                   "%s"
                   "%s"
                   "Channel:%d\r\n",
                   fCurrentCSeq, 
                   aggreStr, 
                   dateHeader(),
                   curChannel);
                   
            snprintf((char*)responseBuffer, sizeof responseBuffer,"%s%s\r\n", fmt,fmtTmp);
            fprintf(stderr,"SETUP responseBuffer Cotnent %s\n",responseBuffer);
      }
}

void RtspSession::handleCmd_PLAY(int mychannel,char *sessionID, char * fullRequestStr)
{
    unsigned char channel;
    char* track = NULL;
    char* sessionIdStr = NULL;
    char* rangeStr = NULL;
    float scale=1.0;
    int reqOff = 0, tmp = 0, resOff = 0;
    char fmtTmp[2048]={0};
    int pos=0;
   
    unsigned short rtpSeqNum = 0;
    unsigned rtpTimestamp = 0;
    rtpSeqNum=GetRandom16(); //随机值
    rtpTimestamp=GetRandom32(); //随机值
    do{
    	if(RTP_TCP==mSM){
    		TCPClientSession *session1=NULL;
			for(int i=0;i<MAX_CONNECT;++i){
        		if(tSession[i]!=NULL){
            		TCPClientSession *session=tSession[i];
            		if(strcmp(session->getSessionId(),sessionID)==0){
                	session1=session;
            		}	
        		}
    		}
    		if(session1==NULL){
				handleCmd_sessionNotFound();
             	break;
    		}

			//组件响应头
         char sessionIDStr[32]={0};
         sprintf(sessionIDStr,"Session:%s\r\n",
         						sessionID); //任务ID
         					
         snprintf((char *)responseBuffer, sizeof(responseBuffer),
                    "RTSP/1.0 200 OK\r\n"
                    "CSeq: %s\r\n"
                    "Range: npt=0.000-\r\n"
                    "%s"
                    "%s\r\n",
                   fCurrentCSeq,sessionIDStr, dateHeader()); 
            //激活任务
         session1->setActive(active);
    		
    		
    	}else if(RTP_UDP==mSM){
         ClientSession *session=findSession(sessionID); //根据任务ID查找任务对象
         if(session==NULL){
             handleCmd_sessionNotFound();
             break;
         }
         //组件响应头
         char sessionIDStr[32]={0};
         sprintf(sessionIDStr,"Session:%s\r\n",
         						sessionID); //任务ID
         char RtpInfo[256]={0};
         
         sprintf(RtpInfo,"RTP-Info:url=rtsp://%s/live%d.264/track1;seq=%d;rtptime=%d\r\n",
         					fsocket->GetRemoteAddrStr()->GetAsCString(),
         					mychannel,
         					rtpSeqNum,
         					rtpTimestamp);
         					
         snprintf((char *)responseBuffer, sizeof(responseBuffer),
                    "RTSP/1.0 200 OK\r\n"
                    "CSeq: %s\r\n"
                    "Range: npt=0.000-\r\n"
                    "%s"
                    "%s"
                    "%s\r\n",
                   fCurrentCSeq,sessionIDStr, dateHeader(),RtpInfo); 
            //激活任务
         session->setActive(active);
         }
    }while(0);
}

void RtspSession::handleCmd_TEARDOWN(char *sessionID)
{
    
    if(sessionID[0]!='\0'){ 
    	bool notFindSession=true;
    	if(mSM==RTP_UDP){

		ClientSession *tClientSession=NULL;

    	
        for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
            tClientSession=cSession[i];
            if(tClientSession!=NULL){
                if(strcmp(tClientSession->getSessionId(),sessionID)==0){
                    tClientSession->setTearDown(TearDown);
                    cSession[i]=NULL; //任务销毁，清空指针
                    notFindSession=false;
                    break;
                }
            }
        }
			 if(!notFindSession && tClientSession!=NULL){
            snprintf((char *)responseBuffer,RESPONSE_BUFFER_SIZE,"RTSP/1.0 200 OK\r\n"
            													 "CSeq: %s\r\n"
            													 "%sSession:%s\r\n\r\n",
            													 fCurrentCSeq,
            													 dateHeader(),
            													 tClientSession->getSessionId());
        }else if(mSM==RTP_TCP){

		 TCPClientSession *session1=NULL;
		 for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
            session1=tSession[i];
            if(session1!=NULL){
                if(strcmp(session1->getSessionId(),sessionID)==0){
                    session1->setTearDown(TearDown);
                    tSession[i]=NULL; //任务销毁，清空指针
                    notFindSession=false;
                    break;
                }
            }
        }
        	if(!notFindSession && session1!=NULL){
            snprintf((char *)responseBuffer,RESPONSE_BUFFER_SIZE,
            			"RTSP/1.0 200 OK\r\n"
            			"CSeq: %s\r\n"
            			"%sSession:%s\r\n\r\n",
            			fCurrentCSeq,
            			dateHeader(),
            			session1->getSessionId());
        }
       
        }else{  //没有找到任务
            handleCmd_sessionNotFound();
        }
    }else{
      handleCmd_sessionNotFound();
    }
	}
}


void RtspSession::handleCmd_PAUSE(char *sessionID)
{
    ClientSession *tClientSession=NULL;
    if(sessionID[0]!='\0'){ //非聚合操作,根据sessionID 查找任务
        bool notFindSession=true;
        for(int i=0;i<MAX_CLIENT_SESSION_NUM;++i){
            tClientSession=cSession[i];
            if(tClientSession==NULL){
               continue;
            }
            if(strcmp(tClientSession->getSessionId(),sessionID)==0){
                tClientSession->setActive(notActive);
                notFindSession=false;
                break;
            }
        }
        if(!notFindSession && tClientSession!=NULL){
            snprintf((char *)responseBuffer,RESPONSE_BUFFER_SIZE,
            			"RTSP/1.0 200 OK\r\n"
            			"CSeq: %s\r\n"
            			"%s"
            			"Session:%s\r\n\r\n",
            			fCurrentCSeq,
            			dateHeader(),
            			tClientSession->getSessionId());
            			
        }else{  //没有找到任务
            handleCmd_sessionNotFound();
        }
    }else{
      handleCmd_sessionNotFound();
    }
}

void RtspSession::handleCmd_YTCTRL(int channel,int nType,char *sessionId)
{
  static OSMutex mutex;
  if(channel<0 || channel>MAX_CHANNEL_NUM){
    handleCmd_notFound();
    return;
  }
  sprintf((char*)responseBuffer,
          "RTSP/1.0 200 OK\r\n"
          "CSeq:%s\r\n"
          "nType:%d\r\n"
          "Session:%s\r\n\r\n",
          fCurrentCSeq,
          nType,
          sessionId);
}

void RtspSession::handleCmd_GETDVS(int channel, char *cmdName,char *cseq,char *sessionId)
{
  int nType=0;
  char pBuffer[2*1024]={0}; //设备信息
  int nSize=1024;
  if(channel<0 || channel>MAX_CHANNEL_NUM ){
    handleCmd_notFound();
    return;
  }
  if(pBuffer[0]=='\0'){
        handleCmd_bad();
        return;
  }
  //获得内容长度
  int len=strlen(pBuffer);
  //组装响应包
  sprintf((char*)responseBuffer,
          "RTSP/1.0 200 OK\r\n"
          "CSeq:%s\r\n"
          "%s"
          "Content-Length:%d\r\n\r\n"
          "%s\r\n",
          fCurrentCSeq,
          dateHeader(),
          len,
          pBuffer);
}

void RtspSession::handleCmd_SETDVS(int channel,char *sessionID,int nType)
{
  static OSMutex mutex;
  if(channel<0 || channel>MAX_CHANNEL_NUM ){
    handleCmd_notFound();
    return;
  }
  sprintf((char*)responseBuffer,
          "RTSP/1.0 200 OK\r\n"
          "CSeq:%s\r\n\r\n",
          fCurrentCSeq);
  
}
void RtspSession::handleCmd_SETDVS_PlayBack(int channel,char *sessionID,int nType,char *playDate)
{
  if(channel<0 || channel>MAX_CHANNEL_NUM){
    handleCmd_notFound();
    return;
  }
  sprintf((char*)responseBuffer,
          "RTSP/1.0 200 OK\r\n"
          "CSeq:%s\r\n\r\n",
          fCurrentCSeq); 
}

char const*  RtspSession::dateHeader(void)
{
    static char buf[200];

#if !defined(_WIN32_WCE)
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
#else
    // WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
    // so generate the "Date:" header a different, WinCE-specific way.
    // (Thanks to Pierre l'Hussiez for this code)
    // RSF: But where is the "Date: " string?  This code doesn't look quite right...
    SYSTEMTIME SystemTime;
    GetSystemTime(&SystemTime);
    WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
    WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
    WCHAR inBuf[200];
    DWORD locale = LOCALE_NEUTRAL;

    int ret = GetDateFormat(locale, 0, &SystemTime,
    (LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof inBuf);
    inBuf[ret - 1] = ' ';
    ret = GetTimeFormat(locale, 0, &SystemTime,
    (LPTSTR)timeFormat,
    (LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
    wcstombs(buf, inBuf, wcslen(inBuf));
#endif
    return buf;
}

void RtspSession::setType(int type)
{
    sType=type;
}

void RtspSession::getAddr(struct sockaddr_in *addr)
{
    if(Socket>0){
            socklen_t namelen = sizeof addr;
            getsockname(Socket, (struct sockaddr*)addr, &namelen);
    }
}

bool RtspSession::parseRTSPRequestString(char const* reqStr,
                   unsigned reqStrSize,
			       char* resultCmdName,
			       unsigned resultCmdNameMaxSize,
			       char* resultURLPreSuffix,
			       unsigned resultURLPreSuffixMaxSize,
			       char* resultURLSuffix,
			       unsigned resultURLSuffixMaxSize,
			       char* resultCSeq,
			       unsigned resultCSeqMaxSize,
                   char* resultSessionIdStr,
                   unsigned resultSessionIdStrMaxSize,
			       unsigned& contentLength,
			       int &type, 
			       char *playDate,
			       int &setdvsChannel)
{
  //跳过请求指令开始的空白字符
  unsigned i;
  for (i = 0; i < reqStrSize; ++i){
     char c = reqStr[i];
     if (!(	c == ' ' 
     		|| c == '\t' 
     		|| c == '\r' 
     		|| c == '\n' 
     		|| c == '\0')){
          break;
     }
  }
  if (i == reqStrSize){
	return false; //请求指令全为空白符
  }

  //读取到下一个空白符之前的值，作为指令名
  bool parseSucceeded = false;
  unsigned i1 = 0;
  for (; i1 < resultCmdNameMaxSize-1 && i < reqStrSize; ++i,++i1){
    char c = reqStr[i];
    if (c == ' ' || c == '\t'){
      parseSucceeded = true;
      break;
    }
    //复制指令名
    resultCmdName[i1] = c;
  }
  resultCmdName[i1] = '\0';
  //解析指令名失败
  if (!parseSucceeded){
    return false;
  }
 // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
  unsigned j = i+1;
  while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j; // skip over any additional white space
  for (; (int)j < (int)(reqStrSize-8); ++j) {
    if ((reqStr[j] == 'r' || reqStr[j] == 'R')
	&& (reqStr[j+1] == 't' || reqStr[j+1] == 'T')
	&& (reqStr[j+2] == 's' || reqStr[j+2] == 'S')
	&& (reqStr[j+3] == 'p' || reqStr[j+3] == 'P')
	&& reqStr[j+4] == ':' && reqStr[j+5] == '/'){
      j += 6;
    if (reqStr[j] == '/') {
	// This is a "rtsp://" URL; skip over the host:port part that follows:
	    ++j;
	while (j < reqStrSize && reqStr[j] != '/' && reqStr[j] != ' ') ++j;
      } else {
	// This is a "rtsp:/" URL; back up to the "/":
	    --j;
      }
      i = j;
      break;
    }
  }

  // Look for the URL suffix (before the following "RTSP/"):
  parseSucceeded = false;
  for (unsigned k = i+1; (int)k < (int)(reqStrSize-5); ++k) {
    if (reqStr[k] == 'R' && reqStr[k+1] == 'T' &&
	reqStr[k+2] == 'S' && reqStr[k+3] == 'P' && reqStr[k+4] == '/') {
      while (--k >= i && reqStr[k] == ' ') {} // go back over all spaces before "RTSP/"
      unsigned k1 = k;
      while (k1 > i && reqStr[k1] != '/') --k1;

      // ASSERT: At this point
      //   i: first space or slash after "host" or "host:port"
      //   k: last non-space before "RTSP/"
      //   k1: last slash in the range [i,k]

      // The URL suffix comes from [k1+1,k]
      // Copy "resultURLSuffix":
      unsigned n = 0, k2 = k1+1;
      if (k2 <= k) {
        if (k - k1 + 1 > resultURLSuffixMaxSize) return false; // there's no room
        while (k2 <= k) resultURLSuffix[n++] = reqStr[k2++];
      }
      resultURLSuffix[n] = '\0';

      // The URL 'pre-suffix' comes from [i+1,k1-1]
      // Copy "resultURLPreSuffix":
      n = 0; k2 = i+1;
      if (k2+1 <= k1) {
        if (k1 - i > resultURLPreSuffixMaxSize) return false; // there's no room
        while (k2 <= k1-1) resultURLPreSuffix[n++] = reqStr[k2++];
      }
      resultURLPreSuffix[n] = '\0';
      decodeURL(resultURLPreSuffix);

      i = k + 7; // to go past " RTSP/"
      parseSucceeded = true;
      break;
    }
  }

  if (!parseSucceeded){
	return false;
  }

  // 查找 "CSeq:"头域
  parseSucceeded = false;
  for (j = i; (int)j < (int)(reqStrSize-5); ++j){
    if (strncasecmp("CSeq:", &reqStr[j], 5) == 0){
      j += 5;
      //跳过空白字符
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      unsigned n;
      for (n = 0; n < resultCSeqMaxSize-1 && j < reqStrSize; ++n,++j){
	        char c = reqStr[j];
	        if (c == '\r' || c == '\n'){
	            parseSucceeded = true;
	            break;
	        }
	        resultCSeq[n] = c;
      }
      resultCSeq[n] = '\0';
      break;
    }
  }
  if (!parseSucceeded){
	return false;
  }

  //查找"Session:"头域
  resultSessionIdStr[0] = '\0'; //默认值,空串
  for (j = i; (int)j < (int)(reqStrSize-8); ++j){
    if (strncasecmp("Session:", &reqStr[j], 8) == 0){
      j += 8;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      unsigned n;
      for (n = 0; n < resultSessionIdStrMaxSize-1 && j < reqStrSize; ++n,++j){
	        char c = reqStr[j];
	        if (c == '\r' || c == '\n'){
	        break;
	  }
	resultSessionIdStr[n] = c;
   }
   resultSessionIdStr[n] = '\0';
   break;
  }
 }

  // 查找"Content-Length:"头域
  contentLength = 0; //默认值
  for (j = i; (int)j < (int)(reqStrSize-15); ++j){
    if (strncasecmp("Content-Length:", &(reqStr[j]), 15) == 0){
      j += 15;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      unsigned num;
      if (sscanf(&reqStr[j], "%u", &num) == 1){
	    contentLength = num;
      }
    }
  }

  // 查找"Aggregation:"头域
  for (j = i; (int)j < (int)(reqStrSize-12); ++j){
    if (strncasecmp("Aggregation:", &(reqStr[j]), 12) == 0){
      j += 12;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      unsigned num;
      if (sscanf(&reqStr[j], "%u", &num) == 1){
	        if (num == 1){
	        }
      }
    }
  }

  //查找 “nType:”头域
  for (j = i; (int)j < (int)(reqStrSize-6); ++j){
    if (strncasecmp("nType:", &(reqStr[j]), 6) == 0){
      j += 6;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      int num;
      if (sscanf(&reqStr[j], "%u", &num) == 1){
	    type=num;
      }
    }
  }
  
  //查找 “YTcrtl:”头域
  for (j = i; (int)j < (int)(reqStrSize-7); ++j){
    if (strncasecmp("YTcrtl:", &(reqStr[j]), 7) == 0){
      j += 7;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      int myType;
      int myChannel;
      if (sscanf(&reqStr[j], "%d,%d",&myChannel,&myType) == 2){
        setdvsChannel=myChannel;
        type=myType;
      }
    }
  }
    //查找 “Playback:”头域
  for (j = i; (int)j < (int)(reqStrSize-9); ++j){
    if (strncasecmp("Playback:", &(reqStr[j]), 9) == 0){
      j += 9;
      while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t')) ++j;
      int myType;
      int myChannel;
      char tDateTime[128];
      
      if (sscanf(&reqStr[j], "%u,%u,%s",&myChannel,&myType,tDateTime) == 3){
        setdvsChannel=myChannel;
        type=myType;
        sprintf(playDate,"%s",tDateTime);
      }
    }
  }
  return true;
}

void RtspSession::decodeURL(char* url)
{
  //将URL中%<hex><hex>后的2位16进制转换为10进制数
  char* cursor = url;
  while (*cursor){
    if ((cursor[0] == '%') && \
	cursor[1] && isxdigit(cursor[1]) && \
	cursor[2] && isxdigit(cursor[2])){
      char hex[3];
      hex[0] = cursor[1];
      hex[1] = cursor[2];
      hex[2] = '\0';
      *url++ = (char)strtol(hex, NULL, 16);
      cursor += 3;
    }
    else{
      *url++ = *cursor++;
    }
  }
  *url = '\0';
}

void RtspSession::parseTransportHeader(char * buf,
				 StreamingMode& streamingMode,
				 char*& streamingModeString,
				 unsigned short int& clientRTPPortNum, // if UDP
				 unsigned char& rtpChannelId, // if TCP
				 unsigned char& rtcpChannelId, // if TCP
				 unsigned & channel,  //聚合操作用于标识通道号
				 char*& track,//聚合操作中用于标识子媒体流track
				 int& off	//指示当前解析到的位置(相对请求字符串开始)
				 )
{
  //初始化为默认值
  streamingMode = RTP_UDP;
  static char sModeString[30]={0};
  streamingModeString = sModeString;
  clientRTPPortNum = 0;
  rtpChannelId = rtcpChannelId = 0xFF;
  channel = 0;

  unsigned short int p1, p2;
  unsigned ttl, rtpCid, rtcpCid, chn;

  //设置并记录查找Transport头的起始位置
  buf += off;
  const char* start = buf;
  // 查找 "Transport:"
  while (1){
      if (*buf == '\0'){
          return; // not found
       }
      if (*buf == '\r' && *(buf+1) == '\n' && *(buf+2) == '\r'){
          return; // 到达结尾 => not found
       }
      if (strncasecmp(buf, "Transport:", 10) == 0){
          break;
       }
      ++buf;
  }

  track = new char[10];
  memset(track, 0, 10);
  // 查找并处理"Transport:"头中每一个域
  char const* fields = buf + 10;
  while (*fields == ' ') ++fields;
  char* field = myStrDupSize(fields);
  while (sscanf(fields, "%[^;\r\n]", field) == 1){
      if (strcmp(field, "RTP/AVP/TCP") == 0){
          streamingMode = RTP_TCP;
        }
      else if (strcmp(field, "RAW/RAW/UDP") == 0 ||
               strcmp(field, "MP2T/H2221/UDP") == 0){
          streamingMode = RAW_UDP;
          memcpy(streamingModeString,field,strlen(field)+1);
        }
      else if (sscanf(field, "client_port=%hu-%hu", &p1, &p2) == 2){
          clientRTPPortNum = p1;
        }
      else if (sscanf(field, "client_port=%hu", &p1) == 1){
          clientRTPPortNum = p1;
        }
      else if (sscanf(field, "interleaved=%u-%u", &rtpCid, &rtcpCid) == 2){
          rtpChannelId = (unsigned char)rtpCid;
          rtcpChannelId = (unsigned char)rtcpCid;
        }
      else if (sscanf(field, "channel=%u", &chn) == 1){
          channel = chn;
        }
      else if (sscanf(field, "track=%s", track) == 1){
        }

      fields += strlen(field);
      while (*fields == ';' || *fields == ' ' || *fields == '\t'){
          ++fields; //跳过';'分隔符或空白符
      }
      if (*fields == '\0' || *fields == '\r' || *fields == '\n'){
          break;
      }
    }
  //设置下次查找Transport头的起始偏移
  off += fields - start;
  delete[] field;

}


unsigned short int RtspSession::GetRandom16()
{
    uint32_t x =  ((GetRandom32() >> 16)&0xffff);
	return (unsigned short int)x;
}

unsigned int RtspSession::GetRandom32()
{
	state = ((0x5DEECE66DULL*state) + 0xBULL)&0x0000ffffffffffffULL;
	uint32_t x = (uint32_t)((state>>16)&0xffffffffULL);
	return x;
}

unsigned int RtspSession::PickSeed()
{
	uint32_t x;
	x = (uint32_t)getpid();
	x += (uint32_t)time(0);
	x += (uint32_t)clock();
	x ^= (uint32_t)((uint8_t *)this - (uint8_t *)0);
	return x;
}

void RtspSession::SetSeed(unsigned int seed)
{
	state = ((uint64_t)seed) << 16 | 0x330EULL;
}

void RtspSession::setClientSessionID(ClientSession *Session)
{
    unsigned int sessionId = GetRandom32();
    char *sessionIdStr = new char[20];
    sprintf(sessionIdStr, "%08X", sessionId);
    Session->setSessionId(sessionIdStr);
    delete[] sessionIdStr;
}

unsigned RtspSession::lookChannelNum(char *channelName)
{
    char *ptr=channelName;
    ptr+=4; 
    unsigned channel;
    sscanf((char const *)ptr,"%02d",&channel);
    return channel;
}
ClientSession *RtspSession::findSession(char *sessionID)
{

    for(int i=0;i<MAX_CONNECT;++i){
        if(cSession[i]!=NULL){
            ClientSession *session=cSession[i];
            if(strcmp(session->getSessionId(),sessionID)==0){
                return session;
            }
        }
    }
    return NULL;
}

void RtspSession::handleCmd_sessionNotFound()
{
  setRTSPResponse("454 Session Not Found");
}

void RtspSession::handleCmd_notFound()
{
  setRTSPResponse("404 Channel Not Found");
  timeInterval=-1;
}

void RtspSession::handleCmd_notSupported()
{
  snprintf((char*)responseBuffer, sizeof responseBuffer,
           "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
           fCurrentCSeq, dateHeader(),allowedCommandNames());
}

void RtspSession::handleCmd_bad()
{
  snprintf((char*)responseBuffer, sizeof responseBuffer,
	   "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
	   dateHeader(), allowedCommandNames());
}

void RtspSession::setRTSPResponse(char const* responseStr)
{
  snprintf((char*)responseBuffer, sizeof responseBuffer,
	   "RTSP/1.0 %s\r\n"
	   "CSeq: %s\r\n"
	   "%s\r\n",
	   responseStr,
	   fCurrentCSeq,
	   dateHeader());
}

char const* RtspSession::allowedCommandNames()
{
  return "OPTIONS,DESCRIBE,SETUP,TEARDOWN,PLAY,PAUSE,GET_PARAMETER,SET_PARAMETER";
}
unsigned RtspSession::extractChannelNumber(char * urlPreSuffix,char * urlSuffix)
{
    char fix[20]={0};
     //获取通道名 live01.264-live32.264
    if(urlPreSuffix[0]!='\0'){
        memcpy(fix,urlPreSuffix,strlen(urlPreSuffix));
    }else{
        memcpy(fix,urlSuffix,strlen(urlSuffix));
    }
    if(fix[0]=='\0'){
        return -1;
    }
    return lookChannelNum(fix);
}

char *RtspSession::extrackTrack(char * urlPreSuffix,char * urlSuffix)
{
    static char track[20]={0};
    if(urlPreSuffix[0]!='\0'){
        memcpy(track,urlSuffix,strlen(urlSuffix));
    }else{
        memcpy(track,urlPreSuffix,strlen(urlPreSuffix));
    }
    track[strlen(track)+1]='\0';
    return track;
}
void RtspSession::testScaleFactor(float& scale)
{
  // 默认仅支持 scale = 1
  scale = 1;
}

void RtspSession::setLocalInfo(char *ip,char *name,char *dId)
{
 	memset(localIp,'\0',sizeof(localIp));
 	memset(localName,'\0',sizeof(localName));
 	memset(deviceId,'\0',sizeof(deviceId));

    memcpy(localIp,ip,strlen(ip));
    memcpy(localName,name,strlen(name));
    memcpy(deviceId,dId,strlen(dId));
}
bool RtspSession::parseScaleHeader(char const* buf, float& scale)
{
  scale = 1.0; //默认值
  //"Scale:"
  while (1)
  {
    if (*buf == '\0'){
	return false; // not found
    }
    if (strncasecmp(buf, "Scale:", 6) == 0){
	break;
    }
    ++buf;
  }
  char const* fields = buf + 6;
  while (*fields == ' ') ++fields;
  float sc;
  if (sscanf(fields, "%f", &sc) == 1){
    scale = sc;
  }
  else{
    return false; //格式有误
  }
  return true;
}
bool RtspSession::parseRangeHeader(char const* buf,double& rangeStart, double& rangeEnd,
			 char*& absStartTime, char*& absEndTime,
			 bool& startTimeIsNow)
{
  // find "Range:"
  while (1){
    if (*buf == '\0'){
	return false; // not found
    }
    if (strncasecmp(buf, "Range: ", 7) == 0){
	break;
    }
    ++buf;
  }
  char const* fields = buf + 7;
  while (*fields == ' ') ++fields;
  return parseRangeParam(fields, rangeStart, rangeEnd, absStartTime, absEndTime, startTimeIsNow);
}
bool RtspSession::parseRangeParam(char const* paramStr, double& rangeStart, double& rangeEnd,
			char*& absStartTime, char*& absEndTime, bool& startTimeIsNow)
{
  delete[] absStartTime;
  delete[] absEndTime;
  absStartTime = absEndTime = NULL; //默认值
  startTimeIsNow = false; // by default
  double start, end;
  int numCharsMatched1 = 0, numCharsMatched2 = 0, numCharsMatched3 = 0, numCharsMatched4 = 0;

  if (sscanf(paramStr, "npt = %lf - %lf", &start, &end) == 2){
    rangeStart = start;
    rangeEnd = end;
  }
  else if (sscanf(paramStr, "npt = %n%lf -", &numCharsMatched1, &start) == 1)
  {
    if (paramStr[numCharsMatched1] == '-'){
      // "npt = -<endtime>"
      rangeStart = 0.0;
      startTimeIsNow = true;
      rangeEnd = -start;
    }
    else
    {
      rangeStart = start;
      rangeEnd = 0.0;
    }
  }
  else if (sscanf(paramStr, "npt = now - %lf", &end) == 1)
  {
      	rangeStart = 0.0;
	    startTimeIsNow = true;
      	rangeEnd = end;
  }
  else if (sscanf(paramStr, "npt = now -%n", &numCharsMatched2) == 0 && numCharsMatched2 > 0)
  {
    	rangeStart = 0.0;
	    startTimeIsNow = true;
    	rangeEnd = 0.0;
  }
  else if (sscanf(paramStr, "clock = %n", &numCharsMatched3) == 0 && numCharsMatched3 > 0)
  {
    rangeStart = rangeEnd = 0.0;

    char const* utcTimes = &paramStr[numCharsMatched3];
    size_t len = strlen(utcTimes) + 1;
    char* as = new char[len];
    char* ae = new char[len];
    int sscanfResult = sscanf(utcTimes, "%[^-]-%s", as, ae);
    if (sscanfResult == 2){
      absStartTime = as;
      absEndTime = ae;
    }
    else if (sscanfResult == 1){
      absStartTime = as;
      delete[] ae;
    }
    else{
      delete[] as;
      delete[] ae;
      return false;
    }
  }
  else if (sscanf(paramStr, "smtpe = %n", &numCharsMatched4) == 0 && numCharsMatched4 > 0)
  {
    // We accept "smtpe=" parameters, but currently do not interpret them.
  }
  else
  {
    return false; // The header is malformed
  }
  return true;
}
void RtspSession::parseChannelInfoHeader(char const* buf,
				 unsigned char& channel,   //聚合操作用于标识通道号
				 char*& track,	//聚合操作中用于标识子媒体流track
				 char*& sessionIdStr,	//子媒体流所在SessionID
				 float& scale,		//子媒体流Scale
				 char*& rangeStr,	//子媒体流Range
				 int& off	//指示当前解析到的位置(相对请求字符串开始)
				 )
{
  channel = 0;
  scale = 0;

  //设置并记录查找ChannelInfo头的起始位置
  buf += off;
  const char* start = buf;
  // 查找 "ChannelInfo:"
  while (1)
  {
    if (*buf == '\0'){
	return; // not found
    }
    if (*buf == '\r' && *(buf+1) == '\n' && *(buf+2) == '\r'){
	    return; // 到达结尾 => not found
    }
    if (strncasecmp(buf, "ChannelInfo:", 12) == 0){
	    break;
    }
    ++buf;
  }

  //track
  if (track == NULL){
  	track = new char[10];
  }
  memset(track, 0, 10);

  //session
  if (sessionIdStr == NULL){
  	sessionIdStr = new char[50];
  }
  memset(sessionIdStr, 0, 50);
  //range
  if (rangeStr ==  NULL){
  	rangeStr = new char[50];
  }
  memset(rangeStr, 0, 50);
  unsigned chn;
  // 查找并处理"ChannelInfo:"头中每一个域
  char const* fields = buf + 12;
  while (*fields == ' ') ++fields;
  char* field = myStrDupSize(fields);
  while (sscanf(fields, "%[^;\r\n]", field) == 1){
    if (sscanf(field, "channel=%u", &chn) == 1){
	        channel = chn;
    }
    else if (sscanf(field, "track=%s", track) == 1){
    }
    else if (sscanf(field, "session=%s", sessionIdStr) == 1){
    }
    else if (sscanf(field, "scale=%f", &scale) == 1){
    }
    else if (sscanf(field, "range=%s", rangeStr) == 1){
    }
    fields += strlen(field);
    while (*fields == ';' || *fields == ' ' || *fields == '\t'){
	++fields; //跳过';'分隔符或空白符
    }
    if (*fields == '\0' || *fields == '\r' || *fields == '\n'){
	break;
    }
  }
  //设置下次查找Transport头的起始偏移
  off += fields - start;
  delete[] field;
}
uint32_t RtspSession::getProfileId(uint8_t* from, unsigned int size)
{
  uint32_t fProfileLevelId=0;
  if (from == NULL){
      return 0;
    }
  unsigned char *fLastSeenSPS = new uint8_t[size];
  memcpy(fLastSeenSPS, from, size);

  unsigned int fLastSeenSPSSize = size;

  // Extract the first 3 bytes of the SPS (after the nal_unit_header byte) as 'profile_level_id'
  if (fLastSeenSPSSize >= 1 + 3){
      fProfileLevelId = (fLastSeenSPS[1]<<16) | (fLastSeenSPS[2]<<8) | fLastSeenSPS[3];
    }
  delete fLastSeenSPS;
  return fProfileLevelId;
}
void RtspSession::subLiveNum()
{
    if(RtspSessionLiveNum>0)
        RtspSessionLiveNum--;    
}
void RtspSession::addLiveNum()
{
    RtspSessionLiveNum++;
}
int RtspSession::getLiveNum()
{
    return RtspSessionLiveNum;
}

