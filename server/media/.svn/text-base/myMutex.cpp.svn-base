#include "myMutex.h"
OSMutex MyMutex::mutex;
MyMutex::MyMutex()
{
    MyMutex::mutex.Lock();
}
MyMutex::~MyMutex()
{
    MyMutex::mutex.Unlock();
}
