// iorequest.h 
//	Data structures for managing IO request.  A IO request represents
//	sequential I/O operation 
//	
//  I/OType: Read and Write
//  waitingTime: blocking time to simulater a I/O device of executing time.
//  thread: the thread pointer for the current thread
//  completionTime: waiting time + current time;

#ifndef IOREQUEST_H
#define IOREQUEST_H

#include "callback.h"
#include "thread.h"

enum IOType {READ, WRITE};

class IORequest: public CallBackObj
{
    public:
        IORequest(IOType tp);
        ~IORequest();
        void PrintInfo(bool isFinish);
        Thread* GetThread();
        int GetRealityRunningTime();
        void Sleep();

    
    private:
        IOType type;
        int waitingTime;
        int completionTime;
        Thread* thread;
        void CallBack();
};

#endif

