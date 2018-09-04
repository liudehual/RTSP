#include "TimeManager.h"
#include "Until.h"
#include "TimeoutJob.h"
#include "Log.h"

TimeManager::TimeManager():fLastSyncTime(0)
{
}
TimeManager::~TimeManager()
{
}
int TimeManager::addElem(ObjectQueueElem *elem)
{
	timeoutQueue.EnQueue(elem);
	return 1;
}
int TimeManager::Processor()
{

//	fprintf(stderr,SIMPLE_RTSP"TimeManager Processor\n");
	synchronizeAndHandle();
	return 1;
}
int TimeManager::delElem(void *object)
{
   TimeoutJob *theJob=(TimeoutJob *)object;
   timeoutQueue.Remove(theJob->getJobElem());
   return 1;
}
unsigned long long  TimeManager::getCurrentTime() 
{
  struct timeval tvNow;

  gettimeofday(&tvNow, NULL);
  #if 0
  fprintf_debug("tvNow %d\n",((tvNow.tv_sec*1000)+(tvNow.tv_usec/1000)));	
  #endif
  return ((tvNow.tv_sec*1000000)+(tvNow.tv_usec));
}
void TimeManager::synchronizeAndHandle() 
{
  /*get current time*/
 unsigned long long timeNow = getCurrentTime();
  if (timeNow < fLastSyncTime){
	/*time set forward*/
    fLastSyncTime  = timeNow;
    return;
  }
  unsigned long long timeSinceLastSync = 
  						timeNow - fLastSyncTime;
  #if 0
  fprintf_debug("timeSinceLastSync %llu fLastSyncTime %llu timeNow %llu\n",
  				timeSinceLastSync,
  				fLastSyncTime,
  				timeNow
  				);
  #endif
  fLastSyncTime = timeNow;
  #if 0
  return ;
  #endif
  for(ObjectQueueIter it(&timeoutQueue);
  					!(it.IsDone());it.Next()){
		ObjectQueueElem *elem=it.GetCurrent();
		TimeoutJob *theJob=(TimeoutJob *)(elem->GetEnclosingObject());
		theJob->addCurrentTimeout(timeSinceLastSync);
		if(theJob->getCurrentTimeout()>=theJob->getTimeout()){
			if(!theJob->Processor()){
				/*do something*/
				timeoutQueue.Remove(elem);
				delete theJob;
			}
			theJob->refreshCurrentTimeout();
		}
  }
}


