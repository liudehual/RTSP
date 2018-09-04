/*
	frame manager 2.0
	
*/

#ifndef _H264FramesManager_H_
#define _H264FramesManager_H_
#include <stdio.h>
#include <stdlib.h>
class FrameContainer;
class RtpPacketContainer;
class OSMutex;
#define MANAGER 0 /* 0 H264FramesManager 1 CacheManager*/ 

/*
	该处代码用于替代H264FrameManager类 已测，可用
	原理
		CacheManager 管理readQueue
	new Code
	add by gct 1608091706
*/

class FrameManager
{
	public:
		FrameManager();
		~FrameManager();
		void cleanContainer();
		void setFreeStat(bool tFree){mFree=tFree;}
		bool getFreeStat(){return mFree;}
		int writeData(unsigned char *buffer,unsigned int bufferLen);
		int readData(unsigned char *&buffer,unsigned int &bufferLen);
		unsigned char *getBuf(){return buf;}
		unsigned int getBufLen(){return bufLen;}
		void setContentBufLen(unsigned int len){contentLen=len;}

		int extendMemory(unsigned int acLen);
	private:
		unsigned char *buf;
		unsigned int bufLen; /*缓冲区总长度*/
		unsigned int contentLen; /*数据长度*/
		bool mFree;
};
class CacheManager
{

	public:
		CacheManager();
		~CacheManager();
		int writeData(unsigned char *buffer,unsigned int bufferLen);
		int readData(unsigned char *&buffer,unsigned int &bufferLen);

		int writeData(FrameContainer *tFc);
	
		void swapQueue();
		void cleanCache();
	private:
		FrameManager **readQueue;
		int readQueueLen;
		FrameManager **writeQueue;
		int writeQueueLen;
		OSMutex *mutex;                 //同步锁
		bool firstRead;
};
#endif

