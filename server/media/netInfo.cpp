#include "netInfo.h"
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <net/if.h>       /* ifreq struct */
#include <netdb.h>
#include <time.h>

#define RTACTION_ADD 1   /* 添加动作*/
#define RTACTION_DEL 2   /* 删除动作*/
// 获取IP地址，子网掩码，MAC地址
int GetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
    )
{
  if(lpszEth==NULL || lpszEth[0]=='\0'
     || szIpAddr==NULL || szNetmask==NULL || szMacAddr==NULL)
    return 0;
  int ret = 0;

  struct ifreq req;
  struct sockaddr_in* host = NULL;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      return -1;
    }
  bzero(&req, sizeof(struct ifreq));
  strcpy(req.ifr_name, lpszEth);
  if ( ioctl(sockfd, SIOCGIFADDR, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      strcpy(szIpAddr, inet_ntoa(host->sin_addr));
    }else{
      ret = -1;
    }
  bzero(&req, sizeof(struct ifreq));
  strcpy(req.ifr_name, lpszEth);
  if ( ioctl(sockfd, SIOCGIFNETMASK, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      strcpy(szNetmask, inet_ntoa(host->sin_addr));
    }else{
      ret = -1;
    }
  bzero(&req, sizeof(struct ifreq));
  strcpy(req.ifr_name, lpszEth);
  if (ioctl(sockfd, SIOCGIFHWADDR,&req)>= 0 ){
      sprintf(
            szMacAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)req.ifr_hwaddr.sa_data[0],
          (unsigned char)req.ifr_hwaddr.sa_data[1],
          (unsigned char)req.ifr_hwaddr.sa_data[2],
          (unsigned char)req.ifr_hwaddr.sa_data[3],
          (unsigned char)req.ifr_hwaddr.sa_data[4],
          (unsigned char)req.ifr_hwaddr.sa_data[5]
          );
    }else{
      ret = -1;
    }
  if ( sockfd != -1 ){
      close(sockfd);
      sockfd = -1;
    }
  return ret;
}

// ------------------------------------------------------

#define BUFSIZE 8192

struct route_info
{
  char ifName[IF_NAMESIZE];
  u_int gateWay;
  u_int srcAddr;
  u_int dstAddr;
};

static int ReadNlSock(
    int sockFd,
    char* bufPtr,
    int seqNum,
    int pId
    )
{
  struct nlmsghdr* nlHdr = NULL;
  int readLen = 0, msgLen = 0;
  while (1){
      if ( (readLen = recv(sockFd,bufPtr,BUFSIZE-msgLen,0))<0){
          return -1;
        }
      nlHdr = (struct nlmsghdr *)bufPtr;
      if ( (NLMSG_OK(nlHdr, (unsigned int)readLen) == 0)
           || (nlHdr->nlmsg_type == NLMSG_ERROR)){
          return -1;
        }
      if ( nlHdr->nlmsg_type == NLMSG_DONE){
          break;
        }
      else{
          bufPtr += readLen;
          msgLen += readLen;
        }
      if ( (nlHdr->nlmsg_flags & NLM_F_MULTI) == 0 ){
          break;
        }

      if ( (nlHdr->nlmsg_seq != (unsigned int)seqNum)
           || (nlHdr->nlmsg_pid != (unsigned int)pId)){
          break;
        }
    }

  return msgLen;
}

static int ParseRoutes(
    struct nlmsghdr* nlHdr,
    struct route_info* rtInfo,
    char* default_gateway
    )
{
  int rtLen = 0;
  struct in_addr dst;
  struct in_addr gate;
  struct rtmsg* rtMsg = NULL;
  struct rtattr* rtAttr = NULL;
  rtMsg = (struct rtmsg*)NLMSG_DATA(nlHdr);
  if ( (rtMsg->rtm_family != AF_INET)
       || (rtMsg->rtm_table != RT_TABLE_MAIN)){
      return -1;
    }
  rtAttr = (struct rtattr*)RTM_RTA(rtMsg);
  rtLen = RTM_PAYLOAD(nlHdr);
  for ( ; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)){
      switch (rtAttr->rta_type){
        case RTA_OIF:
          if_indextoname(*(int*)RTA_DATA(rtAttr), rtInfo->ifName);
          break;
        case RTA_GATEWAY:
          rtInfo->gateWay = *(u_int*)RTA_DATA(rtAttr);
          break;
        case RTA_PREFSRC:
          rtInfo->srcAddr = *(u_int*)RTA_DATA(rtAttr);
          break;
        case RTA_DST:
          rtInfo->dstAddr = *(u_int*)RTA_DATA(rtAttr);
          break;
        }
    }
  dst.s_addr = rtInfo->dstAddr;
  if (strstr((char*)inet_ntoa(dst), "0.0.0.0")){
      gate.s_addr = rtInfo->gateWay;
      strcpy(default_gateway, (char*)inet_ntoa(gate));
    }
  return 0;
}

