#ifndef _CHANNELMANAGER_H
#define _CHANNELMANAGER_H 
#include "IdleTask.h"
#include "RTSPClientSession.h"
#include "H264FramesManager.h"
#include "H264Rtp.h"
class TCPClientSession;
#define MAX_CONNECT 256     //每个通道最多允许256个客户端任务
#define MAX_CHANNEL_NUM 64  //64路通道 0位置暂不使用

class H264MediaManager : public Task
{
    public:
        H264MediaManager();
        virtual ~H264MediaManager();

        /*任务执行体*/
        virtual SInt64 Run();

		void killSelf();
        /*向客户端任务队列加入新任务*/
        int addSession(ClientSession *newSession);
		int addTcpClientSession(TCPClientSession *newSession);


		/*获取缓冲区管理类*/
		CacheManager *getCacheManager(void);
        /*获取通道类指针数组*/
        static H264MediaManager **getChannel();

        /*增加激活任务计数*/
        void addActiveNum();
        
        /*减小激活任务计数*/
        void subActiveNum();
		
		/*增加TCPClientSession 任务数*/
		void addActiveTCPClientSessionNum();
		
        /*减小TCPClientSession 任务数*/
		void subActiveTCPClientSessionNum();

        char* getTrack1(); //track1
        char* getTrack2(); //track=1

        /*增加通道激活数*/
        static void addActiveChannelNum();

        //减小通道激活数
        static void subActiveChannelNum();
	

        //获得通道激活数
        static int getActiveChannelNum();

        int setH264SPS(unsigned char *data,int framesize);
        int setH264PPS(unsigned char *data,int framesize);

		bool initH264SPS();
		bool initH264PPS();
		
   		inline void setID(int place){channelID=place;}
		inline int getID(){return channelID;}
    public:
        static int runChannelNum;
    private:
        /*按索引删除任务*/
        int delSession(int index);
        int delSession(ClientSession *toDelSession,int index);

		/*按索引删除任务*/
        int delTCPClientSession(int index);
        int delTCPClientSession(ClientSession *toDelSession,int index);
    private:
		/*UDP Client Session */
		ClientSession *cSession[MAX_CONNECT]; //客户端任务列表(连接到该通道的客户端数)
		int clientSessionNum;   //任务队列中的任务数
        int activeCSessionNum;  //激活任务数

		/*TCP Client Session*/
		TCPClientSession *tSession[MAX_CONNECT];
		int TCPClientSessionNum;       /*TCPClientSession 任务数*/
		int TCPClientActiveCSessionNum; /*TCPClientSession 激活任务数*/
		
		CacheManager *caManager;
		
		
        char const* fTrackId;   //track

        static H264MediaManager *cManager[MAX_CHANNEL_NUM];  //通道类列表
        static unsigned int activeChannelNum; //激活通道数

        H264RtpBuffer* fRTPPacket;   //RTP打包
        
        unsigned char *sps; //该通道视频流 sps
        unsigned int spsSize;
        unsigned char *pps; //该通道视频流 pps
        unsigned int ppsSize;
        int profileID;  //

        int channelID;
};
#endif
