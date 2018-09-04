#include "RTSPSession.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "RTSPClient.h"
#include "Engine.h"
#include <string.h>
#include "Until.h"
#include "RTPSession.h"
#include "RTCPSession.h"
#include "base64.h"
#include "HeartBeatSession.h"
#include "Log.h"
#include <time.h>

char RTSPSession::userAgent[]="simpleRTSP/0.0.1 (simpleRTSP Streaming Media v2016.07.10)";

RTSPSession::RTSPSession(RTSPClient *rtspClient,char *streamName):EventJob(rtspClient,new TCPSocket),
	Alive(true),fCseq(1),fSpropParameterSets(NULL),fMediumName(NULL),fCodecName(NULL),fProtocolName(NULL),
	fControlPath(NULL),fConnectionEndpointName(NULL),fConfig(NULL),fMode(NULL),
	fEmphasis(NULL),fChannelOrder(NULL),fSessionName(NULL),channel(0)
	
{

	memset(fResponseBuffer,0,sizeof(fResponseBuffer));
	memset(fResquestBuffer,0,sizeof(fResquestBuffer));
	responseBufferSize=20000;

	resetResponseBuffer();
	setSessionID();
	sprintf(mStreamName,"%s",streamName);

	client->addObjectToQueue(new HeartBeatSession(client),HeartBeatObjectType);
}
RTSPSession::~RTSPSession()
{
		fprintf_debug("delete RTSPSession\n");
		delete[] fProtocolName;
		delete[] fCodecName;
		delete[] fMediumName;
		#if 1
		delete[] fControlPath;
	    delete[] fConnectionEndpointName;
	    delete[] fConfig;
	    delete[] fMode;
	    delete[] fSpropParameterSets;
	    delete[] fEmphasis;
	    delete[] fChannelOrder;
	    delete[] fSessionName;
	    #endif

}
int RTSPSession::Processor()
{
	int outLen=0;
	int theErr=((TCPSocket *)getSocket())->read((void *)&fResponseBuffer[fResponseBytesAlreadySeen],(int)fResponseBufferBytesLeft,(unsigned int *)&outLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		fprintf_err("Not find message. theErr %d  errno  %d\n",theErr,errno);
		setRtspSessionStat(false);
		client->stopEngine();
		return 0;
	}
	fprintf_msg("recv message ---- >\n%s\n",fResponseBuffer);

	handlerResponse(outLen);
	return 1;
}

