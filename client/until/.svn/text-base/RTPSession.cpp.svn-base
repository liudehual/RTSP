#include "RTPSession.h"
#include "UDPSocket.h"
#include "RTSPClient.h"
#include "Until.h"
#include "H264Session.h"
#include "Engine.h"
#include <string.h>
#include "Log.h"
RTPSession::RTPSession(RTSPClient *rtspClient):EventJob(rtspClient,new UDPSocket),
							fContainer(NULL),spsBuf(NULL),spsLen(0),
							ppsBuf(NULL),ppsLen(0),frameNums(FRAME_NUMS),minTimeStamp(0),
							maxTimeStamp(0),useableFrameNumbers(0),roundTimeStamp(false)
{
	
	initUdpSocket();
	client->addObjectToQueue(new MemoryPool(),MemoryObjectType);
	fContainer=new FrameContainer*[FRAME_NUMS];

	for(int i=0;i<frameNums;++i){
		fContainer[i]=new FrameContainer;
	}
	memset(sessionIdBuf,'\0',32);

	/*add H264Session*/
	H264Session *session=new H264Session(client,/*10 ms*/20);
	session->setRTPSession(this);
	client->addObjectToQueue(session,H264ObjectType);
	((Engine*)(client->getObjectFromQueue(EngineObjectType)))->addElemToTimeoutQueue(session);
	
}

