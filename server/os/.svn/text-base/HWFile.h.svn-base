#ifndef HWFILE_H
#define HWFILE_H
#include"EventContext.h"

class HWFile : public EventContext
{
    public:
        HWFile(char * path);
        HWFile();
        virtual ~HWFile(){}
        

        
        static void Initialize() { HWFileThread = new EventThread(); HWFileThread->SetThreadFor(1);
		#ifdef PRINT 
			printf("hw thread: %x\n",HWFileThread);
		#endif
		}
        static void StartThread() {HWFileThread->Start();}

        OS_Error Open(int type);
        int GetFD();


        enum
        {
            kNOBLOCK = 1,
            kBLOCK
        };

    private:
        static EventThread * HWFileThread;
        char HWNodePath[128];
};

#endif
