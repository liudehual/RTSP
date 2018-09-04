/*


*/
#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include "Socket.h"
class TCPSocket:public Socket
{
	public:
		TCPSocket();
	    virtual	~TCPSocket();

		/*NoBlock*/
		int connect(unsigned int inRemoteAddr,unsigned short inRemotePort);

		int send(const char* inData, const unsigned int inLength, unsigned int* outLengthSent);
		int read(void *buffer, const unsigned int length, unsigned int *outRecvLenP);
		unsigned int isConnected(){return fState;}
		enum
        {
        	kNotConnected = 0,
            kConnected  = 1
        };
	private:
	    struct sockaddr_in  fRemoteAddr;
		unsigned int fState;
};
#endif