RTPSession::~RTPSession()
{
	fprintf_debug("delete RTPSession\n");
	for(int i=0;i<frameNums;++i){
		delete fContainer[i];
	}
	delete[] fContainer;

	if(spsBuf!=NULL){
		delete[] spsBuf;
		spsBuf=NULL;
	}
	if(ppsBuf!=NULL){
		delete[] ppsBuf;
		ppsBuf=NULL;
	}
}
int RTPSession::Processor()
{
	MemoryPool *pool=(MemoryPool *)(client->getObjectFromQueue(MemoryObjectType));
	RtpPacketContainer *rpc=pool->getFreeContainer();
	if(rpc==NULL){ //容量超了
		pool->resetMemoryPool(); /*清空整个buffer?*/
		rpc=pool->getFreeContainer();
	}
	unsigned char *rtpBuf=NULL;
	unsigned int *rtpOutLen=NULL;
	rpc->getFreeSpaceAndLen(rtpBuf,rtpOutLen);
	unsigned int ip=getSocket()->strIpToUInt32(getSocket()->getRemoteStrIp());
	unsigned short port=getSocket()->getRemotePort();

	/*收到数据并解析*/
	#if 1
	((UDPSocket *)getSocket())->recvFrom(&ip,&port, (void *)rtpBuf,RTP_PACKET_SIZE,rtpOutLen);
	//fprintf(stderr,SIMPLE_RTSP"\n");
	//fprintf(stderr,SIMPLE_RTSP"rtpOutLen %d\n",*rtpOutLen);
	#if 0 //测试收到的数据是否正确

	for(int i=0;i<(*rtpOutLen);++i){
		if(i%16==0){
			fprintf_msg("\n");
		}
		fprintf_debug("%02X ",rtpBuf[i]);
	}
	#endif

	/*解析包*/
	if(!rpc->parserPacket()){ 
		rpc->resetContainer(); //解析错误，重置
		return 1;
	}
	
    /*注:以下逻辑代码假设rtp包到达的顺序是完全乱序的*/

	/*
		分析完成，插入到buf中,测试用
		1.排序测试(OK)
		2.rtp包组成一帧数据成功(不知是什么原因，暴风影音播放时速度很快(设为减速播放后正常，无花屏现象，无卡顿，视频正常播放))
	*/
	#if 0
		FrameContainer *fc=fContainer[0];
		if(!(fc->insert(rpc))){
			fc->resetFrameContainer();

		}
		if(rpc->getStat()==FRAME_END || rpc->getStat()==FRAME_COMPLETE){ //检测到时尾包
			//fprintf_debug("packetNums %d \n",fc->getPacketNums());

			fc->qSort(); //排序

			if(!(fc->ifWeLostPacket())){
				fprintf_debug("Lost Some Packet\n");
			}else{

				#if 0 /*写入文件测试,参数解析是否错误*/
				static bool initI=false;
				/*打开文件*/
				static FILE *fp=NULL;
				static bool initFp=false;
				if(!initFp){
					fp=fopen("/home/test.264","ab+");
			//		fwrite(this->getSps(),this->getSpsLen(),1,fp);
			//		fwrite(this->getPps(),this->getPpsLen(),1,fp);
			//		fflush(fp);
					initFp=true;
				}
				/*I 帧 先写入SPS PPS信息*/
				if(fc->getFrameType()==I_FRAME_TYPE){
					fwrite(this->getSps(),this->getSpsLen(),1,fp);
					fwrite(this->getPps(),this->getPpsLen(),1,fp);
					fflush(fp);
					initI=true;
				}
				if(initI){
					unsigned char header[]={0,0,0,1};
					fwrite(header,4,1,fp);	
					fflush(fp);
					for(int i=0;i<fc->getPacketNums();++i){
					RtpPacketContainer *pp=fc->getRtpPacket(i);
					fwrite(pp->getFrameDataStartOffset(),pp->getFrameDataLen(),1,fp);
					}
					fflush(fp);
				}
				#endif 
				fprintf_debug("Not Lost Packet\n");
			}
			fc->resetFrameContainer();
		}

	#endif

	/*将包插入到Container中*/
	if(!insertToContainer(rpc)){
		if(rpc!=NULL){
			rpc->resetContainer(); //缓冲已满或数据过时
		}/*满了*/
		return 1;
	}

	/*
		执行到这一步我们已经完成了包的分析与插入，如果想获取一帧数据则要进行如下步骤
		1.对已经获取的数据进行排序
		2.检查是否有丢包
		3. 
			1. I帧丢包处理规则
			2. P帧丢包处理规则
			

	*/

	/*
		该处选择的规则是每次收到尾包的时候进行取帧
	*/
	#if 0 /*无缓冲时间 测试用*/
	if(rpc->getStat()==FRAME_END || rpc->getStat()==FRAME_COMPLETE){ 
		
	}
	#endif
	#if 1 /*有缓冲时间*/

		
	#endif
    /*检查每包数据的帧类型*/
	#if 0
	if(rpc->getFrameType()==I_FRAME_TYPE){
		fprintf_debug("---------------------------------> I Frame <-------------------------------\n");
	}
	if(rpc->getFrameType()==P_FRAME_TYPE){
		fprintf_debug("---------------------------------> P Frame <-------------------------------\n");
	}
	if(rpc->getFrameType()==S_FRAME_TYPE){
		fprintf_debug("---------------------------------> S Frame <-------------------------------\n");
	}
	if(rpc->getFrameType()==SPS_FRAME_TYPE){
		fprintf_debug("---------------------------------> SPS Frame <-------------------------------\n");
	}
	if(rpc->getFrameType()==PPS_FRAME_TYPE){
		fprintf_debug("---------------------------------> PPS Frame <-------------------------------\n");
	}
	#endif
	#if 0
	fprintf_msg("DataLen %d\n",dataLen);
	fprintf_msg("\n");
	#endif
	#if 0
	rpc->resetContainer();
	#endif
	
	#endif

	#if 0 //测试网络
	char buf[1500];
	unsigned int outLen;
	fSocketA->recvFrom(&ip,&port, (void *)buf,RTP_PACKET_SIZE,&outLen);

	fprintf_debug("outLen %d\n",outLen);
	#endif
	return 1;
}

int RTPSession::initUdpSocket()
{
	Socket *tSocket=(UDPSocket *)getSocket();
	tSocket->makeSocketNonBlocking(tSocket->getSocketFD());
#if 0
	unsigned short tPort=20000;
	for(;tPort<65535;++tPort){
	 if((fSocketA->bind(fSocketA->strIpToUInt32(fSocketA->getLocalStrIp()),tPort))==1){
		fSocketA->setLocalPort(tPort);
		break;
	 }
	}
#endif
	return 1;
}
int RTPSession::setSessionId(char *sessionStr)
{
	if(sessionStr==NULL){
		return 0;
	}
	memset(sessionIdBuf,0,sizeof(sessionIdBuf));
	sprintf(sessionIdBuf,"%s",sessionStr);
}
char *RTPSession::getSessionId()
{
	return sessionIdBuf;
}

