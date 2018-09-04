#include "myUntil.h"

OSMutex tmpMutex;
FILE *myfp=NULL;

char*  myDate(void)
{
    static char buf[200];
#if !defined(_WIN32_WCE)
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT ", gmtime(&tt));
#else
    // WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
    // so generate the "Date:" header a different, WinCE-specific way.
    // (Thanks to Pierre l'Hussiez for this code)
    // RSF: But where is the "Date: " string?  This code doesn't look quite right...
    SYSTEMTIME SystemTime;
    GetSystemTime(&SystemTime);
    WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
    WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
    WCHAR inBuf[200];
    DWORD locale = LOCALE_NEUTRAL;

    int ret = GetDateFormat(locale, 0, &SystemTime,
    (LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof inBuf);
    inBuf[ret - 1] = ' ';
    ret = GetTimeFormat(locale, 0, &SystemTime,
    (LPTSTR)timeFormat,
    (LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
    wcstombs(buf, inBuf, wcslen(inBuf));
#endif
    return buf;
}
void myLog(const char *fmt,  ...)
{
   static OSMutex logMutex;
   static FILE *logFp=NULL;
   if(logFp==NULL){
   }
}
long our_random() {
#if defined(__WIN32__) || defined(_WIN32)
  return rand();
#else
  return random();
#endif
}
unsigned int  _our_random32()
{
  // Return a 32-bit random number.
  // Because "our_random()" returns a 31-bit random number, we call it a second
  // time, to generate the high bit:
  long random1 = our_random();
  long random2 = our_random();
  return (unsigned int)((random2<<31) | random1);
}