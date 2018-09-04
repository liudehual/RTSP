/*
	
*/
#ifndef __RTP_SESSION_H__
#define __RTP_SESSION_H__
#include "EventJob.h"

class RTSPClient;
class FrameContainer;
class RtpPacketContainer;
class MemoryPool;

/**/
#define FRAME_NUMS 30 //����1������
class RTPSession:public EventJob
{
	public:
		RTPSession(RTSPClient *rtspClient);
		virtual ~RTPSession();
		virtual int Processor();
		int initUdpSocket();
		int setSessionId(char *sessionStr);
		char *getSessionId();

		int setSps(unsigned char *buf,unsigned int bufLen);
		unsigned char * getSps();
		unsigned int getSpsLen();
		
		int setPps(unsigned char *buf,unsigned int bufLen);
		unsigned char * getPps();
		unsigned int getPpsLen();

		void setOldTimeStamp(unsigned int tTimeStamp){oldTimeStamp=tTimeStamp;}
		unsigned int getOldTimeStamp(){return oldTimeStamp;}

		void addUseableFrameNumbers(){useableFrameNumbers++;}
		void subUseableFrameNumbers(){if(useableFrameNumbers>0)useableFrameNumbers--;}
		unsigned int getUseableFrameNumbers(){return useableFrameNumbers;}

		void addFrameNums(){frameNums++;}
		void subFrameNums(){if(frameNums>0)frameNums--;}
		unsigned int getFrameNums(){return frameNums;}

		FrameContainer * getHandleFrame();
		
 		/*֡����*/
		int qSort();
		void qSort(FrameContainer *arr[], int low, int high);  
		
		int resetFrameContainer();
		int clearFrameContainer();
	private:
		int	insertToContainer(RtpPacketContainer *packet);
	private:
	  char sessionIdBuf[32];
	  FrameContainer **fContainer; //ÿ�ο��ܶ�Ҫ������������

	  unsigned int useableFrameNumbers;

	  unsigned char *spsBuf; 
	  unsigned int spsLen;

	  unsigned char *ppsBuf;
	  unsigned int ppsLen;

	  unsigned int minTimeStamp; /*��Сʱ���*/
	  unsigned int maxTimeStamp; /*���ʱ���*/
	  unsigned int frameNums;/*֡��������С*/
	  unsigned int oldTimeStamp;

	  bool roundTimeStamp;
};

/*
	֡����
*/
#define RTP_PACKET_CONTAINER_NUM 2*1024 /*һ֡���������1024*2����,���3M*/
#define DEFAULT_FRAME_SIZE 100   /*Ĭ��֡��С 150K*/
#define EXTEND_FRAME_SIZE  100   /*ÿ����չ֡��С 100*1500�ֽ�*/
#define MAX_FRAME_BUF_SIZE 1000  /*���һ֡��������1.5M*/
class FrameContainer
{
	public:
		FrameContainer();
		~FrameContainer();
		bool getEmpty(){return empty;}
		void setEmpty(bool iEmpty){empty=iEmpty;}
		/*�������кŽ�rtp�����뵽֡�����еĺ���λ��*/
		int insert(RtpPacketContainer *rpContainer);
		int qSort();
		/*Right???*/
		void qSort(RtpPacketContainer *arr[], int low, int high);  

		int ifWeLostPacket();
		int resetFrameContainer();
		unsigned char getFrameType();
		int setFrameType(unsigned char fType);
		unsigned int getTimeStamp(){return timeStamp;}
		int setTimeStamp(unsigned int tStamp){timeStamp=tStamp;return 1;}

		int getPacketNums(){return rtpPacketNums;}

		RtpPacketContainer *getRtpPacket(int i){return rpcBuf[i];}
		int getFrameLen(){return frameLen;}
	private:
		int extendFrameBuf();
	private:
		RtpPacketContainer **rpcBuf;