int RTPSession::setSps(unsigned char *buf,unsigned int bufLen)
{
	if(buf==NULL || bufLen<=0){
		return 0;
	}
	spsBuf=new unsigned char[bufLen];

	memcpy(spsBuf,buf,bufLen);
	spsLen=bufLen;
	return 1;
}
unsigned char * RTPSession::getSps()
{
	return spsBuf;
}
unsigned int RTPSession::getSpsLen()
{
	return spsLen;
}
int RTPSession::setPps(unsigned char *buf,unsigned int bufLen)
{
	if(buf==NULL || bufLen<=0){
		return 0;
	}
	ppsBuf=new unsigned char[bufLen];
	memcpy(ppsBuf,buf,bufLen);

	ppsLen=bufLen;
	return 1;
}
unsigned char * RTPSession::getPps()
{
	return ppsBuf;
}
unsigned int RTPSession::getPpsLen()
{
	return ppsLen;
}

FrameContainer * RTPSession::getHandleFrame()
{
	return fContainer[0];
	
}

int	RTPSession::insertToContainer(RtpPacketContainer *packet)
{
	if(packet==NULL){
		return 0;
	}
	if(packet->getStat()==FRAME_START || packet->getStat()==FRAME_COMPLETE){
		packet->insertHeader();
	}
	/*
		此处虽然有两次循环，但一般情况下只在插入一帧第一个包的时候才会产生循环
	*/
	/*找个合适的位置插入*/
	for(int i=0;i<frameNums;++i){
		FrameContainer *container=fContainer[i];
		if((container!=NULL)&& !(container->getEmpty())){
			 if(container->getTimeStamp()==packet->getTimeStamp()){
			 	#if 1 /*以下代码处理pps I帧同一个时间戳的情况*/
			 				 	
			 	#endif
				container->insert(packet);
				return 1;
			 }else{
				continue;
			 }
		}
	}
	/*
		第一次循环未找到，此处会出现两种情况

		1.新的一帧数据到达(未必是头包数据)
		2.过时数据到达(如何判断及处理)
	*/
	
	/*先排一下序*/
	int flag=this->qSort();
	if(flag){
		FrameContainer *fc=fContainer[0];
		do{
			if(!roundTimeStamp){/*未发生循环*/
				if(fc->getTimeStamp()>packet->getTimeStamp()){
					return 0;
				}else{
					break;
				}
			}else{ /*发生循环*/
				unsigned int packetTimeStamp=packet->getTimeStamp();
				FrameContainer *fc1=fContainer[getUseableFrameNumbers()];
				if(((packetTimeStamp)-(fc1->getTimeStamp()))>=360000*5/*时间戳比最新收到帧的时间戳大360000(即相隔了500帧)*/ 
							&& packetTimeStamp<fc->getTimeStamp()/*时间戳比最早收到的帧数据的时间戳还要大*/){
					/*该包数据的时间戳不再有效范围内，丢弃*/
					return 0;
				}
			}
		}while(0);
	}

	#if 0 /*Test */
	if(useableFrameNumbers>90){
		for(int i=0;i<frameNums;++i){
			FrameContainer *container=fContainer[i];
			if((container!=NULL)){
				container->resetFrameContainer();
			}
			useableFrameNumbers=0;
		}	
	}
	#endif
	
	/*每收到一帧数据的，我们增加一下帧数*/
	addUseableFrameNumbers();

	/*没找着，找第一个空位插入*/
	for(int i=0;i<frameNums;++i){
		FrameContainer *container=fContainer[i];
		if((container!=NULL)&&(container->getEmpty())){
			container->insert(packet);
			return 1;
		}
	}
	
	/*没有合适的位置也没有合适的空位，返回0*/
	return 0; //
}
int RTPSession::qSort()
{
	roundTimeStamp=false;/*我们假设不会发生循环*/

	int flag=useableFrameNumbers-1;
	if(flag<0){
		return 0;
	}
	if(flag==0){
		return 1;
	}
	if(flag==frameNums-1){ 
		clearFrameContainer(); //满了,我们返回1 好让rtpSession 继续运行
		return 1;
	}
	#if 0
	fprintf_debug("Start quick Sort useableFrameNumbers %d\n",useableFrameNumbers);
	#endif
	/*快排*/
	qSort(fContainer,0,useableFrameNumbers-1);	

	#if 0 /*打印排序后的结果*/
	for(int i=0;i<rtpPacketNums;++i){
			fprintf_msg("-%d",rpcBuf[i]->getCseq());
	}
	fprintf_msg("\n\n");
	fprintf_msg("End quick Sort rtpPacketNums %d\n",rtpPacketNums);
	#endif
	int critical=0; //临界点
	if(fContainer[0]->getTimeStamp()==0 && (useableFrameNumbers-1)>0){ //时间戳 循环,重新排序一下 timeStamp 0-4294967295 不断循环
		for(int i=0;i<(useableFrameNumbers-1);++i){
			//fprintf(stderr,SIMPLE_RTSP"%d %d \n",fContainer[i]->getTimeStamp(),fContainer[i+1]->getTimeStamp());
			if((fContainer[i+1]->getTimeStamp()-fContainer[i]->getTimeStamp())>3600000){ /*	1.循环产生 2.中间我们丢了以前帧. 找到临界(此处假定1000(30-40s)帧的时间间隔)*/
				critical=i+1;
				break;
			}
		}
		if(critical==0){ //timeStamp结果正确，只是是从0开始的
			return 1;
		}
		roundTimeStamp=true;/*发生了*/
		FrameContainer * tmpRpcBuf[FRAME_NUMS]={NULL}; 
		for(int j=0;j<critical;++j){ //取出小timeStamp
			tmpRpcBuf[j]=fContainer[j];
		}
		FrameContainer* tmpRpcBuf1[FRAME_NUMS]={NULL};
		for(int i1=critical;i1<useableFrameNumbers;++i1){ //取出大timeStamp
			tmpRpcBuf1[i1-critical]=fContainer[i1];
		}
		for(int i2=0;i2<useableFrameNumbers-critical;++i2){ //将大timeStamp 放入buf前半部分
			fContainer[i2]=tmpRpcBuf1[i2];
		}

		for(int i3=0;i3<critical;++i3){ //将小timeStamp 放入buf后半部分
			fContainer[i3+(useableFrameNumbers-critical)]=tmpRpcBuf[i3]; 
		}
	}
	#if 0 /*测试产生timeStamp循环后的操作是否正确*/
		for(int i=0;i<useableFrameNumbers;++i){
			fprintf_msg("<TimeStamp is >%d",fContainer[i]->getTimeStamp());
		}
		fprintf_msg("\n\n");
		#endif
	/*完成*/
	return 1;

}

