#include "TCPClientSession.h"
#include "RTSPMediaManager.h"
TCPClientSession::TCPClientSession():masterChannel(NULL)
{
}
TCPClientSession::~TCPClientSession()
{
}
/*设置激活状态*/
void TCPClientSession::setActive(unsigned short activeStatus)
{
	 ifActive=activeStatus;
    if(ifActive==active){
    	if(masterChannel==NULL){
			return;
    	}
        masterChannel->addActiveTCPClientSessionNum();  //任务启动，增加通道管理类中的激活任务数
    }
    if(ifActive==notActive){
    	if(masterChannel==NULL){
			return;
    	}
        masterChannel->addActiveTCPClientSessionNum();  //任务启动，增加通道管理类中的激活任务数
    }

}