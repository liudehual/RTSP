#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include "H264FramesManager.h"
#include "OSMutex.h"
#include "RTSPMediaManager.h"
#if 0
H264FramesManager::H264FramesManager(H264MediaManager *manager):cManager(manager){

  mutex=new OSMutex;

  FindSpsPps=false;
  firstRead=true;
  writeFlag=true;

  rFrames=new describeFrames;
  wFrames=new describeFrames;

  rFrames->bufLen=FRAME_DEFAULT_SIZE;
  wFrames->bufLen=FRAME_DEFAULT_SIZE;
  
  rFrames->ptr=new unsigned char[rFrames->bufLen];
  rFrames->frames=0;
  rFrames->currPos=rFrames->ptr;
  rFrames->rightCount=FRAME_DEFAULT_SIZE;
   	
  wFrames->ptr=new unsigned char[wFrames->bufLen];
  wFrames->frames=0;
  wFrames->currPos=wFrames->ptr;
  wFrames->rightCount=FRAME_DEFAULT_SIZE;

  rStart=true;
  readEndData=rFrames->ptr;

  /*
  fp=fopen("my.h264","wb+");
  */
}

H264FramesManager::~H264FramesManager() 
{
#ifdef __WIN32
  Sleep(100);
#else
  usleep(100*1000);
#endif
  delete [] rFrames->ptr;
  delete [] wFrames->ptr;
  delete rFrames;
  delete wFrames;
  delete mutex;
}
void H264FramesManager::resetWriteBuffer()
{
  wFrames->currPos=wFrames->ptr;
  wFrames->frames=0;
  wFrames->rightCount=wFrames->bufLen;
}
int H264FramesManager::WriteFrame(unsigned char *data, int len,int frameNums)
{
  int length=len;
  //没有帧数据或不允许写数据
  if(len<=0){return 0;}

  #if 0
  //获取sps pps数据(注册之前尝试初始化)
  if(!FindSpsPps &&(((unsigned char)data[4]&0x1F)==7 || ((unsigned char)data[4]&0x1F)==8)){ //
		
		  static bool sps=false;
          static bool pps=false;

		  /*单帧*/
          if(frameNums==1){
			  if(((unsigned char)data[4]&0x1F)==7){cManager->setH264SPS(data,len);sps=true;}
		      if(((unsigned char)data[4]&0x1F)==8){cManager->setH264PPS(data,len);pps=true;}  
          }
          
          /*多帧 67 68 65 帧在一起*/
         if(frameNums>1){
         	 unsigned char *dataPtr=data;
         	 unsigned int spsLen=0;
         	 unsigned int ppsLen=0;
			 if(((unsigned char)data[4]&0x1F)==7){
				for(int i=0;i<len;++i){
					if(dataPtr[i]==0 && dataPtr[i+1]==0 && dataPtr[i+2]==0 && dataPtr[i+3]==1 
							&& ((unsigned char)dataPtr[i+4]&0x1F)==8){
						spsLen=i;
						if(((unsigned char)dataPtr[4]&0x1F)==7){
							cManager->setH264SPS(dataPtr,spsLen);
							sps=true;
						}
					}
					if(dataPtr[i]==0 && dataPtr[i+1]==0 && dataPtr[i+2]==0 && dataPtr[i+3]==1 
							&& ((unsigned char)dataPtr[i+4]&0x1F)==5){
						ppsLen=i-spsLen;
						if(((unsigned char)dataPtr[spsLen+4]&0x1F)==8){
							cManager->setH264PPS(&dataPtr[spsLen],ppsLen);
							pps=true;
						}  
					}
  		 	 	}
			 }
         }
         if(sps && pps){FindSpsPps=true;} //sps pps 全部初始化完成，关闭，不再写入数据
  }
  #endif
  /*
  static int iLen=0;
  static int fLen=0;
  if(((unsigned char)data[4]&0x1F)==5){
    fwrite(data,len,1,fp);
    fflush(fp);
    fLen=ftell(fp);
    iLen+=len;
    fprintf(stderr,"H264FramesManager::WriteFrame %d %d\n",iLen,fLen);
  }
  */

   mutex->Lock();
	
  //@@@@@@@@@@@@@@@@@@@@@@@@@@ add by gct 1607071453 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	do{
		if(length>wFrames->bufLen){
			if(wFrames->bufLen>FRAME_MAX_SIZE){
				length=wFrames->bufLen; //大于 FRAME_MAX_SIZE,截断
				break;
  			}
  		
			int newLen=wFrames->bufLen+(length-wFrames->rightCount);
			unsigned char *ptr=new unsigned char[newLen];

			int usedCount=wFrames->bufLen-wFrames->rightCount; //获得已使用缓冲区大小

			memcpy(ptr,wFrames->ptr,usedCount);

			delete[] wFrames->ptr;
		
			wFrames->ptr=ptr;
		
			wFrames->bufLen=newLen; //重新计算整个缓冲区的长度
			wFrames->rightCount=wFrames->bufLen-usedCount; //重新计算剩余缓冲区大小
			wFrames->currPos=wFrames->ptr;   //为wPos指针重新赋值
			wFrames->currPos+=usedCount; //重定位wPos指针
		//	fprintf(stderr,"new BufferLen is %d\n",wFrames->bufLen);
		}
	}while(0);
	
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  /*
  if(len>bufferLen){ //帧长度大于缓冲区最大长度，截断
      length=bufferLen;
  }
  mutex->Lock();
  */
  if(length>wFrames->rightCount){ // 判断剩余空间是否可以容纳帧数据
    resetWriteBuffer();
  } 
  
  memcpy(wFrames->currPos,data,length);
  wFrames->currPos+=length;
  wFrames->frames+=frameNums;
  wFrames->rightCount-=length;
  mutex->Unlock();
  return 1;
}