void RTPSession::qSort(FrameContainer *arr[], int low, int high)
{
	#if 1 /*version 3*/
	if(low<high)  
    {  
    int i = low;  
    int j = high;  
    FrameContainer *x = arr[i];  
  
    while(i<j)  
    {  
        while(i<j&&arr[j]->getTimeStamp()>=x->getTimeStamp())  
            j--;  
        if(i<j){  
            arr[i] = arr[j];  
            i++;  
        }  
        while(i<j&&arr[i]->getTimeStamp()<x->getTimeStamp())  
           i++;  
      	if(i<j){  
       		arr[j] = arr[i];  
       		j--;  
       	}  
    }  
    	arr[i] = x;  
    	qSort(arr, low, i-1);  
    	qSort(arr, i+1, high);  
    }  
    #endif
}
int RTPSession::resetFrameContainer()
{
   FrameContainer *fc=fContainer[0];
   fc->resetFrameContainer();
   fContainer[0]=fContainer[useableFrameNumbers];
   fContainer[useableFrameNumbers]=fc;
   return 1;
}
int RTPSession::clearFrameContainer()
{
	for(int i=0;i<frameNums;++i){
		fContainer[i]->resetFrameContainer();
		
	}
	useableFrameNumbers=0;
	return 1;
}
/*
	帧容器
*/
FrameContainer::FrameContainer():rpcBuf(NULL),					
							empty(true),rtpPacketNums(0),frameType(0),timeStamp(0),frameLen(0)
{
	rpcBuf=new RtpPacketContainer*[RTP_PACKET_CONTAINER_NUM]; //包容器
	for(int i=0;i<RTP_PACKET_CONTAINER_NUM;++i){
		rpcBuf[i]=NULL;
	}	
}
FrameContainer::~FrameContainer()
{	
	delete[] rpcBuf; 
}
int FrameContainer::extendFrameBuf() /*此处并没有实现数据的赋值，建议先检查当前buf是否能容纳一阵长度的数据，若不能，则进行扩展*/
{
	return 1;
}
int FrameContainer::insert(RtpPacketContainer *rpContainer)
{
	if(rpContainer==NULL){
		return 0;
	}
	#if 1
	for(int i=0;i<RTP_PACKET_CONTAINER_NUM;++i){
		if(rpcBuf[i]==NULL){ //当前帧容器为空闲状态，直接插入到帧容器中
			frameLen+=rpContainer->getPacketLen();
			rpcBuf[i]=rpContainer;
			rtpPacketNums++;
			this->setEmpty(false);
			this->setFrameType(rpContainer->getFrameType());
			this->setTimeStamp(rpContainer->getTimeStamp());
			return 1;
		}
	}	
	#endif
	return 0; /*满了?*/

}

