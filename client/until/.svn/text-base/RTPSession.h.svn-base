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
#define FRAME_NUMS 30 //保留1秒数据
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
		
 		/*帧排序*/
		int qSort();
		void qSort(FrameContainer *arr[], int low, int high);  
		
		int resetFrameContainer();
		int clearFrameContainer();
	private:
		int	insertToContainer(RtpPacketContainer *packet);
	private:
	  char sessionIdBuf[32];
	  FrameContainer **fContainer; //每次可能都要遍历整个队列

	  unsigned int useableFrameNumbers;

	  unsigned char *spsBuf; 
	  unsigned int spsLen;

	  unsigned char *ppsBuf;
	  unsigned int ppsLen;

	  unsigned int minTimeStamp; /*最小时间戳*/
	  unsigned int maxTimeStamp; /*最大时间戳*/
	  unsigned int frameNums;/*帧缓冲区大小*/
	  unsigned int oldTimeStamp;

	  bool roundTimeStamp;
};

/*
	帧容器
*/
#define RTP_PACKET_CONTAINER_NUM 2*1024 /*一帧允许最大拆成1024*2个包,最大3M*/
#define DEFAULT_FRAME_SIZE 100   /*默认帧大小 150K*/
#define EXTEND_FRAME_SIZE  100   /*每次扩展帧大小 100*1500字节*/
#define MAX_FRAME_BUF_SIZE 1000  /*最大一帧数不超过1.5M*/
class FrameContainer
{
	public:
		FrameContainer();
		~FrameContainer();
		bool getEmpty(){return empty;}
		void setEmpty(bool iEmpty){empty=iEmpty;}
		/*根据序列号将rtp包插入到帧容器中的合适位置*/
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

		bool empty; //标识当前帧容器为空
		unsigned int rtpPacketNums;

		unsigned char frameType;
		unsigned int timeStamp;


		int frameLen;
};

/*
	RTP Packet 容器

*/
/*
	此处可能会有多占用一些内存 (注意) 以后版本会改进
*/
/*
	该种处理方式欠佳 此处可以改成动态的
		1.设定一个默认值,大于该值的进行扩充，小于该值的直接拷入(按此修改会产生较为复杂的插入过程)
		2.设定一个定值(按最大MTU考虑,可能会产生内存浪费,但会提高处理速度)
*/
#define RTP_PACKET_SIZE 1504 /*默认buf为一个MTU大小,所以应使包大小尽量接近MTU+4(h264 nul 头 0001)，否则会浪费大量内存*/

#define FRAME_START 0x01 //头包
#define FRAME_MIDDLE 0x02 //中间包
#define FRAME_END 0x04 //尾包
#define FRAME_COMPLETE 0x08 //完整包

#define P_FRAME_TYPE 0x01 //P帧
#define I_FRAME_TYPE 0x05 //I帧
#define S_FRAME_TYPE 0x06 //s帧
#define SPS_FRAME_TYPE 0x07  //SPS帧
#define PPS_FRAME_TYPE 0x08  //PPS帧

/*该类完整的包含了一包数据的所有信息(必要的)*/
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
		//该函数主要功能是将指针向外传递
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
		/*该buf装的是整个rtp包的数据*/
		unsigned char *buf;
		unsigned int length; //buf所指向的缓冲区中的实际数据长度，而非整个buf的长度

		/*rtp头的一些属性*/
		unsigned timeStamp;  //时间戳
		unsigned short fCseq; //序列号

		unsigned char stat; //rtp包在帧中的位置

		unsigned char frameType;

		bool mFree; //容器是否空闲
};

/*
	内存池(rtp packet container)
*/

#define DEFAULT_MEMORY_POOL_SIZE 256 // 256*1500=0.75MB 
#define EXTEND_MEMORY_POOL_SIZE 64 //默认每次扩展256
#define MAX_MEMORY_POOL_SIZE 3*1024 //4*1024*1500=4.5MB 帧率 25f/s 码率 1MB/s  最大值为4.5M内存

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
