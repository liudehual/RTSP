#ifndef _RTSPLISTEN_H
#define _RTSPLISTEN_H

#include "TCPListenerSocket.h"
#include "Task.h"
class RtspListener : public TCPListenerSocket
{
  public:
    RtspListener():TCPListenerSocket(){}
    virtual ~RtspListener(){}
    /*��ȡsocket����������*/
    virtual Task* GetSessionTask(TCPSocket** outSocket);
    OS_Error Initialize(UInt32 addr, UInt16 port);

};

#endif