int  RTSPSession::Initialize(char *dstIp,unsigned short dstPort)
{
	if(dstIp==NULL || dstIp[0]=='\0'){
		return 0;
	}
	if(dstPort<0 || dstPort>65535){
		return 0;
	}

	getSocket()->setRemoteStrIp(dstIp);
	getSocket()->setRemotePort(dstPort);

	fprintf_msg("%s %d dstIp %s dstPort %d\n",__FUNCTION__,__LINE__,dstIp,dstPort);

	//getSocket()->makeSocketNonBlocking(getSocket()->getSocketFD());

	int theErr=((TCPSocket *)getSocket())->connect(getSocket()->strIpToUInt32(dstIp),dstPort);

	fprintf_debug("theErr %d\n",theErr);
	// Find out whether the connection succeeded or failed:
	#if 0 /*we test not block*/
	{
	    if(theErr!=0){
   	
	    	fd_set tWriteSet;
			fd_set tExceptionSet;
  			FD_ZERO(&tWriteSet);
  			FD_ZERO(&tExceptionSet);

			struct timeval timeToDelay;
  			timeToDelay.tv_sec=1;
  			timeToDelay.tv_usec=0;

			FD_SET((unsigned)getSocket()->getSocketFD(), &tWriteSet);
            FD_SET((unsigned)getSocket()->getSocketFD(), &tExceptionSet);
            int selectResult = select(getSocket()->getSocketFD(), NULL, &tWriteSet, &tExceptionSet, &timeToDelay);

	    	if(selectResult<0){
	    		fprintf_err("Connection to server failed selectResult %d errno %d\n",selectResult,errno);
				return 0;
	    	}
			int err = 0;
			SOCKLEN_T len = sizeof err;
    		if (getsockopt(getSocket()->getSocketFD(), SOL_SOCKET, SO_ERROR, (char*)&err, (socklen_t*)&len) < 0 || err != 0) {
     			fprintf_err("Connection to server failed err %d errno %d\n",err,errno);
     			return 0;
    		}
			fprintf_msg("Connect Succeed\n");
	    }	
	}
   	
	#endif
	getSocket()->setSendBufferTo(256*1024);
	getSocket()->setReceiveBufferTo(256*1024);
	#if 0
	const char *buf="hello world";
	unsigned int outLen=0;
	fSocket->send(buf,strlen(buf),&outLen);
	#endif
	fprintf_debug("Init RtspSession succeed\n");
	return 1;
}
int RTSPSession::handlerResponse(int rOutLen)
{
  int numBytesRemaining = 0;
  do {
	 if (rOutLen > 0 && (unsigned)rOutLen < fResponseBufferBytesLeft) break; // data was read OK; process it below

    if ((unsigned)rOutLen >= fResponseBufferBytesLeft) {
    }
    if (rOutLen <= 0) return 0;
    resetResponseBuffer();
    return 1;    
  } while (0);

  fResponseBufferBytesLeft -= rOutLen;
  fResponseBytesAlreadySeen += rOutLen;
  fResponseBuffer[fResponseBytesAlreadySeen] = '\0';
  
  bool endOfHeaders = false;
  if (fResponseBytesAlreadySeen > 3) {
     unsigned char * ptrEnd = &fResponseBuffer[fResponseBytesAlreadySeen-3];
     unsigned char * ptr = fResponseBuffer;
    while (ptr < ptrEnd) {
      if (*ptr++ == '\r' && *ptr++ == '\n' && *ptr++ == '\r' && *ptr++ == '\n') {
        endOfHeaders = true;
        break;
      }
    }
  }

  if (!endOfHeaders) return 1;
	fprintf_debug("Good Response\n");

  	char* headerDataCopy=NULL;
  	unsigned responseCode = 200;
  	char const* responseStr = NULL;
  	char const* sessionParamsStr = NULL;
  	char const* transportParamsStr = NULL;
  	char const* scaleParamsStr = NULL;
  	char const* rangeParamsStr = NULL;
  	char const* rtpInfoParamsStr = NULL;
  	char const* wwwAuthenticateParamsStr = NULL;
  	char const* publicParamsStr = NULL;
  	char const* channelNumStr=NULL;
  	char const* dateStr=NULL;
  	char* bodyStart = NULL;
  
  	unsigned numBodyBytes = 0;
  	bool responseSuccess = false;

  	do{
    	headerDataCopy = new char[responseBufferSize];
    	memcpy(headerDataCopy, (char *)fResponseBuffer, fResponseBytesAlreadySeen);
    	headerDataCopy[fResponseBytesAlreadySeen] = '\0';

    	char* lineStart = headerDataCopy;
    	char* nextLineStart = getLine(lineStart);
    	if (!parseResponseCode(lineStart, responseCode, responseStr)) {
      		break; 
    	}
    	bool reachedEndOfHeaders;
    	unsigned cseq = 0;
    	unsigned contentLength = 0;
		//fprintf(stderr,SIMPLE_RTSP"<---------------start parse-------------->\n");
    	while (1){
      		reachedEndOfHeaders = true;
      		lineStart = nextLineStart;
      		if (lineStart == NULL){
				break;
      		} 
      		nextLineStart = getLine(lineStart);
      		if (lineStart[0] == '\0') {
				break;
      		}
      		reachedEndOfHeaders = false;
		
      		char const* headerParamsStr; 
      		if (checkForHeader(lineStart, "CSeq:", 5, headerParamsStr)) {
        		if (sscanf(headerParamsStr, "%u", &cseq) != 1 || cseq <= 0) {
	 			break;
			}
			fprintf_debug("cseq %d\n",cseq);
      		}else if(checkForHeader(lineStart, "ChannelNum:", 11, channelNumStr)){
				fprintf_debug("ChannelNum:%s\n",channelNumStr);
      		}else if(checkForHeader(lineStart, "Date:", 5, dateStr)){
				fprintf_debug("Date:%s\n",dateStr);
      		}else if (checkForHeader(lineStart, "Content-Length:", 15, headerParamsStr)) {
        		if (sscanf(headerParamsStr, "%u", &contentLength) != 1) {
	  				fprintf_warn("parser Content-Length error\n");
	  			break;
			}
			fprintf_debug("Content-Length:%d\n",contentLength);
      		} else if (checkForHeader(lineStart, "Content-Base:", 13, headerParamsStr)) {
        		setBaseURL(headerParamsStr);
        		fprintf_debug("Content-Base:%s\n",headerParamsStr);
      		} else if (checkForHeader(lineStart, "Session:", 8, sessionParamsStr)) {
      			fprintf_debug("Session: %s\n",sessionParamsStr);
      		} else if (checkForHeader(lineStart, "Transport:", 10, transportParamsStr)) {
      			fprintf_debug("Transport: %s\n",transportParamsStr);
      		} else if (checkForHeader(lineStart, "Scale:", 6, scaleParamsStr)) {
      			fprintf_debug("Scale:%s\n",scaleParamsStr);
      		} else if (checkForHeader(lineStart, "Range:", 6, rangeParamsStr)) {
      			fprintf_debug("Range: %s\n",rangeParamsStr);
      		} else if (checkForHeader(lineStart, "RTP-Info:", 9, rtpInfoParamsStr)) {
      			fprintf_debug("RTP-Info: %s\n",rtpInfoParamsStr);
      		} else if (checkForHeader(lineStart, "WWW-Authenticate:", 17, headerParamsStr)) {
				if (wwwAuthenticateParamsStr == NULL || _strncasecmp(headerParamsStr, "Digest", 6) == 0) {
	 	 		wwwAuthenticateParamsStr = headerParamsStr;
			}
				fprintf_debug("WWW-Authenticate:%s\n",headerParamsStr);
      		} else if (checkForHeader(lineStart, "Public:", 7, publicParamsStr)) {
      			fprintf_debug("Public:%s\n",publicParamsStr);
      		} else if (checkForHeader(lineStart, "Allow:", 6, publicParamsStr)) {
      			fprintf_debug("Allow:%s\n",publicParamsStr);
      		} else if (checkForHeader(lineStart, "Location:", 9, headerParamsStr)) {
           		setBaseURL(headerParamsStr);
				fprintf_debug("Location:%s\n",headerParamsStr);
      		}
    	}
    	fprintf_debug("<-----------------end parse----------------->\n");
    	if (!reachedEndOfHeaders) {
    		break;
    	}
    	fprintf_debug("<---------------parse succeed-------------->\n");
    	// If we saw a "Content-Length:" header, then make sure that we have the amount of data that it specified:
    	unsigned bodyOffset = (unsigned )(nextLineStart - headerDataCopy);
    	fprintf_debug("bodyOffset %u\n",bodyOffset);
    	bodyStart = (char *)&fResponseBuffer[bodyOffset];
    	fprintf_debug("Content %s\n",bodyStart);
    	numBodyBytes = fResponseBytesAlreadySeen - bodyOffset;
    	fprintf_debug("numBodyBytes %d\n",numBodyBytes);
    	if (contentLength > numBodyBytes) {
      		unsigned numExtraBytesNeeded = contentLength - numBodyBytes;
      		unsigned remainingBufferSize = responseBufferSize - fResponseBytesAlreadySeen;
      		if (numExtraBytesNeeded > remainingBufferSize) {
        		char tmpBuf[200];
				sprintf(tmpBuf, "Response buffer size (%d) is too small for \"Content-Length:\" %d (need a buffer size of >= %d bytes\n",
               	responseBufferSize, contentLength, fResponseBytesAlreadySeen + numExtraBytesNeeded);
        		break;
      		}
      	delete[] headerDataCopy;
      	return 1;
    	}
    	if(bodyStart!=NULL){
				fprintf_debug("body %s\n",bodyStart);
    	}
		fprintf_debug("responseCode %d\n",responseCode);
      	bool needToResendCommand = false;
      	if (responseCode == 200) {
    		if(strcmp(commandName(), "OPTIONS") == 0){
    			handleOPTIONSResponse();
    		}else if (strcmp(commandName(), "DESCRIBE") == 0){
    			handleDESCRIBEResponse(channel,contentLength,bodyStart);
    		}else if (strcmp(commandName(), "SETUP") == 0) {
    			handleSETUPResponse(channel,(char *)transportParamsStr,(char *)sessionParamsStr);
			}else if (strcmp(commandName(), "PLAY") == 0){
				handlePLAYResponse(channel,(char *)rtpInfoParamsStr);
			}else if (strcmp(commandName(), "TEARDOWN") == 0){
				delete[] headerDataCopy;	
				client->stopEngine();;
				return 1;
			}else if (strcmp(commandName(), "GET_PARAMETER") == 0){
				/*do nothing*/
			}
      }
    #if 0 //错误处理代码，跳过
      else if (responseCode == 401) {
	// We need to resend the command, with an "Authorization:" header:
	needToResendCommand = True;

	if (strcmp(foundRequest->commandName(), "GET") == 0) {
	  // Note: If a HTTP "GET" command (for RTSP-over-HTTP tunneling) returns "401 Unauthorized", then we resend it
	  // (with an "Authorization:" header), just as we would for a RTSP command.  However, we do so using a new TCP connection,
	  // because some servers close the original connection after returning the "401 Unauthorized".
	  resetTCPSockets(); // forces the opening of a new connection for the resent command
	}
      } else if (responseCode == 301 || responseCode == 302) { // redirection
	resetTCPSockets(); // because we need to connect somewhere else next
	needToResendCommand = True;
      }
	#endif
	#if 0 //重新发送命令，跳过
      if (needToResendCommand) {
	resetResponseBuffer();
	if (!resendCommand(foundRequest)) break;
	delete[] headerDataCopy;
	return; // without calling our response handler; the response to the resent command will do that
      }
    #endif
	delete[] headerDataCopy;

    responseSuccess = true;
  }while(0);
	
 fprintf_debug("Reset fResponseBuffer\n");
 resetResponseBuffer(); // in preparation for our next response.  Do this now, in case the handler function goes to the event loop.

}
//重置请求缓冲
void RTSPSession::resetResponseBuffer()
{
  memset(fResponseBuffer,0,RESPONSE_BUFFER_SIZE);
  fResponseBytesAlreadySeen = 0;
  fResponseBufferBytesLeft = responseBufferSize;
}
char* RTSPSession::getLine(char* startOfLine)
{
  for (char* ptr = startOfLine; *ptr != '\0'; ++ptr){
    if (*ptr == '\r' || *ptr == '\n') {
      *ptr++ = '\0';
      if (*ptr == '\n') ++ptr;
      return ptr;
    }
  }
  return NULL;
}
bool RTSPSession::checkForHeader(char const* line, char const* headerName, unsigned headerNameLength, char const*& headerParams)
{
  if (_strncasecmp(line, headerName, headerNameLength) != 0) return false;

  unsigned paramIndex = headerNameLength;
  while (line[paramIndex] != '\0' && (line[paramIndex] == ' ' || line[paramIndex] == '\t')) ++paramIndex;
  if (&line[paramIndex] == '\0') return false; // the header is assumed to be bad if it has no parameters

  headerParams = &line[paramIndex];
  return true;
}
void RTSPSession::setBaseURL(char const* url) 
{
  fBaseURL = strDup(url);
}