int FrameContainer::qSort()
{
	int flags=rtpPacketNums-1;
	if(flags<0){
		return 0;
	}
	if(flags==0){
		return 1;
	}
	//fprintf(stderr,SIMPLE_RTSP"Start quick Sort rtpPacketNums %d\n",rtpPacketNums);

	/*快排*/
	qSort(rpcBuf,0,rtpPacketNums-1);	

	#if 0 /*打印排序后的结果*/
	for(int i=0;i<rtpPacketNums;++i){
			fprintf(stderr,SIMPLE_RTSP"-%d",rpcBuf[i]->getCseq());
	}
	fprintf(stderr,SIMPLE_RTSP"\n\n");
	fprintf(stderr,SIMPLE_RTSP"End quick Sort rtpPacketNums %d\n",rtpPacketNums);
	#endif
	int critical=0; //临界点
	if(rpcBuf[0]->getCseq()==0 && (rtpPacketNums-1)>0){ //cseq 循环,重新排序一下 Cseq 0-65535 不断循环
		for(int i=0;i<(rtpPacketNums-1);++i){
		//	fprintf(stderr,SIMPLE_RTSP"%d %d \n",rpcBuf[i]->getCseq(),rpcBuf[i+1]->getCseq());
			if((rpcBuf[i+1]->getCseq()-rpcBuf[i]->getCseq())>5535){ //找到临界(此处假定一帧数据拆包个数不会大于60000个(即一帧不应大于58MB))
				critical=i+1;
				break;
			}
		}

		if(critical==0){ //fCseq结果正确，只是是从0开始的
			return 1;
		}

		RtpPacketContainer* tmpRpcBuf[RTP_PACKET_CONTAINER_NUM]={NULL}; 
		for(int j=0;j<critical;++j){ //取出小Cseq
			tmpRpcBuf[j]=rpcBuf[j];
		}
		
		RtpPacketContainer* tmpRpcBuf1[RTP_PACKET_CONTAINER_NUM]={NULL};
		for(int i1=critical;i1<rtpPacketNums;++i1){ //取出大Cseq
			tmpRpcBuf1[i1-critical]=rpcBuf[i1];
		}

		for(int i2=0;i2<rtpPacketNums-critical;++i2){ //将大Cseq 放入buf前半部分
			rpcBuf[i2]=tmpRpcBuf1[i2];
		}

		for(int i3=0;i3<critical;++i3){ //将小Cseq 放入buf后半部分
			rpcBuf[i3+(rtpPacketNums-critical)]=tmpRpcBuf[i3]; 
		}
		#if 0 /*测试产生cseq循环后的操作是否正确*/
		for(int i=0;i<rtpPacketNums;++i){
			fprintf_msg("<>%d",rpcBuf[i]->getCseq());
		}
		fprintf_msg("\n\n");
		#endif
	}
	/*完成*/
	return 1;

}

