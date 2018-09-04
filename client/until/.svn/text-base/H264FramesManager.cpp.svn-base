#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include "OSMutex.h"
#include "H264FramesManager.h"
#include "RTPSession.h"
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define DEFAULT_FRAME_SIZE 100*1024
#define MAX_FRAME_SIZE 2*1024*1024
FrameManager::FrameManager():buf(NULL),
					bufLen(0),contentLen(0),mFree(true)
{
	buf=new unsigned char[DEFAULT_FRAME_SIZE];
	bufLen=DEFAULT_FRAME_SIZE;
}
FrameManager::~FrameManager()
{

}
void FrameManager::cleanContainer()
{
	/*just reset contentLen and container's stat*/
	contentLen=0; 
	setFreeStat(true);
}

int FrameManager::writeData(unsigned char *buffer,unsigned int bufferLen)
{
	/*we have Max frame size  limit*/
	if(bufferLen>MAX_FRAME_SIZE){
		return 0;
	}
	/*we don't have enough buffer,so we should create new buffer that enough big to contain the data*/
	if(bufferLen>bufLen){
		bufLen+=bufferLen-bufLen;
		unsigned char *tBuf=new unsigned char[bufLen];
		delete[] buf; //delete old array
		buf=tBuf;
	}
	memcpy(buf,buffer,bufferLen);
	contentLen=bufferLen;

}
int FrameManager::readData(unsigned char *&buffer,unsigned int &bufferLen)
{
		buffer=buf;
		bufferLen=contentLen;
		return 1;
}
int FrameManager::extendMemory(unsigned int acLen)
{
	delete[] buf;
	buf=new unsigned char[acLen];
	bufLen=acLen;
	return 1;
}

#define DEFAULT_QUEUE_LEN 2
CacheManager::CacheManager():readQueue(NULL),readQueueLen(0),
									writeQueue(NULL),writeQueueLen(0),firstRead(true),
									mutex(new OSMutex)
{
	readQueue=new FrameManager*[DEFAULT_QUEUE_LEN];
	readQueueLen=DEFAULT_QUEUE_LEN;
	for(int i=0;i<readQueueLen;++i){
		readQueue[i]=new FrameManager;
	}
	writeQueue=new FrameManager*[DEFAULT_QUEUE_LEN];
	writeQueueLen=DEFAULT_QUEUE_LEN;
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]=new FrameManager;
	}
}
CacheManager::~CacheManager()
{
	for(int i=0;i<readQueueLen;++i){
		delete readQueue[i];
		readQueue[i]=NULL;
	}
	delete readQueue;
	for(int i=0;i<writeQueueLen;++i){
		delete writeQueue[i];
		writeQueue[i]=NULL;
	}
	delete writeQueue;
}

void CacheManager::swapQueue()
{

	/*swap readQueue and writeQueue*/
  	mutex->Lock();
  	FrameManager **tReadQueue=readQueue;
  	readQueue=writeQueue;
  	writeQueue=tReadQueue;

	int tLen=readQueueLen;
  	readQueueLen=writeQueueLen;
	writeQueueLen=tLen;
    mutex->Unlock();
}
void CacheManager::cleanCache()
{
	/*clean readQueue and writeQueue*/
	for(int i=0;i<readQueueLen;++i){
		readQueue[i]->cleanContainer();
		readQueue[i]->setFreeStat(true);
	}
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]->cleanContainer();
		writeQueue[i]->setFreeStat(true);
	}
}
int CacheManager::writeData(unsigned char *buffer,unsigned int bufferLen)
{
	if(bufferLen<=0){
		return 0;
	}
	
	//fprintf(stderr,"%s %d bufferLen %d\n",__FUNCTION__,__LINE__,bufferLen);
	mutex->Lock();
	for(int i=0;i<writeQueueLen;++i){
		FrameManager *fc=writeQueue[i];
		if(fc->getFreeStat()){
			fc->writeData(buffer,bufferLen);
			fc->setFreeStat(false);
			mutex->Unlock();	
			return 1;
		}
	}
	#if 1
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]->cleanContainer();
		writeQueue[i]->setFreeStat(true);
	}	
	#endif
	mutex->Unlock();
	return 0;
}
int CacheManager::readData(unsigned char *&buffer,unsigned int &bufferLen)
{
  if(firstRead){ //we
     cleanCache();
    firstRead=false;
  }
  //fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*get one frame*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			readQueue[i]->readData(buffer,bufferLen);
	//		fprintf(stderr,"%s %d %d\n",__FUNCTION__,__LINE__,bufferLen);
			readQueue[i]->setFreeStat(true);
			return 1;

		}
  }
  /*没取到，交换队列*/
  swapQueue();
 // fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*again*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			readQueue[i]->readData(buffer,bufferLen);
			readQueue[i]->setFreeStat(true);
		}
		return 1;
  }
  
  return 0;
}
int CacheManager::writeData(FrameContainer *tFc)
{
	if(tFc==NULL){
		return 0;
	}
	
	mutex->Lock();
	for(int i=0;i<writeQueueLen;++i){
		FrameManager *fc=writeQueue[i];
		if(fc->getFreeStat()){
			
			/*检查一下长度，有必要则扩展*/
			if(tFc->getFrameLen()>fc->getBufLen()){
				fc->extendMemory(tFc->getFrameLen());
			}
			
			/*获取buf 指针*/
			unsigned char *pos=fc->getBuf();
			/*复制数据*/
			for(int i=0;i<tFc->getPacketNums();++i){
				RtpPacketContainer *pp=tFc->getRtpPacket(i);
				memcpy(pos,pp->getFrameDataStartOffset(),pp->getFrameDataLen());
				pos+=pp->getFrameDataLen();
			}
			/*设置有效数据长度*/
			fc->setContentBufLen(tFc->getFrameLen());
			
			fc->setFreeStat(false);

			mutex->Unlock();	
			return 1;
		}
	}
	fprintf(stderr,"%s %d bufferLen %d\n",__FUNCTION__,__LINE__,tFc->getFrameLen());

	#if 1
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]->cleanContainer();
		writeQueue[i]->setFreeStat(true);
	}	
	#endif
	mutex->Unlock();
	return 1;

}

