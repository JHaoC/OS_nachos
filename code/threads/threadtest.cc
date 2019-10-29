#include "kernel.h"
#include "main.h"
<<<<<<< HEAD
#include "thread.h"
#include "../userprog/train.h"
#include "../userprog/request.h"
#include "simulatorLab3.h"

void newTrain(int id)
{
    printf("Create the train %d \n", id);
    Train *a = new Train(id);
    simulator.appendTrains(kernel->currentThread);
    printf("Sleep the train %d, until all trains setting up \n\n", id);
    kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    //kernel->interrupt->Enable();

    // wake requests which take this train
    while (!a->isFinish())
    {
        int trainState = a->update();
        if (trainState == 3)
        {
            printf("The train %d is running, no passengers can get in or off\n", id);
        }
        else if (trainState == 2)
        {
            a->getDepartureInfo();    
            printf(" no passengers can get off\n");
        }
        else
        {
            int numofRequest = a->getArrivalInfo();
            printf(" no passengers can get in\n");

            printf("awake requests depended on the train \n", id);
            while (!simulator.getonTrainRequestsThread(id)->IsEmpty())
            {
                kernel->interrupt->SetLevel(IntOff);
                kernel->scheduler->ReadyToRun(simulator.RemoveFrontRequestThreadFromTrain(id));
                //kernel->interrupt->Enable();
                //kernel->currentThread->Yield();
            }
            while (simulator.getonTrainRequestsThread(id)->NumInList() < numofRequest)
            {
                kernel->currentThread->Yield();
            }
        }

        //Sleep for next time step
        printf("Sleep the train %d, waiting for next time step \n", id);
        simulator.appendTrains(kernel->currentThread);
        kernel->interrupt->SetLevel(IntOff);
        kernel->currentThread->Sleep(false);
        //kernel->interrupt->Enable();
    }

    printf(" Finish simulation and clean request\n");
    int numofRequest = a->getArrivalInfo();
    printf("awake requests depended on the train \n\n", id);
    while (!simulator.getonTrainRequestsThread(id)->IsEmpty())
    {
        kernel->interrupt->SetLevel(IntOff);
        kernel->scheduler->ReadyToRun(simulator.RemoveFrontRequestThreadFromTrain(id));
    }
    while (simulator.getonTrainRequestsThread(id)->NumInList() < numofRequest)
    {
        kernel->currentThread->Yield();
    }

    //Sleep for next time step
    printf("Simulation summary for the train %d\n", id);
    printf("Total served itinerary is %d\n", a->getServeItinerary());
    printf("Total passengers is %d\n", a->getServePassenger());
    printf("Sleep the train waiting for exit \n\n");
    simulator.appendTrains(kernel->currentThread);
    kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    //kernel->interrupt->Enable();
    printf("exit the train %d thread \n\n", id);
    kernel->currentThread->Finish();
}

void newRequest(int num)
{
    int time = simulator.getTime();
    Request *req = new Request(num, time);
    int trainID = req->selectTrain();

    //Add one on counter
    simulator.AddOneOnCounter();

    // rejected request
    if (trainID == -1)
    {
        printf("finish the Request %d thread\n\n", num);
        kernel->currentThread->Finish();
    }

    //Sleep request and wait for trains
    simulator.appendRequestThreadOnTrain(kernel->currentThread, trainID);
    printf("Sleep the request %d thread, wait the train %d signal \n", num, trainID);
    kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    //kernel->interrupt->Enable();

    while (!req->isfinished(trainID))
    {
        simulator.appendRequestThreadOnTrain(kernel->currentThread, trainID);
        kernel->interrupt->SetLevel(IntOff);
        kernel->currentThread->Sleep(false);
        //kernel->interrupt->Enable();
    }

    printf("finish the Request %d thread\n\n", num);
    kernel->currentThread->Finish();
}

void ThreadTest()
{
    printf("Lab3 a Railway System\n");
    printf("Setting up trains as the schedule\n\n");
    for (int i = 0; i < 5; i++)
    {
        Thread *train = new Thread("train");
        train->Fork((VoidFunctionPtr)newTrain, (void *)i);
        //simulator.appendTrains(train);
        //kernel->currentThread->Yield();
    }
    printf("Waiting for all train Setting up\n");
    while (simulator.getTrainsList()->NumInList() < 5)
    {
        kernel->currentThread->Yield();
    }
    printf("all train Settle done and\n");
    printf("Run simulations from 6:00 am to 10:00 pm \n\n");

    while (simulator.getTime() <= 96)
    {
        // reset counter
        simulator.ResetCounter();

        int timeStep = simulator.getTime();
        int hour = 6 + timeStep / 6;
        int min = timeStep % 6 * 10;
        printf("\n");
        printf("This is the %d th time step which is at %d : %d time\n\n", timeStep, hour, min);

        // while timeStep = 96, skip this part
        if (timeStep != 96)
        {
            // Set up new requests and insert into blockrequest for the selected train
            for (int i = 0; i < 5; i++)
            {
                Thread *request = new Thread("request");
                int num = simulator.getTime() * 5 + i;
                request->Fork((VoidFunctionPtr)newRequest, (void *)num);
            }

            // make sure setting up new requests before awake train
            while (simulator.getCounter() != 5)
            {
                kernel->currentThread->Yield();
            }
        }

        // awake all of train thread
        printf("wake all train \n");
        while (!simulator.getTrainsList()->IsEmpty())
        {
            kernel->interrupt->SetLevel(IntOff);
            kernel->scheduler->ReadyToRun(simulator.getTrainsList()->RemoveFront());
            //kernel->interrupt->Enable();
        }

        //waiting for trains and requests reviews
        printf("Waiting for all train Setting up create \n");
        while (simulator.getTrainsList()->NumInList() < 5)
        {
            kernel->currentThread->Yield();
        }

        simulator.updateTime();
    }

    // wake all of train thread and exit
    printf("wake all train \n");
    while (!simulator.getTrainsList()->IsEmpty())
    {
        kernel->interrupt->SetLevel(IntOff);
        kernel->scheduler->ReadyToRun(simulator.getTrainsList()->RemoveFront());
    }

    int grandReqnum = simulator.getRequestsList(1)->NumInList();
    int rejectReqnum = simulator.getRequestsList(2)->NumInList();
    printf("Simulation summary for the system\n");
    printf("Total # of Requests is %d\n", grandReqnum + rejectReqnum);
    printf("Total # of granded Requests is %d\n\n", grandReqnum);
    kernel->currentThread->Finish();
=======
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

>>>>>>> assignmentFirst
}
