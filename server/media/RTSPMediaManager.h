#ifndef _CHANNELMANAGER_H
#define _CHANNELMANAGER_H 
#include "IdleTask.h"
#include "RTSPClientSession.h"
#include "H264FramesManager.h"
#include "H264Rtp.h"
class TCPClientSession;
#define MAX_CONNECT 256     //ÿ��ͨ���������256���ͻ�������
#define MAX_CHANNEL_NUM 64  //64·ͨ�� 0λ���ݲ�ʹ��

class H264MediaManager : public Task
{
    public:
        H264MediaManager();
        virtual ~H264MediaManager();

        /*����ִ����*/
        virtual SInt64 Run();

		void killSelf();
        /*��ͻ���������м���������*/
        int addSession(ClientSession *newSession);
		int addTcpClientSession(TCPClientSession *newSession);


		/*��ȡ������������*/
		CacheManager *getCacheManager(void);
        /*��ȡͨ����ָ������*/
        static H264MediaManager **getChannel();

        /*���Ӽ����������*/
        void addActiveNum();
        
        /*��С�����������*/
        void subActiveNum();
		
		/*����TCPClientSession ������*/
		void addActiveTCPClientSessionNum();
		
        /*��СTCPClientSession ������*/
		void subActiveTCPClientSessionNum();

        char* getTrack1(); //track1
        char* getTrack2(); //track=1

        /*����ͨ��������*/
        static void addActiveChannelNum();

        //��Сͨ��������
        static void subActiveChannelNum();
	

        //���ͨ��������
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
        /*������ɾ������*/
        int delSession(int index);
        int delSession(ClientSession *toDelSession,int index);

		/*������ɾ������*/
        int delTCPClientSession(int index);
        int delTCPClientSession(ClientSession *toDelSession,int index);
    private:
		/*UDP Client Session */
		ClientSession *cSession[MAX_CONNECT]; //�ͻ��������б�(���ӵ���ͨ���Ŀͻ�����)
		int clientSessionNum;   //��������е�������
        int activeCSessionNum;  //����������

		/*TCP Client Session*/
		TCPClientSession *tSession[MAX_CONNECT];
		int TCPClientSessionNum;       /*TCPClientSession ������*/
		int TCPClientActiveCSessionNum; /*TCPClientSession ����������*/
		
		CacheManager *caManager;
		
		
        char const* fTrackId;   //track

        static H264MediaManager *cManager[MAX_CHANNEL_NUM];  //ͨ�����б�
        static unsigned int activeChannelNum; //����ͨ����

        H264RtpBuffer* fRTPPacket;   //RTP���
        
        unsigned char *sps; //��ͨ����Ƶ�� sps
        unsigned int spsSize;
        unsigned char *pps; //��ͨ����Ƶ�� pps
        unsigned int ppsSize;
        int profileID;  //

        int channelID;
};
#endif
