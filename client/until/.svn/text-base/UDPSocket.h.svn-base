/*


*/
#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__
#include "Socket.h"
class UDPSocket:public Socket
{
	public:
		UDPSocket();
		virtual ~UDPSocket();
		
    	int recvFrom( unsigned int* outRemoteAddr, unsigned short* outRemotePort,void* ioBuffer, unsigned int inBufLen, unsigned int* outRecvLen);
		bool sendTo(unsigned int inRemoteAddr, unsigned short inRemotePort, void* inBuffer, unsigned int inLength);

	private:
		struct sockaddr_in  fMsgAddr;

		//远端IP及端口
	 	char udpRemoteIp[32];
	  	unsigned short udpRemotePort;

		//本地IP及端口
	 	char udpLocalIp[32];
	  	unsigned short udpLocalPort;

};
#endif
