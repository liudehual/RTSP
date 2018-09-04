#include "RTSPInterface.h"
#include "server.h"
#include "version.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


#define WRITE_SINGLE_FRAME 0 /*0 --> 67 68 65 一次性写入   1 ---> 67 68 65 单独写*/
//#define WRITE_MUL_FRAME 
#if 1
void *run(void *arg)
{
	
	
    unsigned char *p0=NULL;
    unsigned size0=0;
    unsigned char *p1=NULL;
    unsigned size1=0;
    struct timeval timeout;

	bool sps=false;
	bool pps=false;
	unsigned char tBuf[3*1024*1024];
	unsigned int aDataLen=0;
	bool i_frame=false;


    while(1){
        timeout.tv_sec=0;
        timeout.tv_usec=39000;
		/*该函数在有些终端设备上的超时是严重不准确的，如果出现视频画面播放缓慢，既可能是网络的原因，也可能是超时严重不准确造成的*/
        select(0,NULL,NULL,NULL,&timeout);

      	while(1){
		readframefromfileandsend1(p1,size1);


		/*67 68 65 放在一起写*/
       // fprintf(stderr,"Find SPS PPS %d\n",((unsigned char)p1[4]&0x1F));

		if(((unsigned char)p1[4]&0x1F)==7){
			aDataLen=0;
			memcpy(&tBuf[aDataLen],p1,size1);
			aDataLen+=size1;
			continue;
		}
		if(((unsigned char)p1[4]&0x1F)==8){
			memcpy(&tBuf[aDataLen],p1,size1);
			aDataLen+=size1;
			continue;
		}
		if(((unsigned char)p1[4]&0x1F)==6){
			memcpy(&tBuf[aDataLen],p1,size1);
			aDataLen+=size1;
			continue;
		}
		if(((unsigned char)p1[4]&0x1F)==5){
			memcpy(&tBuf[aDataLen],p1,size1);

			aDataLen+=size1;
			writeDataToChannel(0,tBuf,aDataLen);
        	writeDataToChannel(1,tBuf,aDataLen);
        	writeDataToChannel(2,tBuf,aDataLen);
        	writeDataToChannel(3,tBuf,aDataLen);
        	writeDataToChannel(4,tBuf,aDataLen);
        	writeDataToChannel(5,tBuf,aDataLen);
        	writeDataToChannel(6,tBuf,aDataLen);
        	writeDataToChannel(7,tBuf,aDataLen);
        	writeDataToChannel(8,tBuf,aDataLen);
        	writeDataToChannel(9,tBuf,aDataLen);
            i_frame=true;
        }
		#endif

			break;
        }
        if(i_frame){
            i_frame=false;
            continue;
        }
        	/* s/p帧 */
                writeDataToChannel(0,p1,size1);
        		writeDataToChannel(1,p1,size1);
       	 		writeDataToChannel(2,p1,size1);
        		writeDataToChannel(3,p1,size1);
        		writeDataToChannel(4,p1,size1); 
        		writeDataToChannel(5,p1,size1);
        		writeDataToChannel(6,p1,size1);
       	 		writeDataToChannel(7,p1,size1);
        		writeDataToChannel(8,p1,size1);
        		writeDataToChannel(9,p1,size1); 

        #if 0
        writeDataToChannel(0,p0,size0);
        writeDataToChannel(1,p1,size1);
        writeDataToChannel(2,p0,size0);
        writeDataToChannel(3,p1,size1);
        writeDataToChannel(4,p0,size0);
        writeDataToChannel(5,p1,size1);
        writeDataToChannel(6,p0,size0);
        writeDataToChannel(7,p1,size1);
        writeDataToChannel(8,p0,size0);
        writeDataToChannel(9,p1,size1);
        writeDataToChannel(10,p0,size0);
        writeDataToChannel(11,p1,size1);
        writeDataToChannel(12,p0,size0);
        writeDataToChannel(13,p1,size1);
        writeDataToChannel(14,p0,size0);
        writeDataToChannel(15,p1,size1);
        #endif
       // writeDataToChannel(16,p0,size0);
       // writeDataToChannel(17,p1,size1);
       // writeDataToChannel(18,p0,size0);
       // writeDataToChannel(19,p1,size1);
       // writeDataToChannel(20,p0,size0);
       // writeDataToChannel(21,p1,size1);
       // writeDataToChannel(22,p0,size0);
       // writeDataToChannel(23,p1,size1);
       // writeDataToChannel(24,p0,size0);
       // writeDataToChannel(25,p1,size1);
       // writeDataToChannel(26,p0,size0);
       // writeDataToChannel(27,p1,size1);
       // writeDataToChannel(28,p0,size0);
       // writeDataToChannel(29,p1,size1);
       // writeDataToChannel(30,p0,size0);
       // writeDataToChannel(31,p1,size1);
       // writeDataToChannel(32,p0,size0);
       // writeDataToChannel(33,p0,size0);
        
    }
}

