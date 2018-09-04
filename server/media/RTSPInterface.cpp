#include "RTSPInterface.h"


//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "OS.h"
#include "OSHeaders.h"
#include "Task.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "UDPSocket.h"
#include "SocketUtils.h"
#include <fcntl.h>
#include <arpa/inet.h>
#include "RTSPListener.h"
#include "RTSPMediaManager.h"
#include "strDup.h"
#include "myUntil.h"

#include "OS.h"
#include "RTSPListener.h"


#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>
 
#include <unistd.h>



H264MediaManager ** chManager=NULL;
#define MAX_LISTENER_NUMBERS 64 
RtspListener *listener[MAX_LISTENER_NUMBERS]={NULL};
int startChannelsByNum(int chnNumbers)
{

	#if 1
	/*将可用文件描述符增长到最大*/
	#if __linux__

    //grow our pool of file descriptors to the max!
    struct rlimit rl;
    
    // set it to the absolute maximum that the operating system allows - have to be superuser to do this
    rl.rlim_cur = RLIM_INFINITY;
    rl.rlim_max = RLIM_INFINITY;
 
    setrlimit(RLIMIT_NOFILE, &rl);
	#endif
	#endif
	
	OS::Initialize();
	
	//通道检查
    if(chManager==NULL){
		chManager=H264MediaManager::getChannel();
    }
    //通道范围检查
    if(chnNumbers>MAX_CHANNEL_NUM || chnNumbers<=0){
		fprintf(stderr,"%s %d %s Range is 0-%d You channel is %d \n",__FUNCTION__,__LINE__,"Channel is error ",MAX_CHANNEL_NUM,chnNumbers);
		return 0;
    }
    //启动指定数量的通道
    H264MediaManager::runChannelNum=chnNumbers;
    for(int i=0;i<chnNumbers;++i){
        chManager[i]=new H264MediaManager;
        H264MediaManager::addActiveChannelNum();
        chManager[i]->setID(i);
    }
    return 1;
}
int startListener(unsigned short port)
{
		if(port<=0 || port>65535){
			fprintf(stderr,"port Error %d\n",port);
			return 0;
		}
	
		for(int i=0;i<MAX_LISTENER_NUMBERS;++i){
			if(listener[i]==NULL){
				listener[i]=new RtspListener;
				listener[i]->Initialize(INADDR_ANY,port);
    			listener[i]->RequestEvent(EV_RE);
    			break;
			}
		}
	}

#if 0
int startSpecialChannel(int channelIndex)
{
	//通道检查
    if(chManager==NULL){
		chManager=H264MediaManager::getChannel();
    }
    //索引范围检查
    if(channelIndex>MAX_CHANNEL_NUM || channelIndex<=0){
		fprintf(stderr,"%s %d %s Range is 0-%d You channel is %d \n",__FUNCTION__,__LINE__,"Channel is error ",MAX_CHANNEL_NUM,channelIndex);
		return -1;
    }
    //启动指定索引的通道
    if(chManager[channelIndex]==NULL){
    	H264MediaManager::runChannelNum++;
		chManager[channelIndex]=new H264MediaManager;
        H264MediaManager::addActiveChannelNum();
        chManager[channelIndex]->setID(channelIndex);
    }
    return 1;
}
#endif

int writeDataToChannel(int channel,unsigned char*buf,unsigned len)
{
	//通道检查
    if(chManager==NULL){
		chManager=H264MediaManager::getChannel();
    }
    
	if(channel>MAX_CHANNEL_NUM || channel<0){
		fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Channel is error ",channel);
		return -1;
	}
	//通道检查
    if(chManager[channel]==NULL){
    	fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Not Find channel Manager ",channel);
		return -1;
    }

    //长度出错
    if(len<=0){
    	#if 0
        fprintf(stderr," %s %d The Frame Len is %d\n",__FUNCTION__,__LINE__,len);
		#endif
        return -1;
    }

    //数据检查
    if(buf==NULL || len<4){
        fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"You write not right data",len);
	    return -1;
    }
  
    //数据头检查
    if(buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 1){
        fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"You write not right data",len);
	    return -1;
    }   

	
	#if 0 //调试帧类型
	fprintf(stderr,"ChannelId %d Frame Type %d Frame Len %d\n",channel,((unsigned char)buf[4]&0x1F),len);
	#endif
	#if 0 //帧个数检查 调试用
   	int frameNum1=0;
  	for(int i=0;i<len-4;++i){
		if(buf[i]==0 && buf[i+1]==0 && buf[i+2]==0 && buf[i+3]==1){
			#if 1
			if(((unsigned char)buf[4]&0x1F)==1){
				fprintf(stderr,"P Frame %d\n",((unsigned char)buf[4]&0x1F));
			}
			if(((unsigned char)buf[4]&0x1F)==5){
				fprintf(stderr,"I Frame %d\n",((unsigned char)buf[4]&0x1F));

			}
			if(((unsigned char)buf[4]&0x1F)==6){
				fprintf(stderr,"S Frame %d\n",((unsigned char)buf[4]&0x1F));

			}
			if(((unsigned char)buf[4]&0x1F)==7){
				fprintf(stderr,"SPS Frame %d\n",((unsigned char)buf[4]&0x1F));

			}
			if(((unsigned char)buf[4]&0x1F)==8){
				fprintf(stderr,"PPS Frame %d\n",((unsigned char)buf[4]&0x1F));

			}
			#endif
			frameNum1++;
		}
	}
	
    if(frameNum1>1){
      // for(int i=0;i<len;i++){fprintf(stderr,"%02X ",buf[i]);} 
       fprintf(stderr,"%s %d <-----------------------> Frame Numbers %d\n",__FUNCTION__,__LINE__,frameNum1);
	   //return 0;
    }            
	#endif

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ modify by gct 1607271010 @@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    return chManager[channel]->getCacheManager()->writeData(buf,len);

}
#if 0
int writeSPSInfo(int channel,unsigned char *data,int dataLen)
{
	//通道检查
    if(chManager==NULL){
		chManager=H264MediaManager::getChannel();
    }
    if(channel>MAX_CHANNEL_NUM || channel<0){
		fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Channel is error ",channel);
		return -1;
	}
	//通道检查
    if(chManager[channel]==NULL){
    	fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Not Find channel Manager ",channel);
		return -1;
    }
    if((data[4]&0x1F)!=7){
		return -1;
    }
    return chManager[channel]->setH264SPS(data,dataLen);

}
#endif
#if 0
int writePPSInfo(int channel,unsigned char *data,int dataLen)
{
	//通道检查
    if(chManager==NULL){
		chManager=H264MediaManager::getChannel();
    }
    if(channel>MAX_CHANNEL_NUM || channel<0){
		fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Channel is error ",channel);
		return -1;
	}
	//通道检查
    if(chManager[channel]==NULL){
    	fprintf(stderr,"%s %d %s %d\n",__FUNCTION__,__LINE__,"Not Find channel Manager ",channel);
		return -1;
    }
    if((data[4]&0x1F)!=8){
		return -1;
    }
    return chManager[channel]->setH264PPS(data,dataLen);

}
#endif
