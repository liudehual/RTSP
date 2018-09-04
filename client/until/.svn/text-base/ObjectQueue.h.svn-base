/*


*/
#ifndef __OBJECT_QUEUE_H__
#define __OBJECT_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

#define ObjectQueueTESTING 0

class ObjectQueue;

class ObjectQueueElem {
    public:
        ObjectQueueElem(void* enclosingObject = NULL) : fNext(NULL), fPrev(NULL), fQueue(NULL),
                                                    fEnclosingObject(enclosingObject) {}
        virtual ~ObjectQueueElem() {}

        unsigned short IsMember(const ObjectQueue& queue) { return (&queue == fQueue); }
        unsigned short IsMemberOfAnyQueue()     { return fQueue != NULL; }
        void* GetEnclosingObject()  { return fEnclosingObject; }
        void SetEnclosingObject(void* obj) { fEnclosingObject = obj; }

        ObjectQueueElem* Next() { return fNext; }
        ObjectQueueElem* Prev() { return fPrev; }
        ObjectQueue* InQueue()  { return fQueue; }
        inline void Remove();

    private:

        ObjectQueueElem*    fNext;
        ObjectQueueElem*    fPrev;
        ObjectQueue *       fQueue;
        void*           fEnclosingObject;

        friend class    ObjectQueue;
};

class ObjectQueue {
    public:
        ObjectQueue();
        ~ObjectQueue() {}

        void            EnQueue(ObjectQueueElem* object);
        ObjectQueueElem*    DeQueue();

        ObjectQueueElem*    GetHead() { if (fLength > 0) return fSentinel.fPrev; return NULL; }
        ObjectQueueElem*    GetTail() { if (fLength > 0) return fSentinel.fNext; return NULL; }
        unsigned int          GetLength() { return fLength; }
        
        void            Remove(ObjectQueueElem* object);

#if ObjectQueueTESTING
        static unsigned short       Test();
#endif

    protected:

        ObjectQueueElem     fSentinel;
        unsigned int          fLength;
};

class ObjectQueueIter
{
    public:
        ObjectQueueIter(ObjectQueue* inQueue) : fQueueP(inQueue), fCurrentElemP(inQueue->GetHead()) {}
        ObjectQueueIter(ObjectQueue* inQueue, ObjectQueueElem* startElemP ) : fQueueP(inQueue)
            {
                if ( startElemP )
                {                   fCurrentElemP = startElemP;
                
                }
                else
                    fCurrentElemP = NULL;
            }
        ~ObjectQueueIter() {}
        
        void            Reset() { fCurrentElemP = fQueueP->GetHead(); }
        
        ObjectQueueElem*    GetCurrent() { return fCurrentElemP; }
        void            Next();
        
        unsigned short          IsDone() { return fCurrentElemP == NULL; }
        
    private:
    
        ObjectQueue*        fQueueP;
        ObjectQueueElem*    fCurrentElemP;
};
void    ObjectQueueElem::Remove()
{
    if (fQueue != NULL)
        fQueue->Remove(this);
}
#endif //_ObjectQueue_H_
