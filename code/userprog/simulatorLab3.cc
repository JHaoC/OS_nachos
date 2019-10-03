#include "simulatorLab3.h"
#include "train.h"
#include "route.h"
#include "../lib/bitmap.h"
#include "../lib/list.h"
#include <map>

//----------------------------------------------------------------------
// SimulatorLab3::SimulatorLab3()
// 	Init simulator
//----------------------------------------------------------------------

SimulatorLab3::SimulatorLab3()
{
    printf("Create global object for the simulation \n");

    businessSeats = new map<int, map<int, Bitmap *>>();
    couchSeats = new map<int, map<int, Bitmap *>>();
    routes = new map<int, Route *>();
    grandedRequest = new List<Request*>();
    refusedRequest = new List<Request*>();
    onTrainRequestThread = new map<int, List<Thread*>*>();
    onTrainRequest = new map<int, List<Request*>*>();
    blockTrain = new List<Thread*>();
    time = 0;
    newRequestCounter =0;
}

//----------------------------------------------------------------------
// SimulatorLab3::~SimulatorLab3()
// 	Destructor a SimulatorLab3
//----------------------------------------------------------------------

SimulatorLab3::~SimulatorLab3()
{
    for (auto iter = this->businessSeats->begin(); iter != this->businessSeats->end(); iter++)
    {
        for (auto seats = iter->second.begin(); seats != iter->second.end(); seats++)
        {
            delete seats->second;
        }
    }
    delete businessSeats;
    for (auto iter = this->couchSeats->begin(); iter != this->couchSeats->end(); iter++)
    {
        for (auto seats = iter->second.begin(); seats != iter->second.end(); seats++)
        {
            delete seats->second;
        }
    }
    delete couchSeats;
    for (auto iter = this->routes->begin(); iter != this->routes->end(); iter++)
    {
        delete iter->second;
    }
    delete routes;
    for (ListIterator<Request *> iter(grandedRequest); !iter.IsDone(); iter.Next())
    {
        delete iter.Item();
    }
    delete grandedRequest;
    for (ListIterator<Request *> iter(refusedRequest); !iter.IsDone(); iter.Next())
    {
        delete iter.Item();
    }
    delete refusedRequest;
    for (auto iter = this->onTrainRequest->begin(); iter != this->onTrainRequest->end(); iter++)
    {
        for (ListIterator<Request*> reqs(iter->second); !reqs.IsDone(); reqs.Next())
        {
            delete reqs.Item();
        }
        delete iter->second;
    }
    delete onTrainRequest;
    for (auto iter = this->onTrainRequestThread->begin(); iter != this->onTrainRequestThread->end(); iter++)
    {
        for (ListIterator<Thread*> reqs(iter->second); !reqs.IsDone(); reqs.Next())
        {
            delete reqs.Item();
        }
        delete iter->second;
    }
    delete onTrainRequestThread;
    for (ListIterator<Thread *> iter(blockTrain); !iter.IsDone(); iter.Next())
    {
        delete iter.Item();
    }
    delete blockTrain;
    printf("Destory Simulator \n");
}

//----------------------------------------------------------------------
// SimulatorLab3::insertSeats(int train_id, map<int,Bitmap> details, bool isBusinessSeats)
//  Insert a new Bitmap into
//----------------------------------------------------------------------

void SimulatorLab3::insertSeats(int train_id, map<int, Bitmap *> details, bool isBusinessSeats)
{
    if (isBusinessSeats)
    {
        this->businessSeats->insert({train_id, details});
    }
    else
    {
        this->couchSeats->insert({train_id, details});
    }
}

//----------------------------------------------------------------------
// SimulatorLab3::insertRoute(int train_id, Route route)
//  Insert a new routing into
//----------------------------------------------------------------------

void SimulatorLab3::insertRoute(int train_id, Route *route)
{
    this->routes->insert({train_id, route});
}

//----------------------------------------------------------------------
// SimulatorLab3::insertListOnTrainRequest(int train_id)
//  Insert a new list into OnTrainRequest
//  Insert a new list into OnTrainRequestThread
//----------------------------------------------------------------------

void SimulatorLab3::insertListOnTrainRequest(int train_id)
{
    this->onTrainRequest->insert({train_id, new List<Request*>()});
    this->onTrainRequestThread->insert({train_id, new List<Thread*>()});
}

//----------------------------------------------------------------------
// SimulatorLab3::appendRequest(Request * request, int num)
//  Insert a new request into list of request
//  Append a request into grandedRequest when num = 1;
//  Append a request into refusedRequest when num = 2;
//----------------------------------------------------------------------

void SimulatorLab3::appendRequest(Request *request, int num)
{
    if (num == 1)
    {
        this->grandedRequest->Append(request);
    }
    else if (num == 2)
    {
        this->refusedRequest->Append(request);
    }
}

