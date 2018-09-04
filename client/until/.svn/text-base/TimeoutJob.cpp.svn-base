#include "TimeoutJob.h"
#include "Engine.h"
#include "Log.h"
TimeoutJob::TimeoutJob(RTSPClient *rtspClient,unsigned long long mSec):Job(rtspClient)
{
	timeout=mSec*1000; //∫¡√Î◊™ªª≥…Œ¢√Ó
	currentTimeout=0;
}
TimeoutJob::~TimeoutJob()
{
}
void TimeoutJob::setTimeout(unsigned long long mSec)
{
	timeout=mSec*1000;
}
unsigned long long TimeoutJob::getTimeout()
{
	return timeout;
}
void TimeoutJob::addCurrentTimeout(unsigned long long t)
{
	currentTimeout+=t;
}
unsigned long long TimeoutJob::getCurrentTimeout()
{
	return currentTimeout;
}
void TimeoutJob::refreshCurrentTimeout()
{
	currentTimeout=0;
}