// 获取默认网关
int GetDefaultGateway(
    const char* lpszEth,
    char* szDefaultGateway
    )
{
  if(lpszEth==NULL ||
     lpszEth[0]=='\0' ||
     szDefaultGateway==NULL
     )
    return 0;
  static char szGatewayTemp[32] = {0};
  static char msgBuf[BUFSIZE] = {0};
  static struct route_info ri;
  int ret = -1;
  struct nlmsghdr* nlMsg = NULL;
  struct rtmsg* rtMsg = NULL;
  struct route_info* rtInfo = &ri;
  int len = 0, msgSeq = 0;

  int sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
  if ( -1 == sock ){
      goto END;
    }

  nlMsg = (struct nlmsghdr*)msgBuf;
  rtMsg = (struct rtmsg*)NLMSG_DATA(nlMsg);

  nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
  nlMsg->nlmsg_type = RTM_GETROUTE;
  nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
  nlMsg->nlmsg_seq = msgSeq++;
  nlMsg->nlmsg_pid = getpid();

  if ( send(sock, nlMsg, nlMsg->nlmsg_len, 0)<0){
      goto END;
    }

  if ( (len = ReadNlSock(sock, msgBuf, msgSeq, getpid())) < 0 ){
      goto END;
    }

  if ( rtInfo != NULL ){
      for ( ; NLMSG_OK(nlMsg, (unsigned int)len); nlMsg = NLMSG_NEXT(nlMsg, len)){
          memset(szGatewayTemp, 0, 64);
          memset(rtInfo, 0, sizeof(struct route_info));
          if ( 0 == ParseRoutes(nlMsg, rtInfo, szGatewayTemp)){
              if ( strcmp(rtInfo->ifName, lpszEth) == 0
                   && strlen(szGatewayTemp) > 0 ){
                  fprintf(stderr,"szGatewayTemp %s\n",szGatewayTemp);
                  strcpy(szDefaultGateway, szGatewayTemp);
                  ret = 0;
                }
            }
        }
    }

END:
  fprintf(stderr,"occered error happened\n");
  if ( sock != -1 ){
      close(sock);
      sock = -1;
    }

  return ret;
}

// ------------------------------------------------------

// 设置IP地址和子网掩码
int SetLocalNetInfo(
    const char* lpszEth,
    const char* lpszIpAddr,
    const char* lpszNetmask
    )
{
  if(lpszEth==NULL || lpszEth[0]=='\0' ||
     lpszIpAddr==NULL || lpszIpAddr[0]=='\0' ||
     lpszNetmask==NULL || lpszNetmask[0]=='\0'
     )
    return 0;
  int ret = -1;

  struct ifreq req;
  struct sockaddr_in* host = NULL;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      goto END;
    }

  bzero(&req, sizeof(struct ifreq));
  strcpy(req.ifr_name, lpszEth);
  host = (struct sockaddr_in*)&req.ifr_addr;
  host->sin_family = AF_INET;
  if (1!=inet_pton(AF_INET, lpszIpAddr, &(host->sin_addr))){
      goto END;
    }
  if (ioctl(sockfd, SIOCSIFADDR, &req) < 0 ){
      goto END;
    }

  bzero(&req, sizeof(struct ifreq));
  strcpy(req.ifr_name, lpszEth);
  host = (struct sockaddr_in*)&req.ifr_addr;
  host->sin_family = AF_INET;
  if ( 1 != inet_pton(AF_INET, lpszNetmask, &(host->sin_addr))){
      goto END;
    }
  if ( ioctl(sockfd, SIOCSIFNETMASK, &req) < 0 ){
      goto END;
    }

  ret = 0;

END:

  if ( sockfd != -1 )
    {
      close(sockfd);
      sockfd = -1;
    }

  return ret;
}

// 设置指定网口的mtu值
int SetMTU(const char* lpszEth, unsigned int mtu)
{
  if(lpszEth==NULL || lpszEth[0]=='\0')
    return 0;

  int ret = -1;

  struct ifreq ifr;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      goto END;
    }

  strcpy(ifr.ifr_name, lpszEth);
  ifr.ifr_ifru.ifru_mtu = mtu;
  if ( ioctl(sockfd, SIOCSIFMTU, &ifr) < 0 ){
      goto END;
    }

  ret = 0;

