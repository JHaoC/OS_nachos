#include "kernel.h"
#include "main.h"
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
}
