#ifndef __CLIENT_INTERFACE_H__
#define __CLIENT_INTERFACE_H__

extern "C"{

	/*
		描述:启动client端
		参数:
			ipBuf 远端IP地址
			port  远端端口
			streamName 流名
		返回值:
			0 失败
			1 成功
	*/
	int startClient(char *ipBuf,unsigned short port,char *streamName);
	/*
		描述: 停止客户端
		参数:
			无
		返回值:
			0 失败
			1 成功
	*/
	int stopClient();

	/*
		读取数据
		参数:
			frameBuf 缓冲区指针，调用者只需要定义指针即可
			frameLen 帧长度
		返回值: 
			 1 读取成功
			-1 Client端已退出,调用者可根据该值判断客户端是否异常退出
			-2 参数 frameLen 为空 
	*/
	int readData(unsigned char **frameBuf,unsigned int *frameLen);
}

#endif