END:

  if ( sockfd != -1 ){
      close(sockfd);
      sockfd = -1;
    }

  return ret;
}

// 关闭指定网口
int SetIfDown(const char* lpszEth)
{
  if(lpszEth==NULL || lpszEth[0]=='\0')
    return 0;
  int ret = -1;
  struct ifreq ifr;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      goto END;
    }
  strcpy(ifr.ifr_name, lpszEth);
  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0 ){
      goto END;
    }

  ifr.ifr_flags &= ~IFF_UP;

  if ( ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0 ){
      goto END;
    }

  ret = 0;

END:

  if ( sockfd != -1 ){
      close(sockfd);
      sockfd = -1;
    }

  return ret;
}

// 打开指定网口
int SetIfUp(const char* lpszEth)
{
  if(lpszEth==NULL || lpszEth[0]=='\0')
    return 0;
  int ret = -1;

  struct ifreq ifr;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      goto END;
    }

  strcpy(ifr.ifr_name, lpszEth);

  if ( ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0 ){
      goto END;
    }

  ifr.ifr_flags |= IFF_UP;

  if ( ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0 ){
      goto END;
    }

  ret = 0;

END:

  if ( sockfd != -1 ){
      close(sockfd);
      sockfd = -1;
    }

  return ret;
}
//绑定socket到指定网络接口
int bindNetCard(int sockfileDec,char *cardName)
{
  if(cardName==NULL || cardName[0]=='\0')
    return 0;
  struct ifreq interface;
  /* Acquire socket here ... */
  strcpy(interface.ifr_ifrn.ifrn_name,cardName);
  if (setsockopt(sockfileDec, SOL_SOCKET, SO_BINDTODEVICE, (char*)&interface, sizeof(interface))!=0) {
      fprintf(stderr,"Bind Card is error \n");
      return 0;
      /* Deal with error... */
    }
  return 1;
}

//查找指定信息
int getSubnetMask(char *lpszEth,int type)
{
  if(lpszEth==NULL || lpszEth[0]=='\0')
    return 0;
  struct sockaddr_in *sin = NULL;
  struct ifaddrs *ifa = NULL, *ifList;

  if (getifaddrs(&ifList) < 0){
      return -1;
    }
  for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next){
      if(ifa->ifa_addr->sa_family == AF_INET){

          if(strcmp(lpszEth,ifa->ifa_name)==0 && type == ETH_TYPE) return 1;

          sin = (struct sockaddr_in *)ifa->ifa_addr;
          if(strcmp(lpszEth,inet_ntoa(sin->sin_addr))==0 && type == IP_TYPE) return 1;

          sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
          if(strcmp(lpszEth,inet_ntoa(sin->sin_addr))==0 && type == B_TYPE) return 1;

          sin = (struct sockaddr_in *)ifa->ifa_netmask;
          if(strcmp(lpszEth,inet_ntoa(sin->sin_addr))==0 && type == S_TYPE) return 1;

        }
    }
  freeifaddrs(ifList);
  return 0;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ add by gct 1604131143 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void usage();
int inet_setroute(int action, char **args);
int inet6_setroute(int action, char **args);


/* main function */
int startRoute(char *ac,char *hOrI,char *ip,char *netmask,char *gw,char *dev,char *mtu)
{
  if(ac==NULL || ac[0]=='\0' ||
     hOrI==NULL || hOrI[0]=='\0' ||
     ip==NULL || ip[0]=='\0' ||
     netmask==NULL || netmask[0]=='\0' ||
     gw==NULL || gw[0]=='\0' ||
     dev==NULL || dev[0]=='\0' ||
     mtu==NULL || mtu[0]=='\0'
     )
    return -1;
  int action = 0;
  char *argv[15];

  int argc=5;
  argv[0]="route";
  argv[1]="-A";
  argv[2]="inet";
  argv[3]=ac;
  argv[4]=hOrI;
  argv[5]=ip;
  argv[6]="netmask";
  argv[7]=netmask;
  argv[8]="gw";
  argv[9]=gw;
  argv[10]="dev";
  argv[11]=dev;
  argv[12]="mtu";
  argv[13]=mtu;
  argv[14]=NULL;
  if(argc < 5){
      usage();
      return -1;
    }
  if(strcmp(argv[1], "-A")!=0){
      usage();
      return -1;
    }

  if(strcmp(argv[3], "add")==0){
      action = RTACTION_ADD;
    }
  if(strcmp(argv[3], "del")==0){
      action = RTACTION_DEL;
    }

  /* add or del a ipv4 route item */
  if(strcmp(argv[2], "inet")==0){
      fprintf(stderr,"set Ip4 infomation\n");
      return inet_setroute(action, argv+4);
    }
  /* add  or del a ipv6 route item */
  if(strcmp(argv[2], "inet6")==0){
      return inet6_setroute(action, argv+4);
    }
  return -1;
}