bool RTSPSession::parseResponseCode(char const* line, unsigned& responseCode, char const*& responseString)
{
  if (sscanf(line, "RTSP/%*s%u", &responseCode) != 1 &&
      sscanf(line, "HTTP/%*s%u", &responseCode) != 1) return false;
  responseString = line;
  while (responseString[0] != '\0' && responseString[0] != ' '  && responseString[0] != '\t') ++responseString;
  while (responseString[0] != '\0' && (responseString[0] == ' '  || responseString[0] == '\t')) ++responseString; // skip whitespace

  return true;
}
int RTSPSession::setCommandName(const char *cName)
{
	//重置命令
	resetCommandName();
	if(cName==NULL || cName[0]=='\0' || strlen(cName)>sizeof(fCommandName)){
		return 0;
	}
	memcpy(fCommandName,cName,strlen(cName));
}
int RTSPSession::handleOPTIONSResponse()
{
	/*do nothing*/
	/*发送 DESCRIBE 命令*/
	sendDESCRIBECommand();
	return 1;
}

int RTSPSession::handleDESCRIBEResponse(int channelNum,int contentLen,char *contentBuf)
{
	if(contentLen<=0 || contentBuf==NULL){
		return false;
	}
	if(channelNum<0){
		return false;
	}

  //解析content内容
  //默认只解析一次，即只有一个 "m="(只支持处理H264视频)
  char const* sdpLine = contentBuf;
  char const* nextSDPLine;
  while (1) {
    if (!parseSDPLine(sdpLine, nextSDPLine)) return false;
    if (sdpLine[0] == 'm') break;
    sdpLine = nextSDPLine;
    if (sdpLine == NULL) break; 

    if (parseSDPLine_s(sdpLine)) continue;
    if (parseSDPLine_i(sdpLine)) continue;
    if (parseSDPLine_c(sdpLine)) continue;
    if (parseSDPAttribute_control(sdpLine)) continue;
    if (parseSDPAttribute_range(sdpLine)) continue;
    if (parseSDPAttribute_type(sdpLine)) continue;
  //  if (parseSDPAttribute_source_filter(sdpLine)) continue; 
  }
  char* mediumName=NULL;
   while (sdpLine != NULL){
     delete[] mediumName;
     mediumName= strDupSize(sdpLine);
    char const* protocolName = NULL;
    unsigned payloadFormat;
    if ((sscanf(sdpLine, "m=%s %hu RTP/AVP %u",
		mediumName, &fClientPortNum, &payloadFormat) == 3 ||
	 sscanf(sdpLine, "m=%s %hu/%*u RTP/AVP %u",
		mediumName, &fClientPortNum, &payloadFormat) == 3)
	&& payloadFormat <= 127) {
      protocolName = "RTP";
    } else if ((sscanf(sdpLine, "m=%s %hu UDP %u",
		       mediumName, &fClientPortNum, &payloadFormat) == 3 ||
		sscanf(sdpLine, "m=%s %hu udp %u",
		       mediumName, &fClientPortNum, &payloadFormat) == 3 ||
		sscanf(sdpLine, "m=%s %hu RAW/RAW/UDP %u",
		       mediumName, &fClientPortNum, &payloadFormat) == 3)
	       && payloadFormat <= 127) {
      protocolName = "UDP";
    } else {
      char* sdpLineStr;
      if (nextSDPLine == NULL) {
	sdpLineStr = (char*)sdpLine;
      } else {
	sdpLineStr = strDup(sdpLine);
	sdpLineStr[nextSDPLine-sdpLine] = '\0';
      }
      if (sdpLineStr != (char*)sdpLine) delete[] sdpLineStr;

   while (1) {
	sdpLine = nextSDPLine;
	if (sdpLine == NULL) break;
	if (!parseSDPLine(sdpLine, nextSDPLine)) return false;

	if (sdpLine[0] == 'm') break; 
      }
      continue;
    }
    
    fRTPPayloadFormat=payloadFormat;
    delete[] fMediumName;
    fMediumName=strDup(mediumName);
    delete[] mediumName;
    delete[] fProtocolName;
    fProtocolName=strDup(protocolName);

	fprintf_debug("<<<<<<<< fMediuName %s >>>>> <<<<<<<fCodecName %s>>>>>>>> <<<<< fProtocolName %s>>>>>\n",fMediumName,
					fCodecName,
					fProtocolName);
    while (1) {
      sdpLine = nextSDPLine;
      if (sdpLine == NULL) break; 
      if (!parseSDPLine(sdpLine, nextSDPLine)) return false;

      if (sdpLine[0] == 'm') break; 

      if (parseSDPLine_c(sdpLine)) continue;
      if (parseSDPLine_b(sdpLine)) continue;
      if (parseSDPAttribute_rtpmap(sdpLine)) continue;
      if (parseSDPAttribute_control(sdpLine)) continue;
      if (parseSDPAttribute_range(sdpLine)) continue;
      if (parseSDPAttribute_fmtp(sdpLine)) continue;
     // if (parseSDPAttribute_source_filter(sdpLine)) continue;
      if (parseSDPAttribute_x_dimensions(sdpLine)) continue;
      if (parseSDPAttribute_framerate(sdpLine)) continue;
    }

    /*wo should check it*/
    if(fMediumName!=NULL 
    	&& fCodecName!=NULL
    	&& fProtocolName!=NULL){

		/*Print fMediumName fCodecName fProtocolName Content*/
		fprintf_debug("@@@@@@@@@@@@@ fMediumName %s fCodecName %s fProtocolName %s\n",
				fMediumName,
				fCodecName,
				fProtocolName);
	/*we are interesting in H264*/
	if((strcmp(fMediumName,"video")==0 	|| strcmp(fMediumName,"Video")==0) 
    	&&(strcmp(fCodecName,"H264")==0 || strcmp(fCodecName,"h264")==0)
    	&&(strcmp(fProtocolName,"RTP"))==0){
		#if 1
		RTPSession *session=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));
		fprintf_debug("RTPSession %p\n",session);
		if(session==NULL){
			fprintf_debug("create new RTPSession\n");
			session=new RTPSession(client);
			client->addObjectToQueue(session,RTPSessionObjectType);
		}
		RTCPSession *session1=(RTCPSession *)(client->getObjectFromQueue(RTCPSessionObjectType));
		fprintf_debug("RTCPSession %p\n",session1);

		if(session1==NULL){
			fprintf_debug("Create new RTCPSession\n");
			session1=new RTCPSession(client);
			client->addObjectToQueue(session1,RTCPSessionObjectType);
		}
	
		#endif
		break; /*we find ,so break*/
    }else{
		/*do nothings*/
    }
        }else{
			fprintf_warn("fMediumName OR fCodecName OR fProtocolName is NULL\n");
        }
  }
	#if 1
	do{
		RTPSession *session=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));
		if(session==NULL){
			fprintf_err("%s %d Not Find RTPSession Object \n",__FUNCTION__,__LINE__);
			return 0;
		}
		if(fSpropParameterSets==NULL){
			fprintf_err("%s %d Not Find SPS PPS Info\n",__FUNCTION__,__LINE__);
			break;
		}
	
		/*从fSpropParameterSets参数中提取sps pps信息*/
		unsigned char tSpsBuf[256]={'\0'};
		unsigned int tSpsLen=0;
		unsigned char tPpsBuf[256]={'\0'};
		unsigned int tPpsLen=0;

		if(sscanf(fSpropParameterSets,"%[^,],%s",(char *)tSpsBuf,(char *)tPpsBuf)!=2){
			fprintf_warn("%s %d Can't Parser SPS PPS Info\n",__FUNCTION__,__LINE__);
			break;
		}

		#if 0
		fprintf(stderr,SIMPLE_RTSP"sps %s spsLen %d \n pps %s ppsLen %d\n",
					(char *)tSpsBuf,
					strlen((char *)tSpsBuf),
					(char *)tPpsBuf,
					strlen((char *)tPpsBuf));

		#endif

		unsigned char dSpsBuf[256]={'\0'};
		dSpsBuf[0]=0;dSpsBuf[1]=0;
		dSpsBuf[2]=0;dSpsBuf[3]=1;

		unsigned char dPpsBuf[256]={'\0'};
		dPpsBuf[0]=0;dPpsBuf[1]=0;
		dPpsBuf[2]=0;dPpsBuf[3]=1;

		int dstSpsLen=Base64decode((char *)&dSpsBuf[4],(char *)tSpsBuf);
		int dstPpsLen=Base64decode((char *)&dPpsBuf[4],(char *)tPpsBuf);

		dstSpsLen+=4;
		dstPpsLen+=4;

		session->setSps(dSpsBuf,dstSpsLen);
		session->setPps(dPpsBuf,dstPpsLen);

		#ifdef BUG /*test sps pps Len and content OK*/
		fprintf(stderr,"SPSLen %d PPSLen %d\n",
					session->getSpsLen(),
					session->getPpsLen()
					);
		unsigned int tmpSpsLen=session->getSpsLen();
		unsigned char *tmpSpsBuf=session->getSps();
		fprintf(stderr,"\n SPS Start ------------------> \n");
		for(int i=0;i<tmpSpsLen;++i){
			fprintf(stderr,"%02X ",tmpSpsBuf[i]);
		}
	
		fprintf(stderr,"\n PPS Start ---------------------> \n");
		unsigned int tmpPpsLen=session->getPpsLen();
		unsigned char *tmpPpsBuf=session->getPps();
		for(int i=0;i<tmpPpsLen;++i){
			fprintf(stderr,"%02X ",tmpPpsBuf[i]);
		}
		fprintf(stderr,"\n");
		
		#endif
	
		#if 1
		fprintf_debug("SPSLen PPSLen---------- > %d %d \n",
					dstSpsLen,
					dstPpsLen
					);
		#endif
	#endif
  }while(0);