int H264FramesManager::ReadFrame(unsigned char * &data, int & frameSize)
{
  // fprintf(stderr,"读缓冲区帧数 %d\n",rFrames->frames);
  if(firstRead){ //第一次读数据，清空所有缓冲区
     clearBuffer();
    firstRead=false;
  }
  if(rFrames->frames==0){
      changeBuffer();
      if(rFrames->frames == 0){
          return -1;
      }
  }
  rStart=true;
  rFrames->frames--;

  unsigned off=0;
  unsigned char *cursor=rFrames->currPos; //缓冲区游标
  for(unsigned i=0;i<(rFrames->bufLen)-4 && cursor!=readEndData;++i){
    if(cursor[0] == 0 && cursor[1] == 0 && ((cursor[2] == 0 
    	&& cursor[3] == 1) || cursor[2] == 1)){ //找到 nalu头
      if(rStart){
          rStart=false;
          cursor+=4;
          off+=4;
          continue;
      }else{break;}
    }
   ++off;
   ++cursor;
  }
  frameSize=off;
  data=rFrames->currPos; //此处将指针直接传入到发送缓冲区中，减少一层数据复制

  //记录下一帧的位置
  rFrames->currPos=cursor;
  return off;
}

void H264FramesManager::changeBuffer()
{
  mutex->Lock();
  describeFrames *dFrames=rFrames;
  rFrames=wFrames;
  readEndData=wFrames->currPos;
  wFrames=dFrames;
  wFrames->reset();  
  rFrames->currPos=rFrames->ptr; 
  mutex->Unlock();
}

void H264FramesManager::allowedWrite(void)
{
    writeFlag=true;
}

void H264FramesManager::notAllowedWrite(void)
{
    writeFlag=false;
}
void H264FramesManager::clearBuffer(void)
{
    mutex->Lock();
    memset(rFrames->ptr,0,rFrames->bufLen);
    rFrames->frames=0;
    rFrames->currPos=rFrames->ptr;
    rFrames->rightCount=rFrames->bufLen;
    
    memset(wFrames->ptr,0,wFrames->bufLen);
    wFrames->frames=0;
    wFrames->currPos=wFrames->ptr;
    wFrames->rightCount=wFrames->bufLen;
    mutex->Unlock();
}
int H264FramesManager::getReadFrames(void)
{
    return rFrames->frames;
}
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define DEFAULT_FRAME_SIZE 100*1024
#define MAX_FRAME_SIZE 2*1024*1024
FrameContainer::FrameContainer():buf(NULL),
					bufLen(0),contentLen(0),mFree(true)
{
	buf=new unsigned char[DEFAULT_FRAME_SIZE];
	bufLen=DEFAULT_FRAME_SIZE;
}
FrameContainer::~FrameContainer()
{

}
void FrameContainer::cleanContainer()
{
	/*just reset contentLen and container's stat*/
	contentLen=0; 
	setFreeStat(true);
}

int FrameContainer::writeData(unsigned char *buffer,unsigned int bufferLen)
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
int FrameContainer::readData(unsigned char *&buffer,unsigned int &bufferLen)
{
		buffer=buf;
		bufferLen=contentLen;
		return 1;
}

#define DEFAULT_QUEUE_LEN 1
CacheManager::CacheManager():readQueue(NULL),readQueueLen(0),
									writeQueue(NULL),writeQueueLen(0),firstRead(true),
									mutex(new OSMutex)
{
	readQueue=new FrameContainer*[DEFAULT_QUEUE_LEN];
	readQueueLen=DEFAULT_QUEUE_LEN;
	for(int i=0;i<readQueueLen;++i){
		readQueue[i]=new FrameContainer;
	}
	writeQueue=new FrameContainer*[DEFAULT_QUEUE_LEN];
	writeQueueLen=DEFAULT_QUEUE_LEN;
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]=new FrameContainer;
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
  	FrameContainer **tReadQueue=readQueue;
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
		FrameContainer *fc=writeQueue[i];
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
