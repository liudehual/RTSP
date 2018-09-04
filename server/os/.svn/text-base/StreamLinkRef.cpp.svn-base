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
    File:       StreamLinkRef.cpp

    Contains:   Implementation of StreamLinkRef class. 
                    

*/

#include "StreamLinkRef.h"

#include <errno.h>


StreamLinkRefTable * Clientdev::ftable = new StreamLinkRefTable;

UInt32  StreamLinkRefTableUtils::HashString(UInt32 channel,UInt32 instream)
{
    return channel * 3 + instream;    
}

OS_Error StreamLinkRefTable::Register(StreamLinkRef* inRef)
{
#ifdef PRINT
    printf("Register\n");
#endif
    Assert(inRef != NULL);
    if (inRef == NULL)
        return EPERM;
#if DEBUG
    Assert(!inRef->fInATable);
#endif
    Assert(inRef->fRefCount == 0);
    //Assert(inRef->fString.Ptr != NULL);
    //Assert(inRef->fString.Len != 0);
#ifdef PRINT
	printf("OSMutexLocker\n");
#endif
    OSMutexLocker locker(&fMutex);
   // if (inRef->fString.Ptr == NULL || inRef->fString.Len == 0)
   // {   //printf("StreamLinkRefTable::Register inRef is invalid \n");
   //     return EPERM;
   // }
    
    // Check for a duplicate. In this function, if there is a duplicate,
    // return an error, don't resolve the duplicate
	#ifdef PRINT
	printf("StreamLinkRefKey key\n");
	#endif
    StreamLinkRefKey key(inRef->fchannel,inRef->fstream,inRef->faddr);
    StreamLinkRef* duplicateRef = fTable.Map(&key);
	#ifdef PRINT
   	printf("StreamLinkRef* duplicateRef\n");
	#endif
    if (duplicateRef != NULL)
        return EPERM;
        
    // There is no duplicate, so add this ref into the table
#if DEBUG
    inRef->fInATable = true;
#endif
    fTable.Add(inRef);
    return OS_NoErr;
}


StreamLinkRef* StreamLinkRefTable::RegisterOrResolve(StreamLinkRef* inRef)
{
    Assert(inRef != NULL);
#if DEBUG
    Assert(!inRef->fInATable);
#endif
    Assert(inRef->fRefCount == 0);
    
    OSMutexLocker locker(&fMutex);

    // Check for a duplicate. If there is one, resolve it and return it to the caller
    StreamLinkRef* duplicateRef = this->Resolve(inRef->fchannel,inRef->fstream,inRef->faddr);
    if (duplicateRef != NULL)
        return duplicateRef;

    // There is no duplicate, so add this ref into the table
#if DEBUG
    inRef->fInATable = true;
#endif
    fTable.Add(inRef);
    return NULL;
}

void StreamLinkRefTable::UnRegister(StreamLinkRef* ref, UInt32 refCount)
{
    Assert(ref != NULL);
    OSMutexLocker locker(&fMutex);

    //make sure that no one else is using the object
    while (ref->fRefCount > refCount)
        ref->fCond.Wait(&fMutex);
    
#if DEBUG
    StreamLinkRefKey key(&ref->fchannel,&ref->fstream,&ref->faddr);
    if (ref->fInATable)
        Assert(fTable.Map(&key) != NULL);
    ref->fInATable = false;
#endif
    
    //ok, we now definitely have no one else using this object, so
    //remove it from the table
    fTable.Remove(ref);
}

Bool16 StreamLinkRefTable::TryUnRegister(StreamLinkRef* ref, UInt32 refCount)
{
    OSMutexLocker locker(&fMutex);
    if (ref->fRefCount > refCount)
        return false;
    
    // At this point, this is guarenteed not to block, because
    // we've already checked that the refCount is low.
    this->UnRegister(ref, refCount);
    return true;
}


StreamLinkRef*  StreamLinkRefTable::Resolve(UInt32 channel,UInt32 stream,UInt32 addr)
{
    //Assert(inUniqueID != NULL);
    StreamLinkRefKey key(channel,stream,addr);

    //this must be done atomically wrt the table
    OSMutexLocker locker(&fMutex);
    StreamLinkRef* ref = fTable.Map(&key);
    if (ref != NULL)
    {
        ref->fRefCount++;
        Assert(ref->fRefCount > 0);
    }
    return ref;
}

void    StreamLinkRefTable::Release(StreamLinkRef* ref)
{
    Assert(ref != NULL);
    OSMutexLocker locker(&fMutex);
    ref->fRefCount--;
    // fRefCount is a UInt32  and QTSS should never run into
    // a ref greater than 16 * 64K, so this assert just checks to
    // be sure that we have not decremented the ref less than zero.
    Assert( ref->fRefCount < 1048576L );
    //make sure to wakeup anyone who may be waiting for this resource to be released
    ref->fCond.Signal();
}

void    StreamLinkRefTable::Swap(StreamLinkRef* newRef)
{
    Assert(newRef != NULL);
    OSMutexLocker locker(&fMutex);
    
    StreamLinkRefKey key(newRef->fchannel,newRef->fstream,newRef->faddr);
    StreamLinkRef* oldRef = fTable.Map(&key);
    if (oldRef != NULL)
    {
        fTable.Remove(oldRef);
        fTable.Add(newRef);
#if DEBUG
        newRef->fInATable = true;
        oldRef->fInATable = false;
        oldRef->fSwapCalled = true;
#endif
    }
    else
        Assert(0);
}


