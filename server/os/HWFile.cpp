#include"HWFile.h"
#include <fcntl.h>
EventThread * HWFile::HWFileThread = NULL;
HWFile::HWFile(char * path) : EventContext(EventContext::kInvalidFileDesc,HWFileThread)
{
    if(path != NULL)
    {
        strcpy(HWNodePath,path);
    }
}
HWFile::HWFile(): EventContext(EventContext::kInvalidFileDesc,HWFileThread)
{
    
}
OS_Error HWFile::Open(int type)
{
    if(fFileDesc == EventContext::kInvalidFileDesc)
    {
        if(type == kNOBLOCK)
        {
            fFileDesc = open(HWNodePath,O_RDWR | O_NONBLOCK);
			#ifdef PRINT
            printf("hwfile open %s err: %d\n",HWNodePath,fFileDesc);
			#endif
        }
        if(type == kBLOCK)
        {
            
        }
    }
    
        
    return 0;
}
int HWFile::GetFD()
{
    return fFileDesc;
}