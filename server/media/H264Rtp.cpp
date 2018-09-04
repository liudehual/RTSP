#include "H264Rtp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OS.h"
H264RtpBuffer::H264RtpBuffer():RtpBuffer((unsigned char)96)
{
    setRate(25);
}
H264RtpBuffer::~H264RtpBuffer()
{

}
void H264RtpBuffer::addFrameToBuffer(unsigned char *frame,unsigned fLen)
{
	 	
	 
     if(fLen<=4){
        return;
     }
     
     frame +=4;   //跳过头4字节
     fLen  -=4;   //长度减少4字节
     #if 0 //关闭帧类型检查
     if((((unsigned char)frame[0]) & 0x1F)==5){
		 fprintf(stderr,"%s %s %d %s I Frame Len is  %d\n",_SDK_PRINT_FLAG_,__FUNCTION__,__LINE__,"Not I Frame But too large",fLen+4);
     }
     if((((unsigned char)frame[0]) & 0x1F)==1){
		 fprintf(stderr,"%s %s %d %s P Frame Len is  %d\n",_SDK_PRINT_FLAG_,__FUNCTION__,__LINE__,"Too Large P Frame",fLen+4);
     }
     if((((unsigned char)frame[0]) & 0x1F)==7){
		 fprintf(stderr,"%s %s %d %s SPS Frame Len is  %d \n",_SDK_PRINT_FLAG_,__FUNCTION__,__LINE__,"Too Large I Frame",fLen+4);
     }
     if((((unsigned char)frame[0]) & 0x1F)==8){
		 fprintf(stderr,"%s %s %d %s PPS Frame Len is  %d \n",_SDK_PRINT_FLAG_,__FUNCTION__,__LINE__,"Too Large I Frame",fLen+4);
     }
     #endif
     if(((((unsigned char)frame[0]) & 0x1F)!=8) 
     			 && ((((unsigned char)frame[0]) & 0x1F)!=5)
     			 && ((((unsigned char)frame[0]) & 0x1F)!=6)){
	     getTimeStamp();

     }
     unsigned fUsedLen=0;
     int start=1;
     if((fLen)<(PACKET_MAX_SIZE-12)){
       RtpPacket * packet=getFreePacket();
       if(packet==NULL)return;
       setRtpHeader(packet);
       packet->setPacketLen(fLen+12);
       unsigned char *buf=packet->getPacketBuffer();
       //buf[0]=0x80 | (frame[0] & 0x1F);
       //buf[0]=(frame[0] & 0xE0) | 28;
       //packet->skipBityes(1);
       buf=packet->getPacketBuffer();
       memcpy(buf,&frame[0],fLen);
     }else{
       while(1){
        if(start){
            RtpPacket * packet=getFreePacket();
            if(packet==NULL) break;
            setRtpHeader(packet);
            packet->setPacketLen(PACKET_MAX_SIZE);
            unsigned char *buf=packet->getPacketBuffer();
            buf[0]=(frame[0] & 0xE0) | 28;
            buf[1]=0x80 | (frame[0] & 0x1F); 
            packet->skipBityes(2);
            buf=packet->getPacketBuffer();
            memcpy(buf,&frame[1],PACKET_MAX_SIZE-14);
            fUsedLen+=PACKET_MAX_SIZE-14+1;
            start=0;
        }else{ 
            if(fLen-fUsedLen>PACKET_MAX_SIZE-14){
                RtpPacket * packet=getFreePacket();
                if(packet==NULL) break;
                setRtpHeader(packet);
                packet->setPacketLen(PACKET_MAX_SIZE);
                unsigned char *buf=packet->getPacketBuffer(); 
                buf[0] = (frame[0] & 0xE0) | 28;
                buf[1] = frame[0]&~0x60 | (frame[0] & 0x1F);
                packet->skipBityes(2);
                buf=packet->getPacketBuffer();
                memcpy(buf,&frame[fUsedLen],PACKET_MAX_SIZE-14);
                fUsedLen+=PACKET_MAX_SIZE-14;
            }else{
                RtpPacket * packet=getFreePacket();
                if(packet==NULL) break;
                setRtpHeader(packet); 
                packet->setPacketLen(fLen-fUsedLen+14);
                unsigned char *buf=packet->getPacketBuffer(); 
                buf[0] = (frame[0] & 0xE0) | 28;
                buf[1] = 0x40 | (frame[0] & 0x1F);
                packet->skipBityes(2);
                buf=packet->getPacketBuffer();
                memcpy(buf,&frame[fUsedLen],fLen-fUsedLen);
                break;
           }
        }
      }
   }
}
unsigned H264RtpBuffer::getTimeStamp()
{
   return fTimeStamp+=(unsigned int)(90000.0/fRate);
	#if 0 /*时间戳为当前时间*/
	  fTimeStamp=(unsigned int)OS::Milliseconds();
	  fprintf(stderr,"%s %d TimeStamp %d\n",__FUNCTION__,__LINE__,fTimeStamp);
	  return fTimeStamp;
	#endif
}
