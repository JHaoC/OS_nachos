// iorequest.cc
//	Routines to manage iorequest.  These are the main operations:
//
//	PrintInfo -- print info of request to show
//	GetThread -- Return the Thread which execute I/O operation
//  GetWaitingTime -- Return waiting time
//  Sleep -- Sleep the thread waiting callback
//  Exit -- exit the thread
//  Callback -- wake thread

#include "iorequest.h"
#include "thread.h"
#include <stdio.h>
#include "kernel.h"
#include "main.h"

//----------------------------------------------------------------------
// IORequest::IORequest(IOType)
// 	Initialize a IORequest
//  Read blocking time 2500 - 10000
//  Write blocking time 250 - 1000
//  Satisfied Write waiting time << Read waiting time
//  Create a I/O intertupte and insert to the pending interrupt list
//  waiting for call back
//----------------------------------------------------------------------

IORequest::IORequest(IOType tp)
{
    type = tp;
    thread = kernel->currentThread;
    srand(kernel->stats->totalTicks); // set rand seed
    if (tp == READ)
    {
        waitingTime = (rand() % 4 + 1) * 2500;
        completionTime = kernel->stats->totalTicks + waitingTime;
        //kernel->interrupt->Schedule(this,waitingTime,ConsoleReadInt);
    }
    else
    {
        waitingTime = (rand() % 4 + 1) * 250;
        completionTime = kernel->stats->totalTicks + waitingTime;
        //kernel->interrupt->Schedule(this,waitingTime,ConsoleWriteInt);
    }
}

//----------------------------------------------------------------------
// IORequest::~IORequest()
// 	destructor IORequest
//----------------------------------------------------------------------

IORequest::~IORequest()
{
    if (thread != NULL)
    {
        thread = NULL;
    }
}

//----------------------------------------------------------------------
// IORequest::PrintInfo(bool isFinish)
// 	Print Info of the request
//----------------------------------------------------------------------

void IORequest::PrintInfo(bool isFinish)
{
    if (!isFinish)
    {
        if (type == READ)
        {
            printf("This is I/O Read request, waiting time is %d, at the thread: %s\n", waitingTime, thread->getName());
            printf("Current time: %d, Should be callback at %d.\n", kernel->stats->totalTicks, completionTime);
            printf("Block the current thread waiting for I/O Interrupt.\n\n");
        }
        else
        {
            printf("This is I/O Write request, waiting time is %d, at the thread: %s\n", waitingTime, thread->getName());
            printf("Current time: %d, Should be callback at %d.\n", kernel->stats->totalTicks, completionTime);
            printf("Block the current thread waiting for I/O Interrupt.\n\n");
        }
    }
    else
    {
        if (type == READ)
        {
            printf("This I/O Read request is running at the thread: %s, expected finished at %d \n", thread->getName(), completionTime);
            printf("It finishes I/O request, the current time: %d.\n", kernel->stats->totalTicks);
            printf("It costs %d in reality\n\n", GetRealityRunningTime());
        }
        else
        {
            printf("This I/O Write request is running at the thread: %s, expected finished at %d \n", thread->getName(), completionTime);
            printf("It finishes and the current time: %d.\n", kernel->stats->totalTicks);
            printf("It costs %d in reality\n\n", GetRealityRunningTime());
        }
    }
}
//----------------------------------------------------------------------
// IORequest::GetThread()
// 	Return thread
//----------------------------------------------------------------------

Thread *IORequest::GetThread()
{
    return thread;
}

//----------------------------------------------------------------------
// IORequest::GetRealityRunningTime()
// 	Return waittime
//----------------------------------------------------------------------

int IORequest::GetRealityRunningTime()
{
    return kernel->stats->totalTicks - completionTime + waitingTime;
}

//----------------------------------------------------------------------
// IORequest::Sleep()
// 	sleep thread
//----------------------------------------------------------------------

void IORequest::Sleep()
{
    if (type == READ)
    {
        //waitingTime = (rand()%100+100)*100;
        kernel->interrupt->Schedule(this, waitingTime, ConsoleReadInt);
    }
    else
    {
        //waitingTime = 100*(rand()%50+10);
        kernel->interrupt->Schedule(this, waitingTime, ConsoleWriteInt);
    }
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    (void) kernel->interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// IORequest::CallBack
//	Software interrupt handler for the I/O device.
//  It will awake the thread which contains the I/O operation and
//
//----------------------------------------------------------------------

void IORequest::CallBack()
{
    printf("I/O interrupt \n");
    if (type == READ)
    {
        printf("The Read Interrupt is from the Thread: %s, expected awake at %d and Current time : %d.\n", thread->getName(), completionTime, kernel->stats->totalTicks);
        printf("Put the thread into Ready queue.\n\n");
    }
    else
    {
        printf("The Write Interrupt is from the Thread: %s, expected awake at %d and Current time : %d.\n", thread->getName(), completionTime, kernel->stats->totalTicks);
        printf("Put the thread into Ready queue.\n\n");
    }
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->scheduler->ReadyToRun(thread);
    (void) kernel->interrupt->SetLevel(oldLevel);
}
