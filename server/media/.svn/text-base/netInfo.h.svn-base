#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <ifaddrs.h>

// 获取IP地址，子网掩码，MAC地址
int GetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
    );

//获取默认网关
int GetDefaultGateway(
    const char* lpszEth,
    char* szDefaultGateway
    );
//设置本地IP地址
int SetLocalNetInfo(
    const char* lpszEth,
    const char* lpszIpAddr,
    const char* lpszNetmask
    );
//设置MTU
int SetMTU(
    const char* lpszEth,
    unsigned int mtu
    );
//关闭网卡
int SetIfDown(
    const char* lpszEth
    );
//打开网卡
int SetIfUp(
    const char* lpszEth
    );
//绑定socket文件描述符到指定网卡
int bindNetCard(
    int sockfileDec,
    char *cardName
    );

//查询指定数据是否存
#define IP_TYPE 1    //查询IP
#define ETH_TYPE 2   //网络接口
#define B_TYPE 3     //广播
#define S_TYPE 4     //子网掩码
int getSubnetMask(
    char *lpszEth,
    int type
    );

//添加或删除指定路由表项
int startRoute(
    char *ac,    //动作 add/del
    char *hOrI,  // 网络或主机 -net/-host
    char *ip,    // 目标IP
    char *netmask, //子网掩码
    char *gw,     //网关
    char *dev,    //网络接口名
    char *mtu     //MTU
    );
