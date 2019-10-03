#include"train.h"
#include"simulatorLab3.h"

//----------------------------------------------------------------------
// Train::Train(int trainID)
//  Create a Train for route
//  trainState: 1 (Arrive) 2(Departure) 3(running)
//----------------------------------------------------------------------

Train::Train(int trainID)
{
    train_id = trainID;
    // Set up Route for the train
    Route *tmpRoute =new Route(trainID);
    simulator.insertRoute(trainID,tmpRoute);
    
    // Set up bitmap for the train
    map<int,Bitmap*> businessSeats = map<int,Bitmap*>();
    map<int,Bitmap*> couchSeats = map<int,Bitmap*>();

    List<Station*> * stations = tmpRoute->getStations();

    for(ListIterator<Station*> iter(stations); !iter.IsDone();iter.Next())
    {
        int id = iter.Item()->getStationID();
        businessSeats.insert({id, new Bitmap(20)});
        couchSeats.insert({id, new Bitmap(40)});
    }
    // Put seats into GV
    simulator.insertSeats(trainID,businessSeats,true);
    simulator.insertSeats(trainID,couchSeats,false);
    
    // Thread list of request into GV
    simulator.insertListOnTrainRequest(trainID);
    
    numthOFCurrentStation = 0;
    currentStation = tmpRoute->getStation(0);
    trainState = 2;
    serveItinerary = 0;
    servePassenger = 0;
    printf("Set up a new train, the train ID is %d \n ", trainID);
}

//----------------------------------------------------------------------
// Train::~Train()
//  Destructor
//----------------------------------------------------------------------

Train::~Train()
{
    if(currentStation != NULL)
    {
        delete currentStation;
    }
}

//----------------------------------------------------------------------
// Train::Train(const Train & a)
//  copy constructor
//----------------------------------------------------------------------

Train::Train(const Train & a)
{
    train_id = a.train_id;
    currentStation = a.currentStation;
    numthOFCurrentStation = a.numthOFCurrentStation;
    trainState = a.trainState;
    servePassenger = a.servePassenger;
    serveItinerary = a.serveItinerary;
}

//----------------------------------------------------------------------
// Train::Train(Train && a)
//  move constructor
//----------------------------------------------------------------------

Train::Train(Train && a)
{
    train_id = a.train_id;
    currentStation = a.currentStation;
    numthOFCurrentStation = a.numthOFCurrentStation;
    trainState = a.trainState;
    serveItinerary = a.serveItinerary;
    servePassenger = a.servePassenger;
    a.currentStation= NULL;
}

//----------------------------------------------------------------------
// Train::isFinish()
//  return True if the train reaches the destiny following its routing
//----------------------------------------------------------------------

bool Train::isFinish()
{
    return simulator.GetRoute(train_id)->getStations()->NumInList() == numthOFCurrentStation+1;
}

//----------------------------------------------------------------------
// Train::update()
//  update trainState and current station of train at the current time
//  when train departure a station, next the station traded as the 
//  currentStation and return number of trainState
//  trainState: 1 (Arrive) 2(Departure) 3(running)
//----------------------------------------------------------------------

int Train::update()
{
    int time = simulator.getTime();
    if(currentStation->getArriveTime() > time)
    {
        trainState = 3;
    }
    if(currentStation->getArriveTime()== time)
    {
        trainState = 1;
        numthOFCurrentStation++;
    }
    if(currentStation->getArriveTime() < time && time < currentStation->getDepartureTime())
    {
        trainState = 1;
    }
    if(currentStation->getDepartureTime() == time)
    {
        trainState = 2;
    }
    if(currentStation->getDepartureTime() < time)
    {
        trainState = 3;
        currentStation = simulator.GetRoute(train_id)->getStation(numthOFCurrentStation+1);
    }
    return trainState;
}

//----------------------------------------------------------------------
// Train::getDepartureInfo()
//  print # of passengers and # of itinerary for boarding
//----------------------------------------------------------------------

void Train::getDepartureInfo()
{
    int numOfItinerary = 0;
    int numOfPassenger = 0;
    for(ListIterator<Request*> iter(simulator.getonTrainRequest(train_id)); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->isbegin(train_id))
        {
            numOfItinerary ++;
            numOfPassenger = numOfPassenger + iter.Item()->getNumOfPassengers();
        }
    }
    printf("# of iteinerary is %d, # of passengers is %d for boarding at the train %d \n", numOfItinerary,numOfPassenger,train_id);
    servePassenger = servePassenger + numOfPassenger;
    serveItinerary = serveItinerary + numOfItinerary;
}

//----------------------------------------------------------------------
// Train::getArrivalInfo()
//  print # of passengers and # of itinerary for getting off
//  return # of requests stays on
//----------------------------------------------------------------------

int Train::getArrivalInfo()
{
    int numOfItinerary = 0;
    int numOfPassanger = 0;
    for(ListIterator<Request*> iter(simulator.getonTrainRequest(train_id)); !iter.IsDone(); iter.Next())
    {
        if(iter.Item()->isfinished(train_id))
        {
            numOfItinerary ++;
            numOfPassanger = numOfPassanger + iter.Item()->getNumOfPassengers();
        }
    }
    printf("# of iteinerary is %d, # of passengers is %d for getting off at the train %d \n", numOfItinerary,numOfPassanger,train_id);
    return simulator.getonTrainRequestsThread(train_id)->NumInList()-numOfItinerary;
}

//----------------------------------------------------------------------
// Train::getServeItinerary()
//  return ServeItinerary
//----------------------------------------------------------------------

int Train::getServeItinerary()
{
    return serveItinerary;
}

//----------------------------------------------------------------------
// Train::getServePassenger()
//  return ServePassenger
//----------------------------------------------------------------------

int Train::getServePassenger()
{
    return servePassenger;
}



