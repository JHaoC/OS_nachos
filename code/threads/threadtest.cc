#include "kernel.h"
#include "main.h"
#include "iorequest.h"
#include "../lib/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


int IOOperation(IOType io)
{
    IORequest* res = new IORequest(io);
    res->PrintInfo(false);
    res->Sleep();
    res->PrintInfo(true);
    return res->GetRealityRunningTime();
}

int CPUOperation(int times)
{
    int start_time = kernel->stats->totalTicks;
    printf("This is CPU bound request, should cost %d time on the CPU at the thread: %s.\n", times * 10, kernel->currentThread->getName());
    printf("Current time: %d, should be finished after at %d.\n", start_time, start_time + times * 10);
    printf("Continue runing until Timer Interrupts or I/O Interrupts coming.\n\n");
    int t = 0;
    while (t != times)
    {
        t++;
        kernel->interrupt->OneTick(); // in the system mode, it add totalTickes 10 in an OneTick()
        //printf("CPU run %d times \n", t);
        //printf("Current time: %d\n", kernel->stats->totalTicks);
    }
    printf("This is CPU bound request is running at the thread: %s, could finish at least at %d.\n", kernel->currentThread->getName(), start_time + times * 10);
    printf("It finishes and the current time: %d\n", kernel->stats->totalTicks);
    printf("It costs %d in reality\n\n", kernel->stats->totalTicks - start_time);
    return kernel->stats->totalTicks - start_time;
}

void MixThread(int which)
{
    printf("Set up a thread: %s and its weight is %d\n", kernel->currentThread->getName(), kernel->currentThread->GetWeight());
    printf("The Thread is a Mix-request. It has one read request and one write request and a CPU bound expected costing time %d.\n\n", which * 10);
    int readTime = IOOperation(READ);
    int CPUTime = CPUOperation(which);
    int writeTime = IOOperation(WRITE);
    int executeTime = readTime + CPUTime + writeTime;
    printf("Summary The Thread: %s\n", kernel->currentThread->getName());
    printf("The Thread is a Mix-request. It has one read request and one write request and a CPU bound expected costing time %d.\n", which * 10);
    printf("It takes %d, %d and %d for reading, CPU and writing\n", readTime, CPUTime, writeTime);
    printf("Total cost in reality, including Block for IO and context switch and waiting in the ready queue, is %d.\n", executeTime);
    printf("Exit Thread: %s.\n\n", kernel->currentThread->getName());
    kernel->currentThread->Finish();
}

void CPUPureThread(int which)
{
    printf("Set up a thread: %s and its weight is %d\n", kernel->currentThread->getName(), kernel->currentThread->GetWeight());
    printf("The Thread is a Pure CPU bound.It costs CPU running time %d.\n\n", which * 10);
    int executeTime = CPUOperation(which);
    printf("Summary The Thread: %s\n", kernel->currentThread->getName());
    printf("The Thread is a Pure CPU bound.It costs CPU running time %d.\n", which * 10);
    printf("Total cost in reality, including context switch and waiting in the ready queue, is %d.\n", executeTime);
    printf("Exit Thread: %s\n\n", kernel->currentThread->getName());
    kernel->currentThread->Finish();
}

void IOThread(int which)
{
    printf("Set up a thread: %s and its weight is %d\n", kernel->currentThread->getName(), kernel->currentThread->GetWeight());
    printf("The Thread is Pure I/O requests. It has %d Read and Write requests and totol requests are %d.\n\n", which, which * 2);
    int executeTime(0);
    while (which != 0)
    {
        executeTime += IOOperation(READ);
        executeTime += IOOperation(WRITE);
        which--;
    }
    printf("Summary The Thread: %s\n", kernel->currentThread->getName());
    printf("The Thread is Pure I/O requests. It has %d Read and Write requests and totol requests are %d.\n", which, which * 2);
    printf("Total cost in reality, including blocking, context switch and waiting in the ready queue, is %d.\n", executeTime);
    printf("Exit Thread: %s\n\n", kernel->currentThread->getName());
    kernel->currentThread->Finish();
}

void ThreadTest()
{
    // Create 25 thread. The first 5th are pure CPU-Bound request, The rest following the order one Pure CPU, 3 Mix and one Pure I/O
    List<char*> * names = new List<char*>();

    names->Append("Thread 1");
    names->Append("Thread 2");
    names->Append("Thread 3");
    names->Append("Thread 4");
    names->Append("Thread 5");
    names->Append("Thread 6");
    names->Append("Thread 7");
    names->Append("Thread 8");
    names->Append("Thread 9");
    names->Append("Thread 10");
    names->Append("Thread 11");
    names->Append("Thread 12");
    names->Append("Thread 13");
    names->Append("Thread 14");
    names->Append("Thread 15");
    names->Append("Thread 16");
    names->Append("Thread 17");
    names->Append("Thread 18");
    names->Append("Thread 19");
    names->Append("Thread 20");
    names->Append("Thread 21");
    names->Append("Thread 22");
    names->Append("Thread 23");
    names->Append("Thread 24");
    names->Append("Thread 25");
    
    int num = 0; 
    for (ListIterator<char*> name(names); !name.IsDone(); name.Next())
    {
        Thread* thread = new Thread(name.Item());
        if( num%5 == 0)
        {
            int times = (rand()%5+1)*5000;
            thread->SetWeight(4);
            thread->Fork((VoidFunctionPtr)CPUPureThread, (void *)times);
        }
        else if (num%5 == 4)
        {
            int times = (rand()%4+1);
            thread->SetWeight(1);
            thread->Fork((VoidFunctionPtr)IOThread, (void *)times);
        }
        else
        {
            int times = (rand()%3+1)*5000;
            if(num%2 == 0 ){
                 thread->SetWeight(2);
            }
            else{ thread->SetWeight(3);}
            thread->Fork((VoidFunctionPtr)MixThread, (void *)times);
        }
        num++;
        //kernel->currentThread->Yield();
    }

}
