#ifndef _PMEDIA_RTP_H_
#define _PMEDIA_RTP_H_

#define PACKETS_MAX_SIZE 3*1024   //每帧可以分成的最大包数
#define PACKETS_DEFAULT_SIZE 1    //默认每帧可分的报数
#define PACKET_MAX_SIZE  1400     //每包最大数据量
class RtpPacket
{
    public:
        RtpPacket():fCurrentIndex(0),fSeqNo(0),fTimeStamp(0),pLen(0){}
        ~RtpPacket(){};
        unsigned char *getPacketBuffer(){return &pBuffer[fCurrentIndex];}
        void setPacketLen(unsigned len){pLen=len;}
        unsigned char * getPacketBufStart(){return &pBuffer[0]; }
        unsigned getPacketLen(){return pLen;}
        void setAvailable(){available=1;}
        void setUnavailable(){available=0;}
        unsigned getAvailable(){return available;}
        void skipBityes(unsigned size){fCurrentIndex+=size;};
        void reset()
        {
            fCurrentIndex=0;
            setUnavailable();
            fTimeStamp=0;
            fSeqNo=0;
        }
    private:
        unsigned char pBuffer[PACKET_MAX_SIZE];  //此处暂时采用缓冲区
        unsigned pLen;
        unsigned short fSeqNo;
        unsigned fTimeStamp;
        unsigned available;

        unsigned fCurrentIndex;
};

class RtpBuffer
{
    public:
        RtpBuffer(unsigned char RTPPayloadType);
        virtual ~RtpBuffer();

        virtual void addFrameToBuffer(unsigned char *frame,unsigned fLen)=0;
        virtual unsigned getTimeStamp()=0;
        RtpPacket *getFreePacket();
        RtpPacket *getRtpPacket();
		void setStreamId(unsigned int id);
        void reset();

	  int	addPacketQueue(int pNum);
    protected:
        void setRate(unsigned rate){fRate=rate;}
        void setRtpHeader(RtpPacket *packet);
        unsigned short fSeqNo;
        unsigned fTimeStamp;
        unsigned fRate;
        unsigned fSSRC;
        unsigned char fRTPPayloadType;
		
    private:
        unsigned lastIndex;  //标记缓冲区内一共有多少RtpPacket
        unsigned currIndex;  //当前要用到的RtpPacket索引
        RtpPacket *recordPacketBuffer; //此处默认一帧数据不会大于3M,大于则会被截断
        int flag;

		int packetNum;
};


#endif