#if 0
  
  fprintf_debug("\nParse SDP Result --->\n"
  "fSessionName	[%s] \n"
  "fSessionDescription	[%s] \n"
  "fConnectionEndpointName [%s] \n"
  "fControlPath [%s] \n"
  "fMaxPlayStartTime [%lf] \n"
  "fMaxPlayEndTime [%lf] \n"
  "fMediaSessionType [%s] \n"
  "fBandwidth [%d]\n"
  "fCodecName [%s]\n"
  "fRTPTimestampFrequency [%d] \n"
  "fNumChannels [%d]\n"
  "fVideoWidth [%d]\n"
  "fVideoHeight [%d]\n"
  "fVideoFPS [%d]\n"
  "fClientPortNum [%d]\n"
  "fRTPPayloadFormat [%d]\n"
  "fProfile_level_id [%d]\n"
  "fSpropParameterSets [%s]\n"
  "fProtocolName [%s]\n"
  "fMediumName [%s]\n\n\n",
  fSessionName,
  fSessionDescription,
  fConnectionEndpointName,
  fControlPath,
  fMaxPlayStartTime,
  fMaxPlayEndTime,
  fMediaSessionType,
  fBandwidth,
  fCodecName,
  fRTPTimestampFrequency,
  fNumChannels,
  fVideoWidth,
  fVideoHeight,
  fVideoFPS,
  fClientPortNum,
  fRTPPayloadFormat,
  fProfile_level_id,
  fSpropParameterSets,
  fProtocolName,
  fMediumName);
#endif
#if 1

  sendSETUPCommand();
#endif
}