void sigcatcher(int sig, int /*sinfo*/, struct sigcontext* /*sctxt*/)
{
  #define DEBUG 1
  #if DEBUG
    qtss_printf("\n%d\n", sig);
  #endif

}
/*屏蔽信息 用于测试*/
void initSingal()
{
#if 0
   
//处理部分信号
struct sigaction act;
    
#if defined(sun) || defined(i386) || defined (__MacOSX__) || defined(__powerpc__) || defined (__osf__) || defined (__sgi_cc__) || defined (__hpux__)
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = (void(*)(int))&sigcatcher;
#elif defined(__sgi__) 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = (void(*)(...))&sigcatcher;
#else
    act.sa_mask = 0;
    act.sa_flags = 0;
    act.sa_handler = (void(*)(...))&sigcatcher;
#endif

    sigset_t block;  
 
    sigemptyset(&block);  
    
sigaddset(&block, SIGINT);  
sigaddset(&block, SIGHUP);    
sigaddset(&block, SIGINT);    
sigaddset(&block, SIGQUIT);    
sigaddset(&block, SIGILL);    
sigaddset(&block, SIGTRAP);    
sigaddset(&block, SIGABRT);    
sigaddset(&block, SIGBUS);    
sigaddset(&block, SIGFPE);    
sigaddset(&block, SIGKILL);    
sigaddset(&block, SIGUSR1);    
sigaddset(&block, SIGSEGV);    
sigaddset(&block, SIGUSR2);    
sigaddset(&block, SIGPIPE);    
sigaddset(&block, SIGCONT);    
sigaddset(&block, SIGTTOU);    
sigaddset(&block, SIGVTALRM);  
sigaddset(&block, SIGPWR);     
sigaddset(&block, SIGALRM);    
sigaddset(&block, SIGSTOP);    
sigaddset(&block, SIGURG);     
sigaddset(&block, SIGPROF);    
sigaddset(&block, SIGSYS);     
sigaddset(&block, SIGTERM);    
sigaddset(&block, SIGTSTP);    
sigaddset(&block, SIGXCPU);    
sigaddset(&block, SIGWINCH);   
sigaddset(&block, SIGCHLD);    
sigaddset(&block, SIGTTIN);    
sigaddset(&block, SIGXFSZ);    
sigaddset(&block, SIGIO);      
sigaddset(&block,SIGRTMIN);    
sigaddset(&block,SIGRTMIN+1); 
sigaddset(&block,SIGRTMIN+2); 
sigaddset(&block,SIGRTMIN+6); 
sigaddset(&block,SIGRTMIN+10); 
sigaddset(&block,SIGRTMIN+14); 
sigaddset(&block,SIGRTMAX-12); 
sigaddset(&block,SIGRTMAX-8); 
sigaddset(&block,SIGRTMAX-4); 
sigaddset(&block,SIGRTMAX); 
sigaddset(&block,SIGRTMIN+3); 
sigaddset(&block,SIGRTMIN+7); 
sigaddset(&block,SIGRTMIN+11); 
sigaddset(&block,SIGRTMIN+15); 
sigaddset(&block,SIGRTMAX-11); 
sigaddset(&block,SIGRTMAX-7); 
sigaddset(&block,SIGRTMAX-3); 
sigaddset(&block,SIGRTMIN+4); 
sigaddset(&block,SIGRTMIN+8); 
sigaddset(&block,SIGRTMIN+12); 
sigaddset(&block,SIGRTMAX-14); 
sigaddset(&block,SIGRTMAX-10); 
sigaddset(&block,SIGRTMAX-6); 
sigaddset(&block,SIGRTMAX-2); 
sigaddset(&block,SIGRTMIN+5); 
sigaddset(&block,SIGRTMIN+9); 
sigaddset(&block,SIGRTMIN+13); 
sigaddset(&block,SIGRTMAX-13); 
sigaddset(&block,SIGRTMAX-9); 
sigaddset(&block,SIGRTMAX-5); 
sigaddset(&block,SIGRTMAX-1); 

  sigprocmask(SIG_BLOCK, &block, NULL);
   
    //非实时信号
    (void)::sigaction(SIGHUP  , &act, NULL);
    (void)::sigaction(SIGINT  , &act, NULL);
    (void)::sigaction(SIGQUIT , &act, NULL);
    (void)::sigaction(SIGILL  , &act, NULL);
    (void)::sigaction(SIGTRAP , &act, NULL);
    (void)::sigaction(SIGABRT , &act, NULL);
    (void)::sigaction(SIGBUS  , &act, NULL);
    (void)::sigaction(SIGFPE  , &act, NULL);
    (void)::sigaction(SIGKILL , &act, NULL);
    (void)::sigaction(SIGUSR1 , &act, NULL);
    (void)::sigaction(SIGSEGV , &act, NULL);
    (void)::sigaction(SIGUSR2 , &act, NULL);
    (void)::sigaction(SIGPIPE , &act, NULL);
    (void)::sigaction(SIGCONT , &act, NULL);
    (void)::sigaction(SIGTTOU , &act, NULL);
    //(void)::sigaction(SIGVTALR, &act, NULL);
    (void)::sigaction(SIGPWR  , &act, NULL);
    (void)::sigaction(SIGALRM , &act, NULL);
    (void)::sigaction(SIGSTOP , &act, NULL);
    (void)::sigaction(SIGURG  , &act, NULL);
    (void)::sigaction(SIGPROF , &act, NULL);
    (void)::sigaction(SIGSYS  , &act, NULL);
    (void)::sigaction(SIGTERM , &act, NULL);
    (void)::sigaction(SIGTSTP , &act, NULL);
    (void)::sigaction(SIGXCPU , &act, NULL);
    (void)::sigaction(SIGWINCH, &act, NULL);
    (void)::sigaction(SIGCHLD , &act, NULL);
    (void)::sigaction(SIGTTIN , &act, NULL);
    (void)::sigaction(SIGXFSZ , &act, NULL);
    (void)::sigaction(SIGIO   , &act, NULL);

     //实时信号
(void)::sigaction(SIGRTMIN   , &act, NULL); 
(void)::sigaction(SIGRTMIN+1 , &act, NULL); 
(void)::sigaction(SIGRTMIN+2 , &act, NULL); 
(void)::sigaction(SIGRTMIN+6 , &act, NULL); 
(void)::sigaction(SIGRTMIN+10, &act, NULL); 
(void)::sigaction(SIGRTMIN+14, &act, NULL); 
(void)::sigaction(SIGRTMAX-12, &act, NULL); 
(void)::sigaction(SIGRTMAX-8 , &act, NULL); 
(void)::sigaction(SIGRTMAX-4 , &act, NULL); 
(void)::sigaction(SIGRTMAX   , &act, NULL); 
(void)::sigaction(SIGRTMIN+3 , &act, NULL); 
(void)::sigaction(SIGRTMIN+7 , &act, NULL); 
(void)::sigaction(SIGRTMIN+11, &act, NULL); 
(void)::sigaction(SIGRTMIN+15, &act, NULL); 
(void)::sigaction(SIGRTMAX-11, &act, NULL); 
(void)::sigaction(SIGRTMAX-7 , &act, NULL); 
(void)::sigaction(SIGRTMAX-3 , &act, NULL); 
(void)::sigaction(SIGRTMIN+4 , &act, NULL); 
(void)::sigaction(SIGRTMIN+8 , &act, NULL); 
(void)::sigaction(SIGRTMIN+12, &act, NULL); 
(void)::sigaction(SIGRTMAX-14, &act, NULL); 
(void)::sigaction(SIGRTMAX-10, &act, NULL); 
(void)::sigaction(SIGRTMAX-6 , &act, NULL); 
(void)::sigaction(SIGRTMAX-2 , &act, NULL); 
(void)::sigaction(SIGRTMIN+5 , &act, NULL); 
(void)::sigaction(SIGRTMIN+9 , &act, NULL); 
(void)::sigaction(SIGRTMIN+13, &act, NULL); 
(void)::sigaction(SIGRTMAX-13, &act, NULL); 
(void)::sigaction(SIGRTMAX-9 , &act, NULL); 
(void)::sigaction(SIGRTMAX-5 , &act, NULL); 
(void)::sigaction(SIGRTMAX-1 , &act, NULL);

    (void)::sigaction(SIGPIPE, &act, NULL);
    (void)::sigaction(SIGHUP,  &act, NULL);
    (void)::sigaction(SIGINT,  &act, NULL);
    (void)::sigaction(SIGTERM, &act, NULL);
    (void)::sigaction(SIGQUIT, &act, NULL);
    (void)::sigaction(SIGALRM, &act, NULL);
    
    #endif
}

