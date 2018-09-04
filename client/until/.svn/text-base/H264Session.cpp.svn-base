#include "H264Session.h"
#include "RTPSession.h"
#include "Log.h"
#include "RTSPClient.h"
#include <string.h>
#include "H264FramesManager.h"
#define BUFERR_FRAME_SIZE 25

H264Session::H264Session(RTSPClient *rtspClient,unsigned long long mSec):TimeoutJob(rtspClient,mSec),
				fSession(NULL),cManager(NULL)
{
	cManager=new CacheManager();
}
H264Session::~H264Session()
{
	delete cManager;
}
int H264Session::Processor()
{
	if(fSession->getUseableFrameNumbers()>BUFERR_FRAME_SIZE){
		/*frame sort*/
		fSession->qSort();
		FrameContainer *fc=fSession->getHandleFrame();
        /*rtp packet sort*/
		fc->qSort();
		#if 0 /*check Cseq*/
		/*Test*/
		fprintf_debug("%s %d PacketNumber %d\n",__FUNCTION__,__LINE__,fc->getPacketNums());	
		fprintf_debug("%s %d Frame Type %d\n",__FUNCTION__,__LINE__,fc->getFrameType());
		for(int i=0;i<fc->getPacketNums();++i){
			RtpPacketContainer *ptk=fc->getRtpPacket(i);
			if(ptk!=NULL){
				//fprintf_debug("<%d>",ptk->getCseq());
			}
		}
		fprintf_debug("\n");
		#endif 
		/*检查每包数据的帧类型*/
		#if 0
			fprintf_debug("%s %d Time Stamp %d\n",__FUNCTION__,__LINE__,fc->getTimeStamp());

			if(fc->getFrameType()==I_FRAME_TYPE){
				fprintf_debug("---------------------------------> I Frame <-------------------------------\n");
			}
			if(fc->getFrameType()==P_FRAME_TYPE){
				fprintf_debug("---------------------------------> P Frame <-------------------------------\n");
			}
			if(fc->getFrameType()==S_FRAME_TYPE){
				fprintf_debug("---------------------------------> S Frame <-------------------------------\n");
			}
			if(fc->getFrameType()==SPS_FRAME_TYPE){
				fprintf_debug("---------------------------------> SPS Frame <-------------------------------\n");
			}
			if(fc->getFrameType()==PPS_FRAME_TYPE){
				fprintf_debug("---------------------------------> PPS Frame <-------------------------------\n");
			}
		#endif

		
		
		#if 0 /*文件写入测试*/
		{ 
			{
				#if 1 /*写入文件测试,参数解析是否错误*/
					static bool initI=false;
					/*打开文件*/
					static FILE *fp=NULL;
					static bool initFp=false;
					if(!initFp){
						fp=fopen("ClientTest.264","ab+");
						initFp=true;
					}
					{
						for(int i=0;i<fc->getPacketNums();++i){
							RtpPacketContainer *pp=fc->getRtpPacket(i);
							fwrite(pp->getFrameDataStartOffset(),pp->getFrameDataLen(),1,fp);
							fflush(fp);
						}
					}
				#endif
			}
		}
		#endif
		#if 0
		if(fc->getFrameLen()>tBufLen){
			delete[] tBuf;
			tBuf=new unsigned char[fc->getFrameLen()];
			tBufLen=fc->getFrameLen();
		}
		unsigned char *pos=tBuf;
		for(int i=0;i<fc->getPacketNums();++i){
			RtpPacketContainer *pp=fc->getRtpPacket(i);
			memcpy(pos,pp->getFrameDataStartOffset(),pp->getFrameDataLen());
			pos+=pp->getFrameDataLen();
		}
		client->getVideo(tBuf,fc->getFrameLen());
		#endif
		#if 1
		cManager->writeData(fc);
		#endif
		/*reset frameContainer buffer*/
		fSession->resetFrameContainer();
		/*sub frame numbers we can use */

		fSession->subUseableFrameNumbers();

	}
	return 1;
}
int H264Session::setRTPSession(RTPSession *session)
{

	fSession=session;
	return 1;
}