int RTSPSession::handleSETUPResponse(int channelNum/*有必要?*/,char *transportStr,char *sessionStr)
{
    char* serverAddressStr=NULL;
    unsigned short serverPortNum,clientPortNum;
    unsigned char rtpChannelId, rtcpChannelId;
    bool socketType;
    char tSessionId[32]={0};
	if(sscanf(sessionStr, "%[^;]", tSessionId) != 1){ //live555 0.8版本以上Session:之后还可能会跟一个 timeout= 字段
		sprintf(tSessionId,"%s",sessionStr);
	}
    fprintf_debug("channel %d Transport %s Session %s\n",channelNum,transportStr,tSessionId);	

    if (!parseTransportParams(transportStr, 	
    							serverAddressStr,
    							serverPortNum,
    							clientPortNum, 
    							rtpChannelId, 
    							rtcpChannelId)){
		return 0;
    }
    fprintf_debug("server Port %d client Port %d\n",serverPortNum,clientPortNum);

	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));
	Socket*socket0=session0->getSocket();
	
	socket0->setRemotePort(serverPortNum);
	
	((Engine *)(client->getObjectFromQueue(EngineObjectType)))->addEvent(session0,SOCKET_READABLE);

	session0->setSessionId(tSessionId);

	RTCPSession *session1=(RTCPSession *)(client->getObjectFromQueue(RTCPSessionObjectType));
	Socket*socket1=session0->getSocket();
	
	socket1->setRemotePort(serverPortNum+1);

	/*RTCP任务暂时未加入到事件队列中*/
	//((Engine *)(client->getObjectFromQueue(EngineObjectType)))->addEvent(session1,SOCKET_READABLE);

	session0->setSessionId(tSessionId);
	#if 0 //暂未使用rtcp
	RTCPSession *session1=(RTCPSession *)(client->getObjectFromQueue(RTCPSessionObjectType));
	Socket*socket1=session1->getSocket();
	#endif
	sendPLAYCommand();
}

int RTSPSession::handlePLAYResponse(int channelNum,char *infoStr)
{
	/*do nothing*/
	((Engine *)client->getObjectFromQueue(EngineObjectType))->
				addElemToTimeoutQueue(client->getObjectFromQueue(HeartBeatObjectType));
	return 0;
}