int main(int argc,char *argv[])
{
    //char c = getchar();
 
	/*初始化OS*/
	/*DEBUG*/
	#if 1
	 //char c = getchar();
	/*打开视频文件1*/
    openVediofile1();
    fprintf(stderr,"Start Channel\n");
    fprintf(stderr,"Version %s\n",VERSION);


    /*启动程序*/
    startChannelsByNum(34);
    OSThread::Sleep(1000);
    fprintf(stderr,"Start Listen\n");
	/*启动监听 最大允许绑定64个端口*/
	startListener(554);
	startListener(7070);
#endif

	
    #if 0
    RtspListener *listen1 =new RtspListener;
    listen1->Initialize(INADDR_ANY,554);
    listen1->RequestEvent(EV_RE);

	fprintf(stderr,"Start Listen\n");
    RtspListener *listen2 =new RtspListener;
    listen2->Initialize(INADDR_ANY,7070);
    listen2->RequestEvent(EV_RE);
	#endif
	
    fprintf(stderr,"Run Thread\n");
    OSThread::Sleep(1000);

	/*创建写数据线程*/
    pthread_t thread;
    pthread_create(&thread,NULL,run,NULL);
    // if(!registerToServer(ip2, port2,dvsId2,card1)) return 0; //注册失败，退出
    while(1){
    	OSThread::Sleep(1*1000000);
    }
    return 0;
}
