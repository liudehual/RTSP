#include <stdio.h>
#include <stdlib.h>
#include "ClientInterface.h"
#include "Until.h"
void usage()
{
	fprintf(stderr,"%s"
	"Usage:\n"
	"./mediaClient remoteIp remotePort streamName \n"
	"if you want request a channel video you should make the streamName=live[0-32].264\n"
	"if you want request a video file you should make sure that streamName= fileName\n"
	);
}

int testDemo(int argc,char *argv[])
{
	
#if 1
	int thePort = ::atoi(argv[2]);
	
	/*创建 RtspClient 实例,并传入参数
		arg1 服务端ip地址
		arg2 服务端端口
		arg3 流名
		
		//可以在函数后面添加新的参数添加新的参数
	*/
	if(!startClient(argv[1],thePort,argv[3])){
		return 0;
	}

	/*
		
	*/
	int status;
	while(1){
			

			SleepBymSec(10);
			unsigned char *frame=NULL;
			unsigned int frameLen=0;

			status=readData(&frame,&frameLen);
			if(status==-1){ /*客户端线程退出了，我们也退出*/
				fprintf(stderr,"Client Exit\n");
				return 1;
			}
			if(frame==NULL){
			   continue;
			}
			#if 0
			fprintf(stderr,"Frame Type %d FrameLne %d\n",(unsigned char)frame[4]&0x1F,frameLen);
			#endif
			#if 1 /*写入文件测试,参数解析是否错误*/
			
			
			/*打开文件*/
			static FILE *fp=NULL;
			static int dataSize=0;
			if(dataSize<20*1024*1024){
			if(fp==NULL){
					fp=fopen("data-720P.h264","ab+");
			}
			if(fp!=NULL){
				fwrite(frame,frameLen,1,fp);
				dataSize+=frameLen;
			}
			}
			
			#endif
	}
	

	#endif
	return 1;
}
int main(int argc,char *argv[])
{
	if(argc!=4){
		usage();
		return 0;
	}
	testDemo(argc,argv);

	fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);
	//	fprintf(stderr,"hello world\n");
	//	fprintf(stderr,"local Ip %s\n",  SocketHelper::GetIPAddrStr(0)->GetAsCString());
	//	fprintf(stderr,"local Ip %s\n",  SocketHelper::GetIPAddrStr(1)->GetAsCString());
	//	SleepBymSec(100);
	
	return 1;
}