int RTSPSession::resetCommandName()
{
	memset(fCommandName,0,sizeof(fCommandName));
	return 1;
}
int RTSPSession::sendOPTIONSCommand()
{	
	memset((char *)fResquestBuffer,'\0',RESPONSE_BUFFER_SIZE);
	static char OPTIONSCommand[]="OPTIONS";

	#if 1
	setCommandName(OPTIONSCommand);
	sprintf((char *)fResquestBuffer, 
				"OPTIONS rtsp://%s:%u/%s RTSP/1.0\r\n"
				"CSeq:%u\r\n"
				"User-agent: %s\r\n\r\n",
				getSocket()->getRemoteStrIp(),
				getSocket()->getRemotePort(),
				mStreamName,
				fCseq++,
				userAgent);

	fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		setRtspSessionStat(false);
		return 0;
	}
	#endif

	   return 1;

}
int RTSPSession::sendDESCRIBECommand()
{
	static char accept[]="application/sdp";
	static char DESCRIBECommand[]="DESCRIBE";

	
	memset(fResquestBuffer,0,REQUEST_BUFFER_SIZE);

	setCommandName(DESCRIBECommand);
	sprintf((char *)fResquestBuffer, 
			"DESCRIBE rtsp://%s:%u/%s RTSP/1.0\r\n"
			"CSeq:%u\r\n"
			"User-agent: %s\r\n"
			"Accept:%s\r\n\r\n",
			getSocket()->getRemoteStrIp(),
			getSocket()->getRemotePort(),
			mStreamName,
			fCseq++,
			userAgent,
			accept);
	fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);
	#if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		setRtspSessionStat(false);
		return 0;
	}
	#endif

	   return 1;


}
int RTSPSession::sendSETUPCommand()
{
	fprintf_debug("SendSETUPCommand \n");
	memset(fResquestBuffer,'\0',REQUEST_BUFFER_SIZE);
	static char SETUPCommand[]="SETUP";

	setCommandName(SETUPCommand);
	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));
	Socket*socket0=session0->getSocket();
	RTCPSession *session1=(RTCPSession *)(client->getObjectFromQueue(RTCPSessionObjectType));
	Socket*socket1=session1->getSocket();

	bool bindFlags=false;
	
	static unsigned short tPort=20000;
	#if 1
	for(;tPort<65535;++tPort){
		#if 0
		if((socket0->bind(socket0->strIpToUInt32(socket0->getLocalStrIp()),tPort))==1 
		&& (socket1->bind(socket1->strIpToUInt32(socket1->getLocalStrIp()),tPort+1))==1)
#endif
	
		if((socket0->bind(INADDR_ANY,tPort))==1 
		&& (socket1->bind(INADDR_ANY,tPort+1))==1)
		{

			socket0->setLocalPort(tPort);
			socket1->setLocalPort(tPort+1);
			bindFlags=true;
			break;	
		}	
	}
	#endif
	#if 0
	bindFlags=true;
	socket0->setLocalPort(tPort);
	socket1->setLocalPort(tPort+1);
	#endif
	socket0->setSendBufferTo(256*1024);
	socket0->setReceiveBufferTo(256*1024);

	//socket1->setSendBufferTo(256*1024);
	//socket1->setReceiveBufferTo(256*1024);
	
	if(!bindFlags){
		fprintf_err("something wrong while bind socket\n");
		return 0;
	}
	#if 0
		SETUP rtsp://192.168.0.168:554/live1.264/track1 RTSP/1.0
		CSeq: 4
		User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
		Transport: RTP/AVP;unicast;client_port=65494-65495
	#endif
	
	sprintf((char *)fResquestBuffer, 
		"SETUP rtsp://%s:%u/%s/track1 RTSP/1.0\r\n"
		"CSeq:%u\r\n"
		"User-Agent: %s\r\n"
		"Transport: RTP/AVP;unicast;client_port=%d-%d\r\n\r\n",
		getSocket()->getRemoteStrIp(),
		getSocket()->getRemotePort(),
		mStreamName,
		fCseq++,
		userAgent,
		socket0->getLocalPort(),
		socket1->getLocalPort());
	#if 0
	for(int i=0;i<strlen((char *)fResquestBuffer);++i){
      	if(i%16==0){
			fprintf_debug("");
      	}
		fprintf_debug("%d-%c ",i,fResquestBuffer[i]);

      }
   #endif
   	fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

   #if 0
	SETUP rtsp://192.168.0.168:554/live0.264/track1 RTSP/1.0
	CSeq: 4
	User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
	Transport: RTP/AVP;unicast;client_port=59402-59403
   #endif

   #if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(theErr!=EAGAIN){
		setRtspSessionStat(false);
		return 0;
	}
	#endif

	   return 1;

}
int RTSPSession::sendPLAYCommand()
{
	static char PLAYCommand[]="PLAY";

	memset(fResquestBuffer,'\0',REQUEST_BUFFER_SIZE);

	setCommandName(PLAYCommand);
   #if 0
	PLAY rtsp://192.168.0.168:554/live0.264 RTSP/1.0
	CSeq: 5
	User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
	Session: 7A12AEBE
	Range: npt=0.000-
   #endif
   	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));

   sprintf((char *)fResquestBuffer,
   		"PLAY rtsp://%s:%u/%s/track1 RTSP/1.0\r\n"
   		"CSeq:%u\r\n"
   		"User-Agent: %s\r\n"
   		"Session:%s\r\n" 
   		"Range: npt=0.000-\r\n\r\n",
		getSocket()->getRemoteStrIp(),
		getSocket()->getRemotePort(),
	   	mStreamName,

		fCseq++,
		userAgent,
		session0->getSessionId()
   		);	

   	fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

   #if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		setRtspSessionStat(false);
		return 0;
	}
	#endif
	return 1;

}
int RTSPSession::sendTEARDOWNCommand()
{
	static char TEARDOWNCommand[]="TEARDOWN";

	setCommandName(TEARDOWNCommand);
	
	memset(fResquestBuffer,'\0',REQUEST_BUFFER_SIZE);

	setCommandName(TEARDOWNCommand);

   	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));

    sprintf((char *)fResquestBuffer,
   			"TEARDOWN rtsp://%s:%u/%s/track1 RTSP/1.0\r\n"
   			"CSeq:%u\r\n"
   			"User-Agent: %s\r\n"
   			"Session:%s\r\n\r\n", 
			getSocket()->getRemoteStrIp(),
			getSocket()->getRemotePort(),
			mStreamName,
			fCseq++,
			userAgent,
			session0->getSessionId()
   			);	
   #if 0
	TEARDOWN rtsp://192.168.0.168:554/live0.264 RTSP/1.0
	CSeq: 7
	User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
	Session: 7A12AEBE
   #endif
		fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

   #if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		setRtspSessionStat(false);
		
		return 0;
	}
   #endif

   return 1;
}
#if 0
int RTSPSession::sendPAUSECommand()
{
	static char TEARDOWNCommand[]="PAUSE";

	setCommandName(TEARDOWNCommand);
	
	memset(fResquestBuffer,'\0',REQUEST_BUFFER_SIZE);

	setCommandName(TEARDOWNCommand);

   	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));

    sprintf((char *)fResquestBuffer,
   			"PAUSE rtsp://%s:%u/%s/track1 RTSP/1.0\r\n"
   			"CSeq:%u\r\n"
   			"User-Agent: %s\r\n"
   			"Session:%s\r\n\r\n", 
			getSocket()->getRemoteStrIp(),
			getSocket()->getRemotePort(),
			mStreamName,
			fCseq++,
			userAgent,
			session0->getSessionId()
   			);	
   #if 0
	TEARDOWN rtsp://192.168.0.168:554/live0.264 RTSP/1.0
	CSeq: 7
	User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
	Session: 7A12AEBE
   #endif
		fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

   #if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(theErr!=EAGAIN){
		setRtspSessionStat(false);
		return 0;
	}
   #endif
}
#endif
int RTSPSession::sendGET_PARAMETERCommand()
{
	static char GET_PARAMETERCommand[]="GET_PARAMETER";

   #if 0
	GET_PARAMETER rtsp://192.168.0.168:554/live0.264 RTSP/1.0
	CSeq: 6
	User-Agent: LibVLC/2.2.1 (LIVE555 Streaming Media v2014.07.25)
	Session: 7A12AEBE

   #endif
	memset(fResquestBuffer,'\0',REQUEST_BUFFER_SIZE);
	setCommandName(GET_PARAMETERCommand);

   	RTPSession *session0=(RTPSession *)(client->getObjectFromQueue(RTPSessionObjectType));
	
	sprintf((char *)fResquestBuffer,
   			"GET_PARAMETER rtsp://%s:%u/%s/track1 RTSP/1.0\r\n"
   			"CSeq:%u\r\n"
   			"User-Agent: %s\r\n"
   			"Session:%s\r\n\r\n", 
			getSocket()->getRemoteStrIp(),
			getSocket()->getRemotePort(),
			mStreamName,
			fCseq++,
			userAgent,
			session0->getSessionId()
   			);	
   	fprintf_msg("send message ---- > \n%s\n",fResquestBuffer);

   #if 1
	int sendLen=0;
	int theErr=((TCPSocket *)getSocket())->send((char *)fResquestBuffer,strlen((char *)fResquestBuffer),(unsigned int *)&sendLen);
	if(!((TCPSocket *)getSocket())->isConnected()){
		setRtspSessionStat(false);
		return 0;
	}
   #endif
}
unsigned int RTSPSession::getRandom32()
{
	state = ((0x5DEECE66DULL*state) + 0xBULL)&0x0000ffffffffffffULL;
	unsigned int x = (unsigned int)((state>>16)&0xffffffffULL);
	return x;
}
void RTSPSession::setSessionID()
{
    unsigned int UIntSessionId = getRandom32();
    sprintf(sessionId, "%08X", UIntSessionId);
}
char *RTSPSession::getSessionID()
{
	return sessionId;
}
bool RTSPSession::parseSDPLine(char const* inputLine,char const*& nextLine)
{
  nextLine = NULL;
  for (char const* ptr = inputLine; *ptr != '\0'; ++ptr) {
    if (*ptr == '\r' || *ptr == '\n') {
      ++ptr;
      while (*ptr == '\r' || *ptr == '\n') ++ptr;
      nextLine = ptr;
      if (nextLine[0] == '\0') nextLine = NULL; 
      break;
    }
  }

  if (inputLine[0] == '\r' || inputLine[0] == '\n') return true;
  if (strlen(inputLine) < 2 || inputLine[1] != '='
      || inputLine[0] < 'a' || inputLine[0] > 'z') {
    return false;
  }

  return true;
}
bool RTSPSession::parseSDPLine_s(char const* sdpLine) 
{
  char* buffer = strDupSize(sdpLine);
  bool parseSuccess = false;

  if (sscanf(sdpLine, "s=%[^\r\n]", buffer) == 1) {
    delete[] fSessionName; fSessionName = strDup(buffer);
    parseSuccess = true;
  }
  delete[] buffer;

  return parseSuccess;
}
bool RTSPSession::parseSDPLine_i(char const* sdpLine)
{
  char* buffer = strDupSize(sdpLine);
  bool parseSuccess = false;

  if (sscanf(sdpLine, "i=%[^\r\n]", buffer) == 1) {
    delete[] fSessionDescription; fSessionDescription = strDup(buffer);
    parseSuccess = true;
  }
  delete[] buffer;

  return parseSuccess;
}
bool RTSPSession::parseSDPLine_c(char const* sdpLine) 
{
  char* connectionEndpointName = parseCLine(sdpLine);
  if (connectionEndpointName != NULL) {
    	delete[] fConnectionEndpointName;
    	fConnectionEndpointName = connectionEndpointName;
    	return true;
  }
  return false;
}
char* RTSPSession::parseCLine(char const* sdpLine) 
{
  char* resultStr = NULL;
  char* buffer = strDupSize(sdpLine);
  if (sscanf(sdpLine, "c=IN IP4 %[^/\r\n]", buffer) == 1) {
    resultStr = strDup(buffer);
  }
  delete[] buffer;

  return resultStr;
}
bool RTSPSession::parseSDPAttribute_control(char const* sdpLine)
{
  bool parseSuccess = false;

  char* controlPath = strDupSize(sdpLine);
  if (sscanf(sdpLine, "a=control: %s", controlPath) == 1) {
    parseSuccess = true;
    delete[] fControlPath; fControlPath = strDup(controlPath);
  }
  delete[] controlPath;

  return parseSuccess;
}

