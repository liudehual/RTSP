#ifndef _RTSP_H
#define _RTSP_H

#define NET_DEBUG


extern "C"{
           
    /*
        函数:startChannelsByNum
        功能:按个数启动通道
        参数:启动通道数量
        返回值: 0 失败 1 成功
    */
    int startChannelsByNum(int chnNumbers);

	/*启动监听*/
	int startListener(unsigned short port);

   /*
        函数:writeDataToChannel
        功能:向通道中写入数据
        参数:channel 通道号
             buf 数据
             len 数据长度
        返回值: 0 未写入 1 成功 -1 出错
        "注:只写入一次sps,pps信息即可，频繁写入会引起视频卡顿"
    */
   int writeDataToChannel(int channel,unsigned char*buf,unsigned len);
}
#endif
