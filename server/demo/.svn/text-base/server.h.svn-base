#ifndef _RTSP_DEMO_H
#define _RTSP_DEMO_H
#include <unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include"OS.h"
#include"OSHeaders.h"
#include"Task.h"
#include"OSThread.h"
#include"IdleTask.h"
#include"Socket.h"
#include"UDPSocket.h"
#include"SocketUtils.h"

#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/mman.h>
int file_len;
void *pfile,*start_fp;

//检查是否是一帧数据
int chece_one_frame(unsigned char * buf){
  if(buf[0]==0 && buf[1]==0 && buf[2]==0 && buf[3]==1 
  	&& (   (buf[4]&0x1F) == 1
  		|| (buf[4]&0x1F) == 5
  		|| (buf[4]&0x1F) == 7
  		|| (buf[4]&0x1F) == 8 
  		|| (buf[4]&0x1F) == 6 )){
      return 1;
    }
  return 0;
}

//获取h264 nalu
int get_nalu(unsigned char * buf,unsigned len){
  int i;
  for(i=5;i<len-5;i++){
      if(chece_one_frame(&buf[i])==1){
          return i;
        }
    }
  return 0;
}

//从文件中读取帧数据
char * readframefromfileandsend(unsigned char *&buf,unsigned &off)
{

  int ifEnd=file_len-((long)pfile - (long)start_fp); //文件剩余量
  off = get_nalu((unsigned char *)pfile,ifEnd);
  if(off > 0){
      buf=(unsigned char *)pfile;
      pfile += off;
    }
  if(off<=0){
      pfile = start_fp;
    }
  return NULL;
}

//打开视频文件
void openVediofile()
{
  int a = 10;
  int i;
  int fd=-1;

  if(fd < 0){

      if((fd = open("cif.h264", O_RDWR, S_IRWXU)) < 0)
	{ //打开视频文件
          printf("open file wrong!");
          exit(1);
        }
      struct stat file_stat;
      if (fstat(fd, &file_stat) < 0 ){ //获取文件长度
          printf(" fstat wrong");
          exit(1);
        }
      file_len = file_stat.st_size;
      if(NULL==start_fp){
          if( ( start_fp = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0 )) == MAP_FAILED)
	    { //映射文件内存空间
              printf("mmap wrong");
              exit(0);
            }
        }
    }
  pfile = start_fp;


}

int file_len1;
void *pfile1,*start_fp1;
//检查是否是一帧数据
int chece_one_frame1(unsigned char * buf){
  if(buf[0]==0 && buf[1]==0 && buf[2]==0 && buf[3]==1 
  	&& (   (buf[4]&0x1F) == 1
  		|| (buf[4]&0x1F) == 5
  		|| (buf[4]&0x1F) == 7
  		|| (buf[4]&0x1F) == 8 
  		|| (buf[4]&0x1F) == 6 )){
      return 1;
    }
  return 0;
}
//获取h264 nalu
int get_nalu1(unsigned char * buf,unsigned len){
  int i;
  for(i=5;i<len-5;i++){
      if(chece_one_frame1(&buf[i])==1){
          return i;
        }
    }
  return 0;
}
//从文件中读取帧数据
char * readframefromfileandsend1(unsigned char *&buf,unsigned &off)
{

  int ifEnd=file_len1-((long)pfile1 - (long)start_fp1); //文件剩余量
  off = get_nalu1((unsigned char *)pfile1,ifEnd);
  if(off > 0){
      buf=(unsigned char *)pfile1;
      pfile1 += off;
    }
  if(off<=0){
      pfile1 = start_fp1;
    }
  return NULL;
}
//打开视频文件
void openVediofile1()
{
  int a = 10;
  int i;
  int fd=-1;

  if(fd < 0){

      if((fd = open("D1.h264", O_RDWR, S_IRWXU)) < 0)

	  { //打开视频文件
          printf("open file wrong!");
          exit(1);
        }
      fprintf(stderr,"D1.h265 file Describe is %d\n",fd);
      struct stat file_stat;
      if (fstat(fd, &file_stat) < 0 ){ //获取文件长度
          printf(" fstat wrong");
          exit(1);
        }
      file_len1 = file_stat.st_size;
      if(NULL==start_fp1){

          if( ( start_fp1 = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0 )) == MAP_FAILED)

		  { //映射文件内存空间
              printf("mmap wrong");
              exit(0);
            }
        }
    }
  pfile1 = start_fp1;


}


#endif
