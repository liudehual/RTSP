#include "Socket.h"
#include <stdio.h>
#include "SocketHelper.h"
#include "Log.h"

#ifdef HAVE_SOCKADDR_LEN
#define SET_SOCKADDR_SIN_LEN(var) var.sin_len = sizeof var
#else
#define SET_SOCKADDR_SIN_LEN(var)
#endif
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
typedef int socklen_t;
#endif

#define DEFAULT_IP 0

#define MAKE_SOCKADDR_IN(var,adr,prt)\
    struct sockaddr_in var;\
    var.sin_family = AF_INET;\
    var.sin_addr.s_addr = (adr);\
    var.sin_port = (prt);\
    SET_SOCKADDR_SIN_LEN(var);

Socket::Socket():fSocket(0)
{
	memset(remoteIp,0,32);
	memset(localIp,0,32);

    //默认启用第一个网络接口的IP linux(eth0) windows(本地连接?)
   // char *dIp=SocketHelper::GetIPAddrStr(DEFAULT_IP)->GetAsCString();
//	sprintf(localIp,"%s",dIp);
//	delete dIp;
	//fprintf(stderr,SIMPLE_RTSP"----------- %s\n",SocketHelper::GetIPAddrStr(0)->GetAsCString());

}
Socket::~Socket()
{
	fprintf_debug("---------------> ============== delete Socket %d\n",fSocket);
	fprintf_msg("@@@@@@@@@@@@@@@@@@ delete SOCKET\n");
    int err = 0;
#if  defined(__WIN32__) || defined(_WIN32) 
      err = ::closesocket(fSocket);
#else
      err = ::close(fSocket);
#endif
}
int Socket::open(int theType)
{
	fSocket=::socket(PF_INET, theType, 0); //此处未做错误检查
	fprintf_debug("fSocket %d Type %d\n",fSocket,theType);
	return 1;
}

unsigned Socket::getBufferSize(int bufOptName,
			      int socket) 
{
  unsigned curSize;
#if defined(__WIN32__) || defined(_WIN32)
  int sizeSize = sizeof curSize;
#else
  socklen_t  sizeSize = sizeof curSize;
#endif
  if (getsockopt(socket, SOL_SOCKET, bufOptName,
		 (char*)&curSize, &sizeSize) < 0) {
    return 0;
  }
  return curSize;
}
unsigned Socket::getSendBufferSize() 
{
  return getBufferSize(SO_SNDBUF, fSocket);
}
unsigned Socket::getReceiveBufferSize() 
{
  return getBufferSize(SO_RCVBUF, fSocket);
}

unsigned Socket::setBufferTo(int bufOptName,int socket, unsigned requestedSize) 
{
  SOCKLEN_T sizeSize = sizeof requestedSize;
  setsockopt(socket, SOL_SOCKET, bufOptName, (char*)&requestedSize, sizeSize);

  return getBufferSize(bufOptName, socket);
}
unsigned Socket::setSendBufferTo(unsigned requestedSize) 
{
	return setBufferTo(SO_SNDBUF, fSocket, requestedSize);
}
unsigned Socket::setReceiveBufferTo(unsigned requestedSize) 
{
	return setBufferTo(SO_RCVBUF, fSocket, requestedSize);
}

unsigned Socket::increaseBufferTo(int bufOptName,int socket, unsigned requestedSize) 
{
  unsigned curSize = getBufferSize(bufOptName, socket);
  while (requestedSize > curSize) {
    SOCKLEN_T sizeSize = sizeof requestedSize;
    if (setsockopt(socket, SOL_SOCKET, bufOptName,
		   (char*)&requestedSize, sizeSize) >= 0) {
      return requestedSize;
    }
    requestedSize = (requestedSize+curSize)/2;
  }
  return getBufferSize(bufOptName, socket);
}
unsigned Socket::increaseSendBufferTo(unsigned requestedSize) 
{
  return increaseBufferTo(SO_SNDBUF, fSocket, requestedSize);
}
unsigned Socket::increaseReceiveBufferTo(unsigned requestedSize) 
{
  return increaseBufferTo(SO_RCVBUF, fSocket, requestedSize);
}

