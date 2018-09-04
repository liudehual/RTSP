#include"ev_epoll.h"

/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */
/*
    File:       ev.cpp

    Contains:   POSIX select implementation of MacOS X event queue functions.


    

*/
#include "PlatformHeader.h"
#if !MACOSXEVENTQUEUE

#define EV_DEBUGGING 0 //Enables a lot of printfs

#if SET_SELECT_SIZE
	#ifndef FD_SETSIZE
		#define FD_SETSIZE SET_SELECT_SIZE
	#endif 
#endif

    #include <sys/time.h>
    #include <sys/types.h>

#ifndef __MACOS__
#ifndef __hpux__
    #include <sys/select.h>
#endif
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/epoll.h>

#include "ev.h"
#include "OS.h"
#include "OSHeaders.h"
#include "MyAssert.h"
#include "OSThread.h"
#include "OSMutex.h"

static fd_set   sReadSet;
static fd_set   sWriteSet;
static fd_set   sReturnedReadSet;
static fd_set   sReturnedWriteSet;
static void**   sCookieArray = NULL;
static int*     sFDsToCloseArray = NULL;
static int sPipes[2];

static int sCurrentFDPos = 0;
static int sMaxFDPos = 0;
static bool sInReadSet = true;
static int sNumFDsBackFromSelect = 0;
static UInt32 sNumFDsProcessed = 0;
static OSMutex sMaxFDPosMutex;


static bool selecthasdata();
static int constructeventreq(struct eventreq* req, int fd, int event);


struct epoll_event ev, events[1000];
static int epfd ;
static int nextevent = 0 ;
static int totalevent = 0 ;

void select_startevents_epoll()
{

    epfd = epoll_create(1000);
    if(epfd == -1)
    {
        printf("epoll create err: %d\n",errno);
    }
    sCookieArray = new void*[8192 * 8];
    ::memset(sCookieArray, 0, sizeof(void *) * 8192 * 8);
}

int select_removeevent_epoll(struct eventreq *req,int which)
{

    {
        //Manipulating sMaxFDPos is not pre-emptive safe, so we have to wrap it in a mutex
        //I believe this is the only variable that is not preemptive safe....
        OSMutexLocker locker(&sMaxFDPosMutex);
        //sCookieArray[which] = NULL; // Clear out the cookie
        if(!req->active)
        {
           struct epoll_event ev;
           ev.data.fd = req->er_handle;
           ev.events= 0;
           
           epoll_ctl(epfd,EPOLL_CTL_MOD,ev.data.fd,&ev);
            return 0;
        }
        
        if(which & EV_RE)
        {
            req->active &= ~EPOLLIN;
        }
        if(which & EV_WR)
        {
             req->active &= ~EPOLLOUT;
        }
        
           struct epoll_event ev;
           ev.data.fd = req->er_handle;
           ev.events= req->active;
           
           epoll_ctl(epfd,EPOLL_CTL_DEL,ev.data.fd,&ev);
    }

    return 0;
}

int select_watchevent_epoll(struct eventreq *req, int which)
{
    return select_modwatch_epoll(req, which);
}

int select_modwatch_epoll(struct eventreq *req, int which)
{

    
    {
        //Manipulating sMaxFDPos is not pre-emptive safe, so we have to wrap it in a mutex
        //I believe this is the only variable that is not preemptive safe....
        OSMutexLocker locker(&sMaxFDPosMutex);
        int op = 0;
        if(req->active)
        {
            op = EPOLL_CTL_MOD;
            
        }
        else
        {
            op = EPOLL_CTL_ADD;
            req->active = EPOLLET;
        }
        //Add or remove this fd from the specified sets
        if (which & EV_RE)
        {
           //printf("add epoll read event\n");
           req->active |= EPOLLIN;
           
           struct epoll_event ev;
           ev.data.fd = req->er_handle;
           ev.events= req->active;
           
           epoll_ctl(epfd,op,ev.data.fd,&ev);
        }
        if (which & EV_WR)
        {
           req->active |= EPOLLOUT;
           
           struct epoll_event ev;
           ev.data.fd = req->er_handle;
           ev.events= req->active;
           
           epoll_ctl(epfd,op,ev.data.fd,&ev);
        }
        
        sCookieArray[req->er_handle] = req->er_data;

    }

    return 0;
}

int constructeventreq(struct eventreq* req, int fd, int event)
{
    //Assert(fd < (int)(sizeof(fd_set) * 8));
    if (fd >=(int)(8192 * 8) )
    {
        #if EV_DEBUGGING
                qtss_printf("constructeventreq: invalid fd=%d\n", fd);
        #endif
        return 0;
    }        
    req->er_handle = fd;
    req->er_eventbits = event;
    req->er_data = sCookieArray[fd];

    return 0;
}

int select_waitevent_epoll(struct eventreq *req, void* /*onlyForMacOSX*/)
{
   // printf("nextevent: %d totalevent: %d\n",nextevent,totalevent);
    if(nextevent < totalevent)
    {
        if(events[nextevent].events & EPOLLIN)
        {
          
           constructeventreq(req,events[nextevent].data.fd,EV_RE);
        }

        nextevent++;
        return 0;
    }
    
    nextevent = 0;
    totalevent = epoll_wait(epfd,events,1000,5000);
    //printf("totalevent : %d  \n",totalevent);
    return EINTR;
   // return totalevent;
}
bool selecthasdata()
{
    if (sNumFDsBackFromSelect < 0)
    {
        int err=OSThread::GetErrno();
        
#if EV_DEBUGGING
        if (err == ENOENT) 
        {
             qtss_printf("selectHasdata: found error ENOENT==2 \n");
        }
#endif

        if ( 
#if __solaris__
            err == ENOENT || // this happens on Solaris when an HTTP fd is closed
#endif      
            err == EBADF || //this might happen if a fd is closed right before calling select
            err == EINTR 
           ) // this might happen if select gets interrupted
             return false;
        return true;//if there is an error from select, we want to make sure and return to the caller
    }
        
    if (sNumFDsBackFromSelect == 0)
        return false;//if select returns 0, we've simply timed out, so recall select
    
    if (FD_ISSET(sPipes[0], &sReturnedReadSet))
    {
#if EV_DEBUGGING
        qtss_printf("selecthasdata: Got some data on the pipe fd\n");
#endif
        //we've gotten data on the pipe file descriptor. Clear the data.
        // increasing the select buffer fixes a hanging problem when the Darwin server is under heavy load
        // CISCO contribution
        char theBuffer[4096]; 
        (void)::read(sPipes[0], &theBuffer[0], 4096);

        FD_CLR(sPipes[0], &sReturnedReadSet);
        sNumFDsBackFromSelect--;
        
        {
            //Check the fds to close array, and if there are any in it, close those descriptors
            OSMutexLocker locker(&sMaxFDPosMutex);
            for (UInt32 theIndex = 0; ((sFDsToCloseArray[theIndex] != -1) && (theIndex < 8192 * 8)); theIndex++)
            {
                (void)::close(sFDsToCloseArray[theIndex]);
                sFDsToCloseArray[theIndex] = -1;
            }
        }
    }
    Assert(!FD_ISSET(sPipes[0], &sReturnedWriteSet));
    
    if (sNumFDsBackFromSelect == 0)
        return false;//if the pipe file descriptor is the ONLY data we've gotten, recall select
    else
        return true;//we've gotten a real event, return that to the caller
}

#endif //!MACOSXEVENTQUEUE


