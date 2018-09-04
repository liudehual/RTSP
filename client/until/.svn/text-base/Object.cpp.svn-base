#include "Object.h"


Object::Object()
{

}
Object::~Object()
{
}

#if 0
signed long long Milliseconds()
{
/*
#if __MacOSX__

#if DEBUG
    OSMutexLocker locker(sLastMillisMutex);
#endif

   UnsignedWide theMicros;
    ::Microseconds(&theMicros);
    SInt64 scalarMicros = theMicros.hi;
    scalarMicros <<= 32;
    scalarMicros += theMicros.lo;
    scalarMicros = ((scalarMicros / 1000) - sInitialMsec) + sMsecSince1970; // convert to msec

#if DEBUG
    static SInt64 sLastMillis = 0;
    //Assert(scalarMicros >= sLastMillis); // currently this fails on dual processor machines
    sLastMillis = scalarMicros;
#endif
    return scalarMicros;
*/
#if defined(__WIN32__) || defined(_WIN32)
    // curTimeMilli = timeGetTime() + ((sLastTimeMilli/ 2^32) * 2^32)
    // using binary & to reduce it to one operation from two
    // sCompareWrap and sWrapTime are constants that are never changed
    // sLastTimeMilli is updated with the curTimeMilli after each call to this function
    signed long long curTimeMilli = (unsigned int) ::timeGetTime();
	#if 0
    if((curTimeMilli - sLastTimeMilli) < 0)
    {
        curTimeMilli += sWrapTime;
    }
    sLastTimeMilli = curTimeMilli;
    
    // For debugging purposes
    //SInt64 tempCurMsec = (curTimeMilli - sInitialMsec) + sMsecSince1970;
    //SInt32 tempCurSec = tempCurMsec / 1000;
    //char buffer[kTimeStrSize];
    //qtss_printf("OS::MilliSeconds current time = %s\n", qtss_ctime(&tempCurSec, buffer, sizeof(buffer)));

    return (curTimeMilli - sInitialMsec) + sMsecSince1970; // convert to application time
	#endif

	return curTimeMilli;
#else
    struct timeval t;
    struct timezone tz;
    int theErr = ::gettimeofday(&t, &tz);

    signed long long curTime;
    curTime = t.tv_sec;
    curTime *= 1000;                // sec -> msec
    curTime += t.tv_usec / 1000;    // usec -> msec

    return curTime;
#endif

}
void SleepBymSec(unsigned int inMsec)
{

#if defined(__WIN32__) || defined(_WIN32)
    ::Sleep(inMsec);
#elif __linux__ ||  __MacOSX__

    if (inMsec == 0)
        return;
        
    signed long long startTime = Milliseconds();
    signed long long timeLeft = inMsec;
    signed long long timeSlept = 0;

    do {
        //qtss_printf("OSThread::SleepBymSec = %qd request sleep=%qd\n",timeSlept, timeLeft);
        timeLeft = inMsec - timeSlept;
        if (timeLeft < 1)
            break;
            
        ::usleep(timeLeft * 1000);

        timeSlept = (Milliseconds() - startTime);
        if (timeSlept < 0) // system time set backwards
            break;
            
    } while (timeSlept < inMsec);

	//qtss_printf("total sleep = %qd request sleep=%lu\n", timeSlept,inMsec);

#elif defined(__osf__) || defined(__hpux__)
    if (inMsec < 1000)
        ::usleep(inMsec * 1000); // useconds must be less than 1,000,000
    else
        ::sleep((inMsec + 500) / 1000); // round to the nearest whole second
#elif defined(__sgi__) 
	struct timespec ts;
	
	ts.tv_sec = 0;
	ts.tv_nsec = inMsec * 1000000;

	nanosleep(&ts, 0);
#else
    ::usleep(inMsec * 1000);
#endif
}
#endif
