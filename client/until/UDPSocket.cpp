#include "UDPSocket.h"
#include "Log.h"

UDPSocket::UDPSocket()
{
	open(SOCK_DGRAM);
}
UDPSocket::~UDPSocket()
{
}

int UDPSocket::recvFrom(unsigned int* outRemoteAddr, unsigned short* outRemotePort,
                            void* ioBuffer, unsigned int inBufLen, unsigned int* outRecvLen) {
    
#if (defined(__WIN32__) || defined(_WIN32)) || __osf__  || __sgi__ || __hpux__
    int addrLen = sizeof(fMsgAddr);
#else
    socklen_t addrLen = sizeof(fMsgAddr);
#endif

#ifdef __sgi__
	int theRecvLen = ::recvfrom(fSocket, ioBuffer, inBufLen, 0, (sockaddr*)&fMsgAddr, &addrLen);
#else
    // Win32 says that ioBuffer is a char*
    int theRecvLen = ::recvfrom(fSocket, (char*)ioBuffer, inBufLen, 0, (sockaddr*)&fMsgAddr, &addrLen);
#endif

    if (theRecvLen == -1){
		fprintf_err("recvFrom is error. errno is %d\n",errno);
		return errno;
    }
    
    *outRemoteAddr = ntohl(fMsgAddr.sin_addr.s_addr);
    *outRemotePort = ntohs(fMsgAddr.sin_port);
    *outRecvLen = (unsigned int)theRecvLen;
    return 1;        
}
bool UDPSocket::sendTo(unsigned int inRemoteAddr, unsigned short inRemotePort, void* inBuffer, unsigned int inLength)
{
    struct sockaddr_in  theRemoteAddr;
    theRemoteAddr.sin_family = AF_INET;
    theRemoteAddr.sin_port = htons(inRemotePort);
    theRemoteAddr.sin_addr.s_addr = htonl(inRemoteAddr);

#ifdef __sgi__
	int theErr = ::sendto(fSocket, inBuffer, inLength, 0, (sockaddr*)&theRemoteAddr, sizeof(theRemoteAddr));
#else
    // Win32 says that inBuffer is a char*
	int theErr = ::sendto(fSocket, (char*)inBuffer, inLength, 0, (sockaddr*)&theRemoteAddr, sizeof(theRemoteAddr));
#endif

    if (theErr == -1){
    	fprintf_err("SendTo is error. errno is %d\n",errno);
        return false;
    }
    return true;
}
