#ifndef _RTSPSESSION_H
#define _RTSPSESSION_H

#include "Task.h"
#include "TCPSocket.h"
#include "RTSPClientSession.h"
#include <stdint.h>
#include "OSMutex.h"
#include "RTSPInterface.h"
#include "TCPClientSession.h"
class H264MediaManager;
class ClientSession;
class TcpTimeoutTask;

#define RESPONSE_BUFFER_SIZE 15*1024 //响应buffer大小
#define REQUEST_BUFFER_SIZE 15*1024  //请求buffer大小
#define RTSP_PARAM_STRING_MAX 512     //解析字符串
#define MAX_CLIENT_SESSION_NUM 256     //每个rtsp连接最大允许的任务数
enum    
{
    reg=0,  //RTSPSession为注册任务(响应代理服务器)
    gen=1   //RTSPSession 为普通任务(响应客户端)
};

typedef enum StreamingMode
{
  RTP_UDP,
  RTP_TCP,
  RAW_UDP
} StreamingMode;

class RtspSession:public Task
{
    public:
        RtspSession();
        virtual ~RtspSession();

        
        /*任务执行体*/
        virtual SInt64  Run(void);
		
		/* 杀死自身 */
		void killSelf();
		
        //初始化RtspSession类
        void initRtspSession(void);
        
        /*获得TCPSocket类*/
        TCPSocket * GetSocket(void){return fsocket;}
        void SetSocket(TCPSocket *tSocket){fsocket=tSocket;}

        /*添加任务到任务线程(该函数用于连接服务器的rtspSession，监听rtspSession不会调用该函数)*/
        
        /*任务类型(用于区分客户端与代理服务器)*/
        void setType(int type);
        void testScaleFactor(float& scale);
        void setLocalInfo(char *ip,char *name,char *dId);
        void setServerInfo(char *ip,int port);
        int Socket;       //socket文件描述符

        static void subLiveNum();
        static void addLiveNum();
        static int  getLiveNum();



   private: //RTSP命令相关

        void handleCmd_OPTIONS();
        void handleCmd_GET_PARAMETER();
        void handleCmd_DESCRIBE(char * urlPreSuffix,char* urlSuffix, char* fullRequestStr);
        void handleCmd_SETUP(char * urlPreSuffix,char * urlSuffix, char * fullRequestStr);
        void handleCmd_PLAY(int mychannel,char *sessionID, char * fullRequestStr);
        void handleCmd_TEARDOWN(char *sessionID);
        void handleCmd_PAUSE(char *sessionID);
        void handleCmd_YTCTRL(int channel,int nType,char *sessionId);
        void handleCmd_GETDVS(int channel, char *cmdName,char *cseq,char *sessionId);
        void handleCmd_SETDVS(int channel,char *sessionID,int nType);
        void handleCmd_SETDVS_PlayBack(int channel,char *sessionID,int nType,char *playDate);

        

        /*解析请求中通用头域(url,cseq,sessionId,contentlenght,aggregation等)*/
        bool parseRTSPRequestString(char const* reqStr,
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
			       int &setdvsChannel);
        
        /*解析Transport头域*/
        void parseTransportHeader(char * buf,
				 StreamingMode& streamingMode,
				 char*& streamingModeString,
				 unsigned short int& clientRTPPortNum, // if UDP
				 unsigned char& rtpChannelId, // if TCP
				 unsigned char& rtcpChannelId, // if TCP
				 unsigned & channel,  //聚合操作用于标识通道号
				 char*& track,//聚合操作中用于标识子媒体流track
				 int& off	//指示当前解析到的位置(相对请求字符串开始)
				 );
        
        //解析Range头域
        bool parseRangeHeader(char const* buf,
			 double& rangeStart, double& rangeEnd,
			 char*& absStartTime, char*& absEndTime,
			 bool& startTimeIsNow);
        bool parseRangeParam(char const* paramStr, double& rangeStart, double& rangeEnd,
			char*& absStartTime, char*& absEndTime, bool& startTimeIsNow);

