
#include "ObjectQueue.h"


ObjectQueue::ObjectQueue() : fLength(0)
{
    fSentinel.fNext = &fSentinel;
    fSentinel.fPrev = &fSentinel;
}

void ObjectQueue::EnQueue(ObjectQueueElem* elem)
{
    if (elem->fQueue == this){
		return;
    }
    elem->fNext = fSentinel.fNext;
    elem->fPrev = &fSentinel;
    elem->fQueue = this;
    fSentinel.fNext->fPrev = elem;
    fSentinel.fNext = elem;
    fLength++;
}

ObjectQueueElem* ObjectQueue::DeQueue()
{
    if (fLength > 0){
        ObjectQueueElem* elem = fSentinel.fPrev;
        elem->fPrev->fNext = &fSentinel;
        fSentinel.fPrev = elem->fPrev;
        elem->fQueue = NULL;
        fLength--;
        return elem;
    }
    else
        return NULL;
}

void ObjectQueue::Remove(ObjectQueueElem* elem)
{
    if (elem->fQueue == this)
    {
        elem->fNext->fPrev = elem->fPrev;
        elem->fPrev->fNext = elem->fNext;
        elem->fQueue = NULL;
        fLength--;
    }
}

#if ObjectQueueTESTING
Bool16 ObjectQueue::Test()
{
    ObjectQueue theVictim;
    void *x = (void*)1;
    ObjectQueueElem theElem1(x);
    x = (void*)2;
    ObjectQueueElem theElem2(x);
    x = (void*)3;
    ObjectQueueElem theElem3(x);
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem1)
        return false;
    
    ObjectQueueElem* theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;
        
    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    if (theVictim.GetHead() != &theElem3)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem3)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);
    
    ObjectQueueIter theIterVictim(&theVictim);
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem3)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem2)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem1)
        return false;
    theIterVictim.Next();
    if (!theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != NULL)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem3);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    return true;
}   
#endif


void ObjectQueueIter::Next()
{
    if (fCurrentElemP == fQueueP->GetTail())
        fCurrentElemP = NULL;
    else
        fCurrentElemP = fCurrentElemP->Prev();
}
