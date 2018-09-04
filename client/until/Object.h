/*
	Object class
*/
#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "Version.h"
#include <stdio.h>
#include <stdlib.h>

/*
	此枚举既是对象类型也是对象在队列中的位置下标
	如要扩大对象队列，请同时修改RTSPClient.h文件中的 MAX_OBJECT_NUM 宏
    该对象队列目前支持最大16种对象
*/
enum
{
 	RTSPClientObjectType=0, // RTSPClient 对象
	RTSPSessionObjectType=1, // RTSPSession 对象
	RTPSessionObjectType=2,  // RTPSession 对象
	EngineObjectType=3, // EventLoop 对象
	RTCPSessionObjectType=4, // RTCPSession 对象
	MemoryObjectType=5, // 内存池对象
	H264ObjectType=6, // H264Session 对象
    HeartBeatObjectType=7, //HeartBeatSession 对象
    H264FrameContainerObjectType=8, // 

	endType=16    //此处用于标示队列类型结束 修改此处即可扩大对象队列
};

#define SOCKET_READABLE    (1<<1)
#define SOCKET_WRITABLE    (1<<2)
#define SOCKET_EXCEPTION   (1<<3)

/*do nothing*/
class Object
{
  protected:
	Object();
  public:
	virtual ~Object();
};
#endif