        //解析ChannelInfo头域
        void parseChannelInfoHeader(char const* buf,
				 unsigned char& channel,   //聚合操作用于标识通道号
				 char*& track,	//聚合操作中用于标识子媒体流track
				 char*& sessionIdStr,	//子媒体流所在SessionID
				 float& scale,		//子媒体流Scale
				 char*& rangeStr,	//子媒体流Range
				 int& off	//指示当前解析到的位置(相对请求字符串开始)
				 );

        //解析Scale头域
        bool parseScaleHeader(char const* buf, float& scale);

        //错误的请求
        void handleCmd_bad();
        
        //未找到流
        void handleCmd_notFound();
        
        //命令不支持
        void handleCmd_notSupported();

        //任务为找到
        void handleCmd_sessionNotFound();
        
        //支持的命令
        char const* allowedCommandNames();
               
        //任务出错是的辅助函数
        void setRTSPResponse(char const* responseStr);
        
        //提取通道号
        unsigned extractChannelNumber(char * urlPreSuffix,char * urlSuffix);
        
        //提取track
        char *extrackTrack(char * urlPreSuffix,char * urlSuffix);
        
        //获取当前socket地址结构
        void getAddr(struct sockaddr_in *addr);

        //重置请求缓冲区
        void resetRequestBuffer();
    
        /*处理RTSP请求*/
        bool handleRequest(int rOutLen);

        /*组建时间头*/
        char const* dateHeader(void);
        
        /*解析URL*/
        void decodeURL(char* url);
        
        //获得端随机数
        unsigned short int GetRandom16();
        
        /*获取随机数(用于生成SessionID)*/
        unsigned int GetRandom32();

        /*获取随机种子*/
        unsigned int PickSeed();

        /*设置随机种子*/
        void SetSeed(unsigned int seed);

        //为ClientSession对象添加任务ID
        void setClientSessionID(ClientSession *Session);

        //从通道名中提取通道号
        unsigned lookChannelNum(char *channelName);

        //根据任务ID查找任务对象
        ClientSession *findSession(char *sessionID);

        //获得profile_id
        uint32_t getProfileId(uint8_t* from, unsigned int size);

        //获得SDP信息
        int getSDP(H264MediaManager *manager,int channel,char *sdp);

        //索引查找通道
		bool findChannelByIndex(int index);

        //索引获得通道
		H264MediaManager *getChannelByIndex(int index);
	private:
        int fRequestBufferBytesLeft;    //已使用空间大小
        int fRequestBytesAlreadySeen;   //剩余空间大小
        unsigned char* fLastCRLF;       //记录RTSP请求头中结尾的倒数第二个"\r\n"
        unsigned char requestBuffer[REQUEST_BUFFER_SIZE]; //请求缓冲区
        
        unsigned char responseBuffer[RESPONSE_BUFFER_SIZE]; //响应缓冲区
        char const* fCurrentCSeq;        //记录客户端RTSP请求指令中的CSeq值
        SInt64 timeInterval;    //任务调用时间间隔

        TCPSocket * fsocket;  //socket类

        H264MediaManager **cManager; //通道管理类列表
        ClientSession *cSession[MAX_CLIENT_SESSION_NUM];    //任务管理列表(本客户端连接)
        TCPClientSession *tSession[MAX_CLIENT_SESSION_NUM];    //任务管理列表(本客户端连接)

        int survivalClientSessionNum;
        
        int sType;  //任务类型(响应代理服务器、普通客户端)
        unsigned long long int state; // 随机函数中间状态
        TcpTimeoutTask *watchTask;
        int connectNumbers;

        char localIp[32];
        char localName[32];
        char deviceId[64];
        char serverIp[32];
        int serverPort;
        static int RtspSessionLiveNum; /*因是多网卡所以可能会有多个RTSPSession任务,故需要记录当前仍存活的RTSPSession数量*/


		StreamingMode mSM;
};

#endif