bool Socket::makeSocketNonBlocking(int sock) 
{
#if defined(__WIN32__) || defined(_WIN32) || defined(IMN_PIM)
  unsigned long arg = 1;
  return ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
  int arg = 1;
  return ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
  int curFlags = fcntl(sock, F_GETFL, 0);
  return fcntl(sock, F_SETFL, curFlags|O_NONBLOCK) >= 0;
#endif
}

bool Socket::makeSocketBlocking(int sock) 
{
#if defined(__WIN32__) || defined(_WIN32) || defined(IMN_PIM)
  unsigned long arg = 0;
  return ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
  int arg = 0;
  return ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
  int curFlags = fcntl(sock, F_GETFL, 0);
  return fcntl(sock, F_SETFL, curFlags&(~O_NONBLOCK)) >= 0;
#endif
}
int Socket::bind(unsigned int addr, unsigned short port)
{
#if  defined(__WIN32__) || defined(_WIN32)
    int len = sizeof(fLocalAddr);
#else
	socklen_t len = sizeof(fLocalAddr);
#endif
    ::memset(&fLocalAddr, 0, sizeof(fLocalAddr));
    fLocalAddr.sin_family = AF_INET;
    fLocalAddr.sin_port = htons(port);
    fLocalAddr.sin_addr.s_addr = htonl(addr);
    
    int err;
    err = ::bind(fSocket, (sockaddr *)&fLocalAddr, sizeof(fLocalAddr));
    
    if (err == -1)
    {
        fLocalAddr.sin_port = 0;
        fLocalAddr.sin_addr.s_addr = 0;
        return errno;
    }
    else ::getsockname(fSocket, (sockaddr *)&fLocalAddr, &len);
    
    return 1;
}
unsigned int Socket::strIpToUInt32(char * ip)
{
	if(ip==NULL){
		return 0;
	}
	return ntohl(::inet_addr(ip));
}
void Socket::UInt32ToStrIp(unsigned int uIntIp, char * strIp)
{
	if(strIp==NULL){
		return;
	}
	unsigned char* p = (unsigned char *)&uIntIp;
	sprintf(strIp, "%u.%u.%u.%u", p[3]&0xff,p[2]&0xff,p[1]&0xff,p[0]&0xff);
}
char *Socket::getRemoteStrIp()
{
	return remoteIp;
}
char *Socket::getLocalStrIp()
{
	return localIp;
}
unsigned short Socket::getRemotePort()
{
	return remotePort;
}
unsigned short Socket::getLocalPort()
{
	return localPort;
}
void Socket::setRemoteStrIp(char *ip)
{
	if(ip==NULL || ip[0]=='\0' || strlen(ip)>sizeof(remoteIp)){
		return;
	}
	memcpy(remoteIp,ip,strlen(ip)+1);
}
void Socket::setLocalStrIp(char *ip)
{
	if(ip==NULL || ip[0]=='\0' || strlen(ip)>sizeof(localIp)){
		return;
	}
	memcpy(localIp,ip,strlen(ip)+1);
}
void Socket::setRemotePort(unsigned short port)
{
	remotePort=port;
}
void Socket::setLocalPort(unsigned short port)
{
	localPort=port;
}

static bool getSourcePort0(int socket, unsigned short& resultPortNum) 
{
  sockaddr_in test; test.sin_port = 0;

  SOCKLEN_T len = sizeof test;
  if (getsockname(socket, (struct sockaddr*)&test, (socklen_t*)&len) < 0) return false;

  resultPortNum = ntohs(test.sin_port);
  return true;
}

bool getSourcePort(int socket, unsigned short& port) 
{
  unsigned short portNum = 0;
  if (!getSourcePort0(socket, portNum) || portNum == 0) {
    MAKE_SOCKADDR_IN(name, INADDR_ANY, 0);
    ::bind(socket, (struct sockaddr*)&name, sizeof name);

    if (!getSourcePort0(socket, portNum) || portNum == 0) {
      return false;
    }
  }

  port = portNum;
  return true;
}