void usage()
{
  fprintf(stderr,"%s\n","IPv4 Command: route -A inet add/del -net/-host TARGET netmask "
                        "NETMASK gw GETWAY dev DEVICE mtu MTU\n");
  fprintf(stderr,"%s\n","IPv6 Command: route -A inet6 add/del -net TARGET/PREFIX "
                        "gw GETWAY dev DEVICE mtu MTU\n");
  return;
}

/*
 *  IPv4 add/del route item in route table
 */
int inet_setroute(int action, char **args)
{
  struct rtentry route;  /* route item struct */
  char target[128] = {0};
  char gateway[128] = {0};
  char netmask[128] = {0};

  struct sockaddr_in *addr;

  int skfd;

  /* clear route struct by 0 */
  memset((char *)&route, 0x00, sizeof(route));

  /* default target is net (host)*/
  route.rt_flags = RTF_UP ;

  while(args){
      if(*args == NULL){
          break;
        }

      if(!strcmp(*args, "-net")){/* default is a network target */
          args++;
          strcpy(target, *args);
          addr = (struct sockaddr_in*) &route.rt_dst;
          addr->sin_family = AF_INET;
          addr->sin_addr.s_addr = inet_addr(target);
          args++;
        }else if(!strcmp(*args, "-host")){/* target is a host */
          args++;
          strcpy(target, *args);
          addr = (struct sockaddr_in*) &route.rt_dst;
          addr->sin_family = AF_INET;
          addr->sin_addr.s_addr = inet_addr(target);
          route.rt_flags |= RTF_HOST;
          args++;
        }else if(!strcmp(*args,"netmask")){/* netmask setting */
          args++;
          strcpy(netmask, *args);
          addr = (struct sockaddr_in*) &route.rt_genmask;
          addr->sin_family = AF_INET;
          addr->sin_addr.s_addr = inet_addr(netmask);
          args++;
        }
      if(!strcmp(*args,"gw") || !strcmp(*args, "gateway")){/* gateway setting */
          args++;
          strcpy(gateway, *args);
          addr = (struct sockaddr_in*) &route.rt_gateway;
          addr->sin_family = AF_INET;
          addr->sin_addr.s_addr = inet_addr(gateway);
          route.rt_flags |= RTF_GATEWAY;
          args++;
        }
      if(!strcmp(*args,"device") || !strcmp(*args, "dev")){/* device setting */
          args++;
          route.rt_dev = *args;
          args++;
        }
      if(!strcmp(*args, "mtu")){/* mtu setting */
          args++;
          route.rt_flags |= RTF_MTU;
          route.rt_mtu = atoi(*args);
          args++;
        }
      /* if you have other options, please put them in this place,
          like the options above. */
    }

  /* create a socket */
  skfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(skfd < 0){
      perror("socket");
      return -1;
    }

  /* tell the kernel to accept this route */
  if(action == RTACTION_DEL){/* del a route item */
      if(ioctl(skfd, SIOCDELRT, &route) < 0){
          perror("SIOCDELRT");
          close(skfd);
          return -1;
        }
    }else{/* add a route item */
      if(ioctl(skfd, SIOCADDRT, &route) < 0){
          perror("SIOCADDRT");
          close(skfd);
          return -1;
        }
    }
  (void) close(skfd);
  return 0;
}

int INET6_resolve(char *name, struct sockaddr_in6 *sin6);
int INET6_input(int type, char *bufp, struct sockaddr *sap);
int INET6_getsock(char *bufp, struct sockaddr *sap);

