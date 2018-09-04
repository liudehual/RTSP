#include "Rtp.h"
#include "myUntil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "OS.h"
RtpPacket *RtpBuffer::getFreePacket()
{
    if(lastIndex>=packetNum){
//		reset(); //空间已满，重置
		addPacketQueue(lastIndex+20);
    }
    return &recordPacketBuffer[lastIndex++];
    
}
RtpPacket *RtpBuffer::getRtpPacket()
{
    if(currIndex>=lastIndex) return NULL;
    return &recordPacketBuffer[currIndex++];
}
void RtpBuffer::setStreamId(unsigned int id)
{
	fSSRC=id;
}
void RtpBuffer::reset()
{
    for(int i=0;i<=lastIndex;++i){
      recordPacketBuffer[i].reset(); 
    }
    lastIndex=0;
    currIndex=0;
}
void RtpBuffer::setRtpHeader(RtpPacket *packet)
{
	if(fSeqNo-flag!=1){
	  fprintf(stderr,"%d %u %d\n",fSeqNo-flag,fSeqNo,flag);
    }
    flag=fSeqNo;
		
	
    //设置RTP前四个字节
    unsigned rtpHdr = 0x80000000; // RTP 版本 2
    rtpHdr |= (fRTPPayloadType<<16);
    
    rtpHdr |= fSeqNo++; // 序列号
 
    unsigned hdr=htonl(rtpHdr);
	
    unsigned char *buf= packet->getPacketBuffer();
    memmove(buf,(unsigned char *)&hdr,4);
    packet->skipBityes(4);
    

    //添加时间戳
    buf= packet->getPacketBuffer();
    unsigned ts=htonl(fTimeStamp);
    memmove(buf,(unsigned char *)&ts,4);
    packet->skipBityes(4);

    //添加SSRC    
    buf= packet->getPacketBuffer();
    unsigned fss=htonl(fSSRC);
    memmove(buf,(unsigned char *)&fss,4);
    packet->skipBityes(4);
}

RtpBuffer::RtpBuffer(unsigned char RTPPayloadType):fRTPPayloadType(RTPPayloadType)
{
    currIndex=0;
    lastIndex=0;
    fTimeStamp=(unsigned int)OS::Milliseconds();    
    fSeqNo=_our_random32();
    fSSRC=0;
    flag=0;
	packetNum=PACKETS_DEFAULT_SIZE;
    recordPacketBuffer=new RtpPacket[packetNum];
    
}
RtpBuffer::~RtpBuffer()
{
	delete[] recordPacketBuffer;
	recordPacketBuffer=NULL;
}
int	RtpBuffer::addPacketQueue(int pNum)
{
	
	if(pNum<=packetNum){
		return 0;
	}
	if(pNum>=PACKETS_MAX_SIZE){
		pNum=PACKETS_MAX_SIZE;
		
	}
	if(packetNum>=PACKETS_MAX_SIZE){ //达到申请上限,不再申请
		reset();
		return 0;  
	}
	RtpPacket *ptr=new RtpPacket[pNum];
	for(int i=0;i<packetNum;++i){
		ptr[i]=recordPacketBuffer[i];
	}
	packetNum=pNum;
//	fprintf(stderr,"Packet Number is %d\n",packetNum);
	delete[] recordPacketBuffer;
	recordPacketBuffer=ptr;
	
	return 1;
}

