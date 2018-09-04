#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__
#include "EventJob.h"
#include "Socket.h"

#if defined(__WIN32__) || defined(_WIN32) || defined(_QNX4)
#define _strncasecmp _strnicmp
#define snprintf _snprintf
#else
#define _strncasecmp strncasecmp
#endif

#define RESPONSE_BUFFER_SIZE 15*1024
#define REQUEST_BUFFER_SIZE  15*1024

class TCPSocket;
class RTSPClient;
class RTSPSession:public EventJob
{
	friend class HeartBeatSession;

	public:
		RTSPSession(RTSPClient *rtspClient,char *streamName);
		virtual ~RTSPSession();
		virtual int Processor();
		int Initialize(char *dstIp,unsigned short dstPort);
		int sendOPTIONSCommand();	
		int sendTEARDOWNCommand();
		int sendDESCRIBECommand();

	private:
		int handlerResponse(int rOutLen);
		void resetResponseBuffer();
		char* getLine(char* startOfLine);
		bool checkForHeader(char const* line, char const* headerName, unsigned headerNameLength, char const*& headerParams);
		bool parseResponseCode(char const* line, unsigned& responseCode, char const*& responseString);
		char const* commandName() const { return fCommandName; }
		int setCommandName(const char *cName);
		int resetCommandName();
		void setBaseURL(char const* url);
		unsigned int getRandom32();
		void setSessionID();
		char *getSessionID();

	private:
		int sendSETUPCommand();
		int sendPLAYCommand();
		int sendGET_PARAMETERCommand();

	private:
		int handleOPTIONSResponse();
		int handleDESCRIBEResponse(int channelNum,int contentLen,char *contentBuf);
		int handleSETUPResponse(int channelNum,char *transportStr,char *sessionStr);
		int handlePLAYResponse(int channelNum,char *infoStr);

	private:
		bool parseSDPLine(char const* inputLine,char const*& nextLine);
		bool parseSDPLine_s(char const* sdpLine);
		bool parseSDPLine_c(char const* sdpLine); 
		char* parseCLine(char const* sdpLine);
		bool parseSDPAttribute_control(char const* sdpLine);
		bool parseSDPAttribute_range(char const* sdpLine);
		bool parseRangeAttribute(char const* sdpLine, double& startTime, double& endTime); 
		bool parseSDPLine_i(char const* sdpLine);
		bool parseSDPAttribute_type(char const* sdpLine);
		bool parseSDPLine_c1(char const* sdpLine); 
		bool parseSDPLine_b(char const* sdpLine);
		bool parseSDPAttribute_rtpmap(char const* sdpLine); 
		bool parseSDPAttribute_control1(char const* sdpLine);
		bool parseSDPAttribute_range1(char const* sdpLine);
		bool parseSDPAttribute_fmtp(char const* sdpLine);
		bool parseSDPAttribute_x_dimensions(char const* sdpLine);
		bool parseSDPAttribute_framerate(char const* sdpLine); 
		bool parseTransportParams(char const* paramsStr,
					 char*& serverAddressStr, unsigned short& serverPortNum,unsigned short &clientPortNum,
					 unsigned char& rtpChannelId, unsigned char& rtcpChannelId);
		bool parseScaleParam(char const* paramStr, float& scale); 
		bool parseRTPInfoParams(char const*& paramsStr, u_int16_t& seqNum, u_int32_t& timestamp);

	private:
		char* fBaseURL;
		unsigned responseBufferSize;
        char channel;
        unsigned long long int state; // 随机函数中间状态
		int fResponseBufferBytesLeft;    //已使用空间大小
        int fResponseBytesAlreadySeen;   //剩余空间大小
        unsigned char* fLastCRLF;       //记录RTSP请求头中结尾的倒数第二个"\r\n"
		unsigned char fResponseBuffer[RESPONSE_BUFFER_SIZE];
		unsigned char fResquestBuffer[REQUEST_BUFFER_SIZE];

		/*此处为SDP参数，暂定放在RTSPSession类内*/
		 // Fields set from a SDP description:
  		char* fConnectionEndpointName;
  		double fMaxPlayStartTime;
  		double fMaxPlayEndTime;
  		struct in_addr fSourceFilterAddr; // used for SSM
  		float fScale; // set from a RTSP "Scale:" header
  		char* fMediaSessionType; // holds a=type value
  		char* fSessionName; // holds s=<session name> value
  		char* fSessionDescription; // holds i=<session description> value
  		char* fControlPath; // holds optional a=control: string

		// Fields set from a SDP description:
  		char* fConnectionEndpointName1; // may also be set by RTSP SETUP response
  		unsigned short fClientPortNum; // in host byte order
      	// This field is also set by initiate()
  		unsigned char fRTPPayloadFormat;
  		char* fSavedSDPLines;
  		char* fMediumName;
  		char* fCodecName;
  		char* fProtocolName;
  		unsigned fRTPTimestampFrequency;
  		char* fControlPath1; // holds optional a=control: string
  		struct in_addr fSourceFilterAddr1; // used for SSM
  		unsigned fBandwidth; // in kilobits-per-second, from b= line

  		// Parameters set by "a=fmtp:" SDP lines:
  		unsigned fAuxiliarydatasizelength, fConstantduration, fConstantsize;
  		unsigned fCRC, fCtsdeltalength, fDe_interleavebuffersize, fDtsdeltalength;
  		unsigned fIndexdeltalength, fIndexlength, fInterleaving;
  		unsigned fMaxdisplacement, fObjecttype;
  		unsigned fOctetalign, fProfile_level_id, fRobustsorting;
  		unsigned fSizelength, fStreamstateindication, fStreamtype;
  		bool fCpresent, fRandomaccessindication;
  		char *fConfig, *fMode, *fSpropParameterSets, *fEmphasis, *fChannelOrder;

	  	double fPlayStartTime;
  		double fPlayEndTime;
  		unsigned short fVideoWidth, fVideoHeight;
     	// screen dimensions (set by an optional a=x-dimensions: <w>,<h> line)
  		unsigned fVideoFPS;
     	// frame rate (set by an optional "a=framerate: <fps>" or "a=x-framerate: <fps>" line)
  		unsigned fNumChannels;
     	// optionally set by "a=rtpmap:" lines for audio sessions.  Default: 1
  		float fScale1; // set from a RTSP "Scale:" header
  		double fNPT_PTS_Offset; // set by "getNormalPlayTime()"; add this to a PTS to get NPT

	public:
		void setRtspSessionStat(bool stat){Alive=stat;}
		bool getRtspSessionStat(){return Alive;}

	private:
		bool Alive;
		private:
		char fCommandName[20];
		unsigned int fCseq;
		static char userAgent[];
		char sessionId[20]; //任务ID
		char mStreamName[32];
};
#endif