//----------------------------------------------------------------------
// SimulatorLab3::appendRequestOnTrain(Request * request, int trainID)
//  Insert a new request thread into the train'slist of the map
//----------------------------------------------------------------------

void SimulatorLab3::appendRequestOnTrain(Request * request, int trainID)
{
    this->onTrainRequest->find(trainID)->second->Append(request);
}

//----------------------------------------------------------------------
// SimulatorLab3::appendRequestThreadOnTrain(Thread * request, int trainID)
//  Insert a new request thread into the train'slist of the map
//----------------------------------------------------------------------

void SimulatorLab3::appendRequestThreadOnTrain(Thread * request, int trainID)
{
    this->onTrainRequestThread->find(trainID)->second->Append(request);
}

//----------------------------------------------------------------------
// SimulatorLab3::appendTrains(Thread * train);
//  Insert a new train into list of train
//----------------------------------------------------------------------

void SimulatorLab3::appendTrains(Thread *train)
{
    this->blockTrain->Append(train);
}

//----------------------------------------------------------------------
// SimulatorLab3::RemoveFrontRequestFromTrain(int trainID)
//  remove a front request from the onTrainRequest
//----------------------------------------------------------------------

Request *SimulatorLab3::RemoveFrontRequestFromTrain(int trainID)
{
    return this->onTrainRequest->find(trainID)->second->RemoveFront();
}

//----------------------------------------------------------------------
// SimulatorLab3::RemoveFrontRequestThreadFromTrain(int trainID)
//  remove a front request from the onTrainRequest
//----------------------------------------------------------------------

Thread *SimulatorLab3::RemoveFrontRequestThreadFromTrain(int trainID)
{
    return this->onTrainRequestThread->find(trainID)->second->RemoveFront();
}

//----------------------------------------------------------------------
// SimulatorLab3::GetSeats(int train_id, int station_id, bool isBusinessSeats)
//  Reture a bitmap for a specific train at a specific station
//----------------------------------------------------------------------

Bitmap *SimulatorLab3::GetSeats(int train_id, int station_id, bool isBusinessSeats)
{
    if (isBusinessSeats)
    {
        return this->businessSeats->find(train_id)->second.find(station_id)->second;
    }
    else
    {
        return this->couchSeats->find(train_id)->second.find(station_id)->second;
    }
}

//----------------------------------------------------------------------
// SimulatorLab3::GetRoute(int train_id)
//  Return a specific train route
//----------------------------------------------------------------------

Route *SimulatorLab3::GetRoute(int train_id)
{
    return this->routes->find(train_id)->second;
}

//----------------------------------------------------------------------
// SimulatorLab3::getRequestsList(int index)
//  Return the grandedRequest if index == 1
//  Return the refusedRequest if index == 2
//----------------------------------------------------------------------

List<Request*> *SimulatorLab3::getRequestsList(int index)
{
    if(index == 1){
        return this->grandedRequest;
    }
    return this->refusedRequest;
}

//----------------------------------------------------------------------
// SimulatorLab3::getonTrainRequest(int trainID)
//  Return list thread of requests on this train
//----------------------------------------------------------------------

List<Request*>*SimulatorLab3::getonTrainRequest(int trainID)
{
    return this->onTrainRequest->find(trainID)->second;
}

//----------------------------------------------------------------------
// SimulatorLab3::getonTrainRequestsThread(int trainID)
//  Return list thread of requests on this train
//----------------------------------------------------------------------

List<Thread*> *SimulatorLab3::getonTrainRequestsThread(int trainID)
{
    return this->onTrainRequestThread->find(trainID)->second;
}

//----------------------------------------------------------------------
// SimulatorLab3::getTrainsList()
//  Return list thread of trains
//----------------------------------------------------------------------

List<Thread*> *SimulatorLab3::getTrainsList()
{
    return this->blockTrain;
}

//----------------------------------------------------------------------
// SimulatorLab3::updateTime()
//  time ++
//----------------------------------------------------------------------

void SimulatorLab3::updateTime()
{
    time++;
}

//----------------------------------------------------------------------
// SimulatorLab3::getTime()
//  return time
//----------------------------------------------------------------------

int SimulatorLab3::getTime()
{
    return time;
}

//----------------------------------------------------------------------
// SimulatorLab3::ResetCounter()
//  newRequestCounter = 0
//----------------------------------------------------------------------

void SimulatorLab3::ResetCounter()
{
    newRequestCounter = 0;
}

//----------------------------------------------------------------------
// SimulatorLab3::AddOneOnCounter()
//  newRequestCounter ++
//----------------------------------------------------------------------

void SimulatorLab3::AddOneOnCounter()
{
    newRequestCounter++;
}

//----------------------------------------------------------------------
// SimulatorLab3::getCounter()
//  return newRequestCounter
//----------------------------------------------------------------------

int SimulatorLab3::getCounter()
{
    return newRequestCounter;
}


SimulatorLab3 simulator = SimulatorLab3();