/* IPv6 add/del route item in route table */
/* main part of this function is from net-tools inet6_sr.c file */
int inet6_setroute(int action, char **args)
{
  struct in6_rtmsg rt;          /* ipv6 route struct */
  struct ifreq ifr;             /* interface request struct */
  struct sockaddr_in6 sa6;      /* ipv6 socket address */
  char target[128];
  char gateway[128] = "NONE";
  int metric;
  int prefix_len;               /* network prefix length */
  char *devname = NULL;         /* device name */
  char *cp;
  int mtu = 0;

  int skfd = -1;

  if (*args == NULL ){
      usage();
      return -1;
    }

  args++;
  strcpy(target, *args);

  if (!strcmp(target, "default")){
      prefix_len = 0;
      memset(&sa6, 0, sizeof(sa6));
    } else{
      if((cp = strchr(target, '/'))){
          prefix_len = atol(cp + 1);
          if ((prefix_len < 0) || (prefix_len > 128))
            usage();
          *cp = 0;
        } else{
          prefix_len = 128;
        }
      if (INET6_input(1, target, (struct sockaddr *) &sa6) < 0
          && INET6_input(0, target, (struct sockaddr *) &sa6) < 0){
          return (1);
        }
    }

  memset((char *)&rt,0,sizeof(struct in6_rtmsg));

  memcpy(&rt.rtmsg_dst, sa6.sin6_addr.s6_addr, sizeof(struct in6_addr));

  rt.rtmsg_flags = RTF_UP;
  if (prefix_len == 128)
    rt.rtmsg_flags |= RTF_HOST;
  rt.rtmsg_metric = 1;
  rt.rtmsg_dst_len = prefix_len;

  args++;
  while (*args){
      if (!strcmp(*args, "metric")){
          args++;
          if (!*args || !isdigit(**args)){
              usage();
              return -1;
            }
          metric = atoi(*args);
          rt.rtmsg_metric = metric;
          args++;
          continue;
        }
      if (!strcmp(*args, "gw") || !strcmp(*args, "gateway")){
          args++;
          if (!*args)
            return -1;
          if (rt.rtmsg_flags & RTF_GATEWAY)
            return -1;
          strcpy(gateway, *args);
          if (INET6_input(1, gateway, (struct sockaddr *) &sa6) < 0){
              return -1;
            }
          memcpy(&rt.rtmsg_gateway, sa6.sin6_addr.s6_addr,
                 sizeof(struct in6_addr));
          rt.rtmsg_flags |= RTF_GATEWAY;
          args++;
          continue;
        }
      if (!strcmp(*args, "mod")){
          args++;
          rt.rtmsg_flags |= RTF_MODIFIED;
          continue;
        }
      if (!strcmp(*args, "dyn")){
          args++;
          rt.rtmsg_flags |= RTF_DYNAMIC;
          continue;
        }
      if (!strcmp(*args, "mtu")){
          args++;
          mtu = atoi(*args);
          args++;
          continue;
        }
      if (!strcmp(*args, "device") || !strcmp(*args, "dev")){
          args++;
          if (!*args)
            return -1;
        } else if (args[1])
        return -1;

      devname = *args;
      args++;
    }

  if ((skfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
      perror("socket");
      return -1;
    }

  memset(&ifr, 0, sizeof(ifr));

  if (devname){
      strcpy(ifr.ifr_name, devname);

      if (ioctl(skfd,SIOGIFINDEX,&ifr)<0){
          perror("SIOGIFINDEX");
          return -1;
        }
      rt.rtmsg_ifindex = ifr.ifr_ifindex;
    }

  if (mtu){
      ifr.ifr_mtu = mtu;

      if(ioctl(skfd,SIOCSIFMTU,&ifr)<0){
          perror("SIOCGIFMTU");
          return -1;
        }
    }

  if (action == RTACTION_DEL){
      if (ioctl(skfd, SIOCDELRT, &rt) < 0){
          perror("SIOCDELRT");
          close(skfd);
          return -1;
        }
    }else{
      if (ioctl(skfd, SIOCADDRT, &rt) < 0){
          perror("SIOCADDRT");
          close(skfd);
          return -1;
        }
    }

  (void) close(skfd);
  return (0);
}


int INET6_resolve(char *name, struct sockaddr_in6 *sin6)
{
  struct addrinfo req, *ai;
  int s;

  memset (&req, '\0', sizeof req);
  req.ai_family = AF_INET6;
  if ((s = getaddrinfo(name, NULL, &req, &ai))){
      fprintf(stderr, "getaddrinfo: %s: %d\n", name, s);
      return -1;
    }
  memcpy(sin6, ai->ai_addr, sizeof(struct sockaddr_in6));

  freeaddrinfo(ai);

  return (0);
}
int INET6_getsock(char *bufp, struct sockaddr *sap)
{
  struct sockaddr_in6 *sin6;

  sin6 = (struct sockaddr_in6 *) sap;
  sin6->sin6_family = AF_INET6;
  sin6->sin6_port = 0;

  if (inet_pton(AF_INET6, bufp, sin6->sin6_addr.s6_addr) <= 0)
    return (-1);

  return 16;
}

int INET6_input(int type, char *bufp, struct sockaddr *sap)
{
  switch (type){
    case 1:
      return (INET6_getsock(bufp, sap));
    default:
      return (INET6_resolve(bufp, (struct sockaddr_in6 *) sap));
    }
}
