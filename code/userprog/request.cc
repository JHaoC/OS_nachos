#include"request.h"
#include"route.h"
#include"simulatorLab3.h"
#include <math.h>

//----------------------------------------------------------------------
// Requst::Requst(int num, int time)
// 	Create a Request
//  for the departTime, the simulation is 6am to 10pm and 10 mins 
//  as a time step. Therefore, total time steps are 16*60/10 = 96 
//----------------------------------------------------------------------

Request::Request(int num, int time)
{
    requestID = num;
    departureStationID = rand()%20; //select num for Departure station
    destinyStationID = rand()%20;   //select num for Destiny station
    while(departureStationID == destinyStationID) // make sure two stations are not same
    { 
        destinyStationID = rand()%20;
    }
    businessClass = (rand()%2==0); // even is businessclass otherwise is couch
    numOfPassengers = rand()%7+1;  //(1-8)
    departureTime = rand()%96;
    while(departureTime < time)
    {
        departureTime = rand()%96;
    }
    printf("Create a new Request %d\n", num);
    printf("There are %d passenagers from staion %d to station %d\n", numOfPassengers, departureStationID,destinyStationID);
    int hour = 6+departureTime/6;
    int min = departureTime%6*10;
    printf("They departure time is %d:%d\n",hour, min);
    if(businessClass){
        printf("They request Business seats\n\n");
    }
    else{
        printf("They request Couch seats\n\n");
    }
    
}

//----------------------------------------------------------------------
// Requst::~Requst()
// 	Destructor a Request
//----------------------------------------------------------------------

Request::~Request(){}

//----------------------------------------------------------------------
// Requst::Request(Request &r)
// 	Copy constructor
//----------------------------------------------------------------------

Request::Request(const Request &r)
{
    requestID = r.requestID;
    departureStationID = r.departureStationID;
    destinyStationID = r.destinyStationID;
    businessClass = r.businessClass;
    numOfPassengers = r.numOfPassengers;
    departureTime = r.departureTime;
}

//----------------------------------------------------------------------
// Requst::Request(Request &&r)
// 	move constructor
//----------------------------------------------------------------------

Request::Request( Request &&r)
{
    requestID = r.requestID;
    departureStationID = r.departureStationID;
    destinyStationID = r.destinyStationID;
    businessClass = r.businessClass;
    numOfPassengers = r.numOfPassengers;
    departureTime = r.departureTime;
}

//----------------------------------------------------------------------
// Requst::getRequestID()
// return RequestID
//----------------------------------------------------------------------

int Request::getRequestID(){
    return requestID;
}

//----------------------------------------------------------------------
// Requst::getDepartureStationID()
// return departureStationID
//----------------------------------------------------------------------

int Request::getDepartureStationID(){
    return departureStationID;
}

//----------------------------------------------------------------------
// Requst::getDestinyStationID()
// return destinyStationID
//----------------------------------------------------------------------

int Request::getDestinyStationID(){
    return destinyStationID;
}

//----------------------------------------------------------------------
// Requst::getNumOfPassengers()
// return numOfPassengers
//----------------------------------------------------------------------

int Request::getNumOfPassengers(){
    return numOfPassengers;
}

//----------------------------------------------------------------------
// Requst::getDepartureTime()
// return departureTime
//----------------------------------------------------------------------

int Request::getDepartureTime(){
    return departureTime;
}

//----------------------------------------------------------------------
// Requst::isBusinessClass()
// return businessClass
//----------------------------------------------------------------------

bool Request::isBusinessClass(){
    return businessClass;
}

//----------------------------------------------------------------------
// Requst::isAvailabletrain(int trainID)
// return true if the train can be selected otherwise false
//----------------------------------------------------------------------

bool Request::isAvailabletrain(int trainID)
{
    List<Station*> * stations = simulator.GetRoute(trainID)->getStations();
    bool reachstart = false;
    for(ListIterator<Station*> iter(stations); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->getStationID() != departureStationID)
        {
            if(!reachstart) continue;
            else{
                if(iter.Item()->getStationID() == destinyStationID)
                {
                    if(iter.Item()->isavailable(trainID,numOfPassengers,businessClass))
                    {return true;}
                }
                else
                {
                    if(!iter.Item()->isavailable(trainID,numOfPassengers,businessClass))
                    {return false;}
                }
            }
        }
        else{
            if(iter.Item()->getDepartureTime() < departureTime) return false;
            reachstart = true;
        }
    }
    return false;
}


//----------------------------------------------------------------------
// Requst::assignSeat(int trainID)
// Assign seats
//----------------------------------------------------------------------

void Request::assignSeat(int trainID)
{
    List<Station*> * stations = simulator.GetRoute(trainID)->getStations();
    bool reachstart = false;
    for(ListIterator<Station*> iter(stations); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->getStationID() != departureStationID)
        {
            if(!reachstart) continue;
            else{
                if(iter.Item()->getStationID() == destinyStationID)
                {
                    break;
                }
                else
                {
                    iter.Item()->assignSeats(trainID,numOfPassengers,businessClass);
                }
            }
        }
        else{
            reachstart = true;
            iter.Item()->assignSeats(trainID,numOfPassengers,businessClass);
        }
    }
}

//----------------------------------------------------------------------
// Requst::selectTrain()
// Find available trains, random select it, assign seat, put into granded
// queue, return selected trainID, otherwise input the reject queue 
// return -1
//----------------------------------------------------------------------

int Request::selectTrain()
{
    List<int> trains = List<int>();
    for(int i = 0; i < 5 ;i++)
    {
        if(isAvailabletrain(i))
        {
            trains.Append(i);
            printf("Find a train, train ID is %d\n", i);
        }
    }
    if(trains.IsEmpty()) {
        simulator.appendRequest(this,2);
        printf("There is no train can selected,\n put this request into rejected queue\n\n");
        return -1;
    }
    else
    {
        int train_num = rand()%(trains.NumInList());
        int counter = 0;
        int trainID;
        while(!trains.IsEmpty())
        {
            if(counter != train_num)
            {
                trains.RemoveFront();
                counter++;
            }
            else{
                trainID = trains.RemoveFront();
                break;
            }
        }
        assignSeat(trainID);
        simulator.appendRequest(this,1);
        simulator.appendRequestOnTrain(this,trainID);
        printf("Selected a train, train ID is %d\n\n", trainID);
        return trainID;
    }
}


//----------------------------------------------------------------------
// Requst::isbegin(int trainID)
// Return true if it is beginning
//----------------------------------------------------------------------

bool Request::isbegin(int trainID)
{
    for(ListIterator<Station*> iter(simulator.GetRoute(trainID)->getStations()); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->getStationID() == departureStationID)
        {
            return simulator.getTime() == iter.Item()->getDepartureTime(); 
        }
    }
    return false;
}

//----------------------------------------------------------------------
// Requst::isfinished(int trainID)
// Return true if it is arrival
//----------------------------------------------------------------------

bool Request::isfinished(int trainID)
{
    for(ListIterator<Station*> iter(simulator.GetRoute(trainID)->getStations()); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->getStationID() == destinyStationID)
        {
            return simulator.getTime() == iter.Item()->getArriveTime(); 
        }
    }
    return false;
}