void FrameContainer::qSort(RtpPacketContainer *arr[], int low, int high)  
{  
	#if 1 /*version 3*/
	if(low<high)  
    {  
    int i = low;  
    int j = high;  
    RtpPacketContainer *x = arr[i];  
  
    while(i<j)  
    {  
        while(i<j&&arr[j]->getCseq()>x->getCseq())  
            j--;  
        if(i<j){  
            arr[i] = arr[j];  
            i++;  
        }  
        while(i<j&&arr[i]->getCseq()<x->getCseq())  
           i++;  
      	if(i<j){  
       		arr[j] = arr[i];  
       		j--;  
       	}  
    }  
    	arr[i] = x;  
    	qSort(arr, low, i-1);  
    	qSort(arr, i+1, high);  
    }  
    #endif
}

int FrameContainer::resetFrameContainer()
{
	for(int i=0;i<RTP_PACKET_CONTAINER_NUM;++i){
		if(rpcBuf[i]!=NULL){
			rpcBuf[i]->resetContainer();
			rpcBuf[i]=NULL;
		}
	}
	rtpPacketNums=0;
	frameLen=0;
	this->setEmpty(true);
	return 1;
}

unsigned char FrameContainer::getFrameType()
{
	return frameType;
}

int FrameContainer::setFrameType(unsigned char fType)
{
	return frameType=fType;
}

int FrameContainer::ifWeLostPacket()
{
	int flags=rtpPacketNums-1;
	if(flags<0){
		return 0;
	}
	if(flags==0){
		return 1;
	}
	for(int i=0;i<(rtpPacketNums-1);++i){
		int tCseq=rpcBuf[i+1]->getCseq()-rpcBuf[i]->getCseq();
	//	fprintf_msg("i-%d [i+1]-%d ",rpcBuf[i]->getCseq(),rpcBuf[i+1]->getCseq());
		if(tCseq==1 || tCseq==-65535){
			continue;
		}else{
			return 0;
		}
	}
	return 1;
}

/*
	RTP包容器

*/
RtpPacketContainer::RtpPacketContainer():mFree(true),length(0)
{
	buf=new unsigned char[RTP_PACKET_SIZE];
}
RtpPacketContainer::~RtpPacketContainer()
{
//	fprintf_debug("delete RtpPacketContainer\n");
	delete[] buf;
}
int RtpPacketContainer::getFreeSpaceAndLen(unsigned char *&tBuf,unsigned int *&len)
{
	tBuf=buf;
	len=&length;
	return 1;
}
bool RtpPacketContainer::insertHeader()
{
	unsigned char headerBuf[4]={0,0,0,1};
	unsigned headerBufLen=sizeof(headerBuf);
	int pos=0;
	if(this->getStat()==FRAME_COMPLETE){pos=12;}
	else if(this->getStat()==FRAME_START){pos=13;}
	memmove(&buf[pos+headerBufLen],&buf[pos],length); //
	memcpy(&buf[pos],headerBuf,headerBufLen);
	length+=headerBufLen;
	return true;
}