		bool empty; //��ʶ��ǰ֡����Ϊ��
		unsigned int rtpPacketNums;

		unsigned char frameType;
		unsigned int timeStamp;


		int frameLen;
};

/*
	RTP Packet ����

*/
/*
	�˴����ܻ��ж�ռ��һЩ�ڴ� (ע��) �Ժ�汾��Ľ�
*/
/*
	���ִ���ʽǷ�� �˴����Ըĳɶ�̬��
		1.�趨һ��Ĭ��ֵ,���ڸ�ֵ�Ľ������䣬С�ڸ�ֵ��ֱ�ӿ���(�����޸Ļ������Ϊ���ӵĲ������)
		2.�趨һ����ֵ(�����MTU����,���ܻ�����ڴ��˷�,������ߴ����ٶ�)
*/
#define RTP_PACKET_SIZE 1504 /*Ĭ��bufΪһ��MTU��С,����Ӧʹ����С�����ӽ�MTU+4(h264 nul ͷ 0001)��������˷Ѵ����ڴ�*/

#define FRAME_START 0x01 //ͷ��
#define FRAME_MIDDLE 0x02 //�м��
#define FRAME_END 0x04 //β��
#define FRAME_COMPLETE 0x08 //������

#define P_FRAME_TYPE 0x01 //P֡
#define I_FRAME_TYPE 0x05 //I֡
#define S_FRAME_TYPE 0x06 //s֡
#define SPS_FRAME_TYPE 0x07  //SPS֡
#define PPS_FRAME_TYPE 0x08  //PPS֡

/*���������İ�����һ�����ݵ�������Ϣ(��Ҫ��)*/
class RtpPacketContainer
{
	public:
		RtpPacketContainer();
		~RtpPacketContainer();
		
		unsigned char *getPacketStartOffset(){return buf;}
		unsigned char *getFrameDataStartOffset();
		unsigned int getPacketLen(){return length;}
		unsigned int getFrameDataLen();

		int parserPacket();
		//�ú�����Ҫ�����ǽ�ָ�����⴫��
		int getFreeSpaceAndLen(unsigned char *&tBuf,unsigned int *&len);
		
		void setFreeStat(bool f){mFree=f;}
		bool getFreeStat(){return mFree;}
		bool insertHeader();
		void setStat(unsigned char tStat){stat &= 0x00;stat |= tStat;} 	
		unsigned char getStat(){return stat;}
		void setFrameType(unsigned char fType){frameType&= 0x00;frameType|= fType;}
		unsigned char getFrameType(){return frameType;}
		void resetContainer();
		unsigned int getTimeStamp(){return timeStamp;}
		unsigned short getCseq(){return fCseq;}
	private:
		/*��bufװ��������rtp��������*/
		unsigned char *buf;
		unsigned int length; //buf��ָ��Ļ������е�ʵ�����ݳ��ȣ���������buf�ĳ���

		/*rtpͷ��һЩ����*/
		unsigned timeStamp;  //ʱ���
		unsigned short fCseq; //���к�

		unsigned char stat; //rtp����֡�е�λ��

		unsigned char frameType;

		bool mFree; //�����Ƿ����
};

/*
	�ڴ��(rtp packet container)
*/

#define DEFAULT_MEMORY_POOL_SIZE 256 // 256*1500=0.75MB 
#define EXTEND_MEMORY_POOL_SIZE 64 //Ĭ��ÿ����չ256
#define MAX_MEMORY_POOL_SIZE 3*1024 //4*1024*1500=4.5MB ֡�� 25f/s ���� 1MB/s  ���ֵΪ4.5M�ڴ�

class MemoryPool:public Object
{
	public:
		MemoryPool();
		~MemoryPool();
		RtpPacketContainer *getFreeContainer();
		void extendMemoryPool();
		void resetMemoryPool();
	private:
		RtpPacketContainer **mPool;
		int memoryPoolActualSize;
};
#endif
