#ifndef _PMEDIA_H264_RTP_H_
#define _PMEDIA_H264_RTP_H_
#include "Rtp.h"
class H264RtpBuffer:public RtpBuffer
{
public:
    H264RtpBuffer();
    ~H264RtpBuffer();

    //@@@@@@@@@@@@@@@@@@@@@@@@@ add by gct 1605100919 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    virtual void addFrameToBuffer(unsigned char *frame,unsigned fLen);
    virtual unsigned getTimeStamp();
};
#endif