void RtpPacketContainer::resetContainer()
{
	setFreeStat(true);
	timeStamp=0;
	fCseq=0;
	stat=0;
	frameType=0;
}
int RtpPacketContainer::parserPacket()
{
	unsigned char *ptr=buf;
	unsigned int rtpHead=ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3]; //提取RTP头的头四个字节
	
	#if 1
	if ((rtpHead&0xC0000000) != 0x80000000){ //做一个简单的检查
		fprintf_msg("RTP Packet Error\n");
		return 0;
	}
	#endif
	unsigned short tCseq=ptr[2]<<8 | ptr[3]; //提取序列号
	fCseq=tCseq;

	#if 0
	fCseq=ntohs(tCseq);
	#endif

	unsigned int tTimeStamp=ptr[4]<<24 | ptr[5]<<16 | ptr[6]<<8 | ptr[7]; //提取时间戳
	#if 0
	timeStamp=ntohl(tTimeStamp);
	#endif 
	timeStamp=tTimeStamp;

	unsigned char flags=ptr[12] & 0x1F; //提取分包类型
	if(flags==28 || flags==29){
		 if((ptr[13] & 0x80)==0x80){ //头包
			setStat(FRAME_START);
			ptr[13]=(ptr[12] & 0xE0) | (ptr[13] & 0x1F);
			//fprintf(stderr,SIMPLE_RTSP"Frame First Packet--------------\n");
		 }else if((ptr[13] & 0x40)==0x40){ //尾包
			setStat(FRAME_END);
		 }else if((ptr[13] & 0x80)!=0x80 
		 		&& (ptr[13] & 0x40)!=0x40){ //中间包
			setStat(FRAME_MIDDLE);
		 }
		 setFrameType((ptr[13] &0x1F)); //设置帧类型
	}else{
		setStat(FRAME_COMPLETE);	//一包包含完整帧
		setFrameType((ptr[12] &0x1F)); //设置帧类型
	}
	#if 0 //输出分析结果
	fprintf_debug("fCseq %u timeStamp %u FU Type %d FramePlace %d Frame Type %d\n",fCseq,timeStamp,flags,stat,frameType);
	#endif
	//默认收到数据即为可用数据,设置该容器为非空闲状态
	setFreeStat(false);
	return 1;
}
unsigned char * RtpPacketContainer::getFrameDataStartOffset()
{
	if(getStat()==FRAME_COMPLETE){ //一包数据即为一个完整帧,跳过RTP头
		return buf+12;  
	}else if(getStat()==FRAME_START){ //分包帧头包,跳过13字节(RTP头加一个额外的字节)
		return buf+13;
	}
	return buf+14; //分包帧尾帧、中间帧，跳过14字节(rtp头加两个个额外的字节)
}
unsigned int RtpPacketContainer::getFrameDataLen()
{
	if(getStat()==FRAME_COMPLETE){  //一包数据即为一个完整帧,跳过RTP头
		return length-12;
	}else if(getStat()==FRAME_START){ //分包帧头包,跳过13字节(RTP头加一个额外的字节)
		return length-13;
	}
	return length-14; //分包帧尾帧、中间帧，跳过14字节(rtp头加两个个额外的字节)
}

MemoryPool::MemoryPool():memoryPoolActualSize(DEFAULT_MEMORY_POOL_SIZE)
{
	mPool=new RtpPacketContainer*[memoryPoolActualSize];
	for(int i=0;i<memoryPoolActualSize;++i){
		mPool[i]=new RtpPacketContainer;
	}
}
MemoryPool::~MemoryPool()
{
	fprintf_debug("delete MemoryPool\n");

	for(int i=0;i<memoryPoolActualSize;++i){
		delete mPool[i];
	}
	delete[] mPool;
	mPool=NULL;
}
RtpPacketContainer *MemoryPool::getFreeContainer()
{
	for(int i=0;i<memoryPoolActualSize;++i){
		if(mPool[i]->getFreeStat()){
			return mPool[i];
		}
	}
	//缓冲区已满
	/*
		处理方式 1.截断
				 2.重新申请并扩大缓冲区(OK)
	*/
	extendMemoryPool();

	/*重新申请内存后再找一次*/
	for(int i=0;i<memoryPoolActualSize;++i){
		if(mPool[i]->getFreeStat()){
			return mPool[i];
		}
	}
	
	/*达到申请上限，返回NULL*/
	return NULL;
}
void MemoryPool::extendMemoryPool()
{
	if((memoryPoolActualSize+EXTEND_MEMORY_POOL_SIZE)>MAX_MEMORY_POOL_SIZE){
		fprintf_warn("Memory pool is max size %d\n",memoryPoolActualSize+EXTEND_MEMORY_POOL_SIZE);
		return;
	}
	RtpPacketContainer	**tPool=new RtpPacketContainer*[memoryPoolActualSize+EXTEND_MEMORY_POOL_SIZE];
	for(int i=0;i<memoryPoolActualSize;++i){
		tPool[i]=mPool[i];
	}
	delete[] mPool;
	mPool=NULL;
	for(int j=memoryPoolActualSize;j<memoryPoolActualSize+EXTEND_MEMORY_POOL_SIZE;++j){
		tPool[j]=new RtpPacketContainer;
	}
	memoryPoolActualSize+=EXTEND_MEMORY_POOL_SIZE;
	mPool=tPool;
}
void MemoryPool::resetMemoryPool()
{
	for(int i=0;i<memoryPoolActualSize;++i){
		mPool[i]->resetContainer();
	}
}
