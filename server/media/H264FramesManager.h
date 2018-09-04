/*
	frame manager 2.0
	
*/

#ifndef _H264FramesManager_H_
#define _H264FramesManager_H_
#include <stdio.h>
#include <stdlib.h>

#define MANAGER 0 /* 0 H264FramesManager 1 CacheManager*/ 

class H264MediaManager;
class OSMutex;

#define FRAME_MAX_SIZE 3*1024*1024
#define FRAME_DEFAULT_SIZE 10*1024
	/* 
		该注释块代码测试可用，已被替换掉
	*/
#if 0
// 缓冲区描述
struct describeFrames
{
    int frames;             //缓冲区内的帧数量
    unsigned char *ptr;     //缓冲区指针
    unsigned int bufLen;
	unsigned int rightCount;
	unsigned char *currPos;
	void reset(){
		frames=0;
		rightCount=bufLen;
		currPos=ptr;
	}
    //重载=运算符
    describeFrames & operator=(describeFrames &src){
        this->frames=src.frames;
        this->ptr=src.ptr;
		this->bufLen=src.bufLen;
		this->rightCount=src.rightCount;
		this->currPos=src.currPos;
        return *this;
    }
};

// 帧管理
class H264FramesManager{

public:
    H264FramesManager(H264MediaManager *manager);
    ~H264FramesManager();

public:
    //写入帧数据 (函数有限制，一帧不可大于1M)
    int WriteFrame(unsigned char *data, int len,int frameNums);
    
    //读取帧数据
    int ReadFrame(unsigned char * &data,int & frameSize);
    
    //允许写入数据
    void allowedWrite(void);
    
    //不允许写入数据
    void notAllowedWrite(void);

    //清空buffer
    void clearBuffer(void);
    int getReadFrames(void);
private:
    //重置写缓冲区
    void resetWriteBuffer();
    
    //交换读写缓冲区并重置指针
    void changeBuffer(void);
    
private:
    struct describeFrames *rFrames; //读缓冲区
    struct describeFrames *wFrames; //写缓冲区
    bool rStart;                    //一次读取数据的开始
    unsigned char *readEndData;     //读缓冲区最后有效数据
    bool writeFlag;
    bool FindSpsPps;               //是否读取sps pps信息
    bool firstRead;
private:
   OSMutex *mutex;                 //同步锁
   H264MediaManager *cManager;
 //  FILE *fp;
};
#endif
/*
	该处代码用于替代H264FrameManager类 已测，可用
	原理
		CacheManager 管理readQueue
	new Code
	add by gct 1608091706
*/
class FrameContainer
{
	public:
		FrameContainer();
		~FrameContainer();
		void cleanContainer();
		void setFreeStat(bool tFree){mFree=tFree;}
		bool getFreeStat(){return mFree;}
		int writeData(unsigned char *buffer,unsigned int bufferLen);
		int readData(unsigned char *&buffer,unsigned int &bufferLen);

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

		void swapQueue();
		void cleanCache();
	private:
		FrameContainer **readQueue;
		int readQueueLen;
		FrameContainer **writeQueue;
		int writeQueueLen;
		OSMutex *mutex;                 //同步锁
		bool firstRead;

		
};
#endif