bool RTSPSession::parseSDPAttribute_range(char const* sdpLine)
{
  bool parseSuccess = false;

  double playStartTime;
  double playEndTime;
  if (parseRangeAttribute(sdpLine, playStartTime, playEndTime)) {
    parseSuccess = true;
    if (playStartTime > fMaxPlayStartTime) {
      fMaxPlayStartTime = playStartTime;
    }
    if (playEndTime > fMaxPlayEndTime) {
      fMaxPlayEndTime = playEndTime;
    }
  }

  return parseSuccess;
}
bool RTSPSession::parseRangeAttribute(char const* sdpLine, double& startTime, double& endTime) 
{
  return sscanf(sdpLine, "a=range: npt = %lg - %lg", &startTime, &endTime) == 2;
}
bool RTSPSession::parseSDPAttribute_type(char const* sdpLine) 
{
  bool parseSuccess = false;

  char* buffer = strDupSize(sdpLine);
  if (sscanf(sdpLine, "a=type: %[^ ]", buffer) == 1) {
    delete[] fMediaSessionType;
    fMediaSessionType = strDup(buffer);
    parseSuccess = true;
  }
  delete[] buffer;

  return parseSuccess;
}

bool RTSPSession::parseSDPLine_c1(char const* sdpLine) 
{
  char* connectionEndpointName = parseCLine(sdpLine);
  if (connectionEndpointName != NULL) {
    delete[] fConnectionEndpointName1;
    fConnectionEndpointName1 = connectionEndpointName;
    return true;
  }

  return false;
}
bool RTSPSession::parseSDPLine_b(char const* sdpLine)
{
  return sscanf(sdpLine, "b=AS:%u", &fBandwidth) == 1;
}
bool RTSPSession::parseSDPAttribute_rtpmap(char const* sdpLine) 
{
  bool parseSuccess = false;

  unsigned rtpmapPayloadFormat;
  char* codecName = strDupSize(sdpLine);
  unsigned rtpTimestampFrequency = 0;
  unsigned numChannels = 1;
  if (sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u/%u",
	     &rtpmapPayloadFormat, codecName, &rtpTimestampFrequency,
	     &numChannels) == 4
      || sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u",
	     &rtpmapPayloadFormat, codecName, &rtpTimestampFrequency) == 3
      || sscanf(sdpLine, "a=rtpmap: %u %s",
		&rtpmapPayloadFormat, codecName) == 2) {
    parseSuccess = true;
    if (rtpmapPayloadFormat == fRTPPayloadFormat) {
      {
	for (char* p = codecName; *p != '\0'; ++p) *p = toupper(*p);
      }
      delete[] fCodecName; fCodecName = strDup(codecName);
      fRTPTimestampFrequency = rtpTimestampFrequency;
      fNumChannels = numChannels;
    }
  }
    delete[] codecName;

  return parseSuccess;
}
bool RTSPSession::parseSDPAttribute_control1(char const* sdpLine)
{
  bool parseSuccess = false;

  char* controlPath = strDupSize(sdpLine);
  if (sscanf(sdpLine, "a=control: %s", controlPath) == 1) {
    parseSuccess = true;
    delete[] fControlPath1; fControlPath1 = strDup(controlPath);
  }
  delete[] controlPath;

  return parseSuccess;
}
bool RTSPSession::parseSDPAttribute_range1(char const* sdpLine)
{
  return true;
}
bool RTSPSession::parseSDPAttribute_fmtp(char const* sdpLine)
{
  do {
    if (strncmp(sdpLine, "a=fmtp:", 7) != 0) break; sdpLine += 7;
    while (isdigit(*sdpLine)) ++sdpLine;

    char* const lineCopy = strDup(sdpLine); char* line = lineCopy;
    {
      for (char* c = line; *c != '\0'; ++c) *c = tolower(*c);
    }
    while (*line != '\0' && *line != '\r' && *line != '\n') {
      unsigned u;
      char* valueStr = strDupSize(line);
      if (sscanf(line, " auxiliarydatasizelength = %u", &u) == 1) {
	fAuxiliarydatasizelength = u;
      } else if (sscanf(line, " constantduration = %u", &u) == 1) {
	fConstantduration = u;
      } else if (sscanf(line, " constantsize; = %u", &u) == 1) {
	fConstantsize = u;
      } else if (sscanf(line, " crc = %u", &u) == 1) {
	fCRC = u;
      } else if (sscanf(line, " ctsdeltalength = %u", &u) == 1) {
	fCtsdeltalength = u;
      } else if (sscanf(line, " de-interleavebuffersize = %u", &u) == 1) {
	fDe_interleavebuffersize = u;
      } else if (sscanf(line, " dtsdeltalength = %u", &u) == 1) {
	fDtsdeltalength = u;
      } else if (sscanf(line, " indexdeltalength = %u", &u) == 1) {
	fIndexdeltalength = u;
      } else if (sscanf(line, " indexlength = %u", &u) == 1) {
	fIndexlength = u;
      } else if (sscanf(line, " interleaving = %u", &u) == 1) {
	fInterleaving = u;
      } else if (sscanf(line, " maxdisplacement = %u", &u) == 1) {
	fMaxdisplacement = u;
      } else if (sscanf(line, " objecttype = %u", &u) == 1) {
	fObjecttype = u;
      } else if (sscanf(line, " octet-align = %u", &u) == 1) {
	fOctetalign = u;
      } else if (sscanf(line, " profile-level-id = %x", &u) == 1) {
	fProfile_level_id = u;
      } else if (sscanf(line, " robust-sorting = %u", &u) == 1) {
	fRobustsorting = u;
      } else if (sscanf(line, " sizelength = %u", &u) == 1) {
	fSizelength = u;
      } else if (sscanf(line, " streamstateindication = %u", &u) == 1) {
	fStreamstateindication = u;
      } else if (sscanf(line, " streamtype = %u", &u) == 1) {
	fStreamtype = u;
      } else if (sscanf(line, " cpresent = %u", &u) == 1) {
	fCpresent = u != 0;
      } else if (sscanf(line, " randomaccessindication = %u", &u) == 1) {
	fRandomaccessindication = u != 0;
      } else if (sscanf(line, " config = %[^; \t\r\n]", valueStr) == 1) {
	delete[] fConfig; fConfig = strDup(valueStr);
      } else if (sscanf(line, " mode = %[^; \t\r\n]", valueStr) == 1) {
	delete[] fMode; fMode = strDup(valueStr);
      } else if (sscanf(sdpLine, " sprop-parameter-sets = %[^; \t\r\n]", valueStr) == 1) {
	delete[] fSpropParameterSets; fSpropParameterSets = strDup(valueStr);
      } else if (sscanf(line, " emphasis = %[^; \t\r\n]", valueStr) == 1) {
	delete[] fEmphasis; fEmphasis = strDup(valueStr);
      } else if (sscanf(sdpLine, " channel-order = %[^; \t\r\n]", valueStr) == 1) {
	delete[] fChannelOrder; fChannelOrder = strDup(valueStr);
      } else {
	if (sscanf(line, " %[^; \t\r\n]", valueStr) == 1) {
	  if (strcmp(valueStr, "octet-align") == 0) {
	    fOctetalign = 1;
	  } else if (strcmp(valueStr, "cpresent") == 0) {
            fCpresent = true;
	  } else if (strcmp(valueStr, "crc") == 0) {
	    fCRC = 1;
	  } else if (strcmp(valueStr, "robust-sorting") == 0) {
	    fRobustsorting = 1;
	  } else if (strcmp(valueStr, "randomaccessindication") == 0) {
	    fRandomaccessindication = true;
	  }
	}
      }
      delete[] valueStr;

      while (*line != '\0' && *line != '\r' && *line != '\n'
	     && *line != ';') ++line;
      while (*line == ';') ++line;

      while (*sdpLine != '\0' && *sdpLine != '\r' && *sdpLine != '\n'
	     && *sdpLine != ';') ++sdpLine;
      while (*sdpLine == ';') ++sdpLine;
    }
    delete[] lineCopy;
    return true;
  } while (0);

  return false;
}
bool RTSPSession::parseSDPAttribute_x_dimensions(char const* sdpLine)
{
  bool parseSuccess = false;

  int width, height;
  if (sscanf(sdpLine, "a=x-dimensions:%d,%d", &width, &height) == 2) {
    parseSuccess = true;
    fVideoWidth = (unsigned short)width;
    fVideoHeight = (unsigned short)height;
  }

  return parseSuccess;
}
bool RTSPSession::parseSDPAttribute_framerate(char const* sdpLine) 
{
  bool parseSuccess = false;

  float frate;
  int rate;
  if (sscanf(sdpLine, "a=framerate: %f", &frate) == 1 || sscanf(sdpLine, "a=framerate:%f", &frate) == 1) {
    parseSuccess = true;
    fVideoFPS = (unsigned)frate;
  } else if (sscanf(sdpLine, "a=x-framerate: %d", &rate) == 1) {
    parseSuccess = true;
    fVideoFPS = (unsigned)rate;
  }

  return parseSuccess;
}
bool RTSPSession::parseTransportParams(char const* paramsStr,
					 char*& serverAddressStr, unsigned short& serverPortNum,unsigned short &clientPortNum,
					 unsigned char& rtpChannelId, unsigned char& rtcpChannelId)
{
  serverAddressStr = NULL;
  serverPortNum = 0;
  rtpChannelId = rtcpChannelId = 0xFF;

  char* foundServerAddressStr = NULL;
  bool foundServerPortNum = false;
  bool foundClientPortNum = false;
  bool foundChannelIds = false;
  unsigned rtpCid, rtcpCid;
  bool isMulticast = true; // by default
  char* foundDestinationStr = NULL;
  unsigned short multicastPortNumRTP, multicastPortNumRTCP;
  bool foundMulticastPortNum = false;

  char const* fields = paramsStr;
  char* field = strDupSize(fields);
  while (sscanf(fields, "%[^;]", field) == 1) {
    if (sscanf(field, "server_port=%hu", &serverPortNum) == 1) {
      foundServerPortNum = true;
    } else if (sscanf(field, "client_port=%hu", &clientPortNum) == 1) {
      foundClientPortNum = true;
    } else if (_strncasecmp(field, "source=", 7) == 0) {
      delete[] foundServerAddressStr;
      foundServerAddressStr = strDup(field+7);
    } else if (sscanf(field, "interleaved=%u-%u", &rtpCid, &rtcpCid) == 2) {
      rtpChannelId = (unsigned char)rtpCid;
      rtcpChannelId = (unsigned char)rtcpCid;
      foundChannelIds = true;
    } else if (strcmp(field, "unicast") == 0) {
      isMulticast = false;
    } else if (_strncasecmp(field, "destination=", 12) == 0) {
      delete[] foundDestinationStr;
      foundDestinationStr = strDup(field+12);
    } else if (sscanf(field, "port=%hu-%hu", &multicastPortNumRTP, &multicastPortNumRTCP) == 2 ||
	       sscanf(field, "port=%hu", &multicastPortNumRTP) == 1) {
      foundMulticastPortNum = true;
    }

    fields += strlen(field);
    while (fields[0] == ';') ++fields;
    if (fields[0] == '\0') break;
  }
  delete[] field;

  if (isMulticast && foundDestinationStr != NULL && foundMulticastPortNum) {
    delete[] foundServerAddressStr;
    serverAddressStr = foundDestinationStr;
    serverPortNum = multicastPortNumRTP;
    return true;
  }
  delete[] foundDestinationStr;

  if (foundChannelIds || foundServerPortNum || foundClientPortNum) {
    if (foundClientPortNum && !foundServerPortNum) {
      serverPortNum = clientPortNum;
    }
    serverAddressStr = foundServerAddressStr;
    return true;
  }

  delete[] foundServerAddressStr;
  return false;
}
bool RTSPSession::parseScaleParam(char const* paramStr, float& scale) 
{
  return sscanf(paramStr, "%f", &scale) == 1;
}

bool RTSPSession::parseRTPInfoParams(char const*& paramsStr, u_int16_t& seqNum, u_int32_t& timestamp)
{
  while (paramsStr[0] == ',') ++paramsStr;

  char* field = strDupSize(paramsStr);

  while (sscanf(paramsStr, "%[^;,]", field) == 1) {
    if (sscanf(field, "seq=%hu", &seqNum) == 1 ||
	sscanf(field, "rtptime=%u", &timestamp) == 1) {
    }

    paramsStr += strlen(field);
    if (paramsStr[0] == '\0' || paramsStr[0] == ',') break;
    ++paramsStr;
  }

  delete[] field;
  return true;
}
