#include "RTCPSession.h"
#include "UDPSocket.h"
#include "RTSPClient.h"
#include "Until.h"
#include "Log.h"

RTCPSession::RTCPSession(RTSPClient *rtspClient):EventJob(rtspClient,new UDPSocket)
{
	UDPSocket *tSocket=((UDPSocket *)getSocket());
	tSocket->makeSocketNonBlocking(tSocket->getSocketFD());
}
RTCPSession::~RTCPSession()
{
}
int RTCPSession::Processor()
{
	fprintf_debug("RTCPSession::Processor\n");
	//SleepBymSec(1000);//sleep 1s
	/*do nothing*/
	return 1;
}

