#include"route.h"
#include"simulatorLab3.h"

//----------------------------------------------------------------------
// Station::Station(int station_ID)
//  Create a Station for route
//----------------------------------------------------------------------

Station::Station(int station_ID)
{
    stationID = station_ID;
    arriveTime = -1;
    departureTime = -1;
    index = -1;
}

//----------------------------------------------------------------------
// Station::~Station()
//  Destructor 
//----------------------------------------------------------------------

Station::~Station(){}

//----------------------------------------------------------------------
// Station::Station(const Station &s)
//  Copy constructor
//----------------------------------------------------------------------

Station::Station(const Station &s)
{
    stationID = s.stationID;
    arriveTime = s.arriveTime;
    departureTime = s.departureTime;
    index = s.index;
}

//----------------------------------------------------------------------
// Station::Station(Station &&s)
//  move constructor
//----------------------------------------------------------------------

Station::Station(Station &&s)
{
    stationID = s.stationID;
    arriveTime = s.arriveTime;
    departureTime = s.departureTime;
    index = s.index;
}

//----------------------------------------------------------------------
// Station::setArriveTime(int time)
//  Set Arrival time
//----------------------------------------------------------------------

void Station::setArriveTime(int time)
{
    arriveTime = time;
}

//----------------------------------------------------------------------
// Station::setDepartureTime(int time)
//  Set departure time
//----------------------------------------------------------------------

void Station::setDepartureTime(int time)
{
    departureTime = time;
}

//----------------------------------------------------------------------
// Station::setIndex(int ind)
//  Set indes
//----------------------------------------------------------------------

void Station::setIndex(int ind)
{
    index = ind;
}

//----------------------------------------------------------------------
// Station::getArriveTime()
//  return Arrival time
//----------------------------------------------------------------------

int Station::getArriveTime()
{
   return arriveTime;
}

//----------------------------------------------------------------------
// Station::getDepartureTime()
//  return departure time
//----------------------------------------------------------------------

int Station::getDepartureTime()
{
   return departureTime;
}

//----------------------------------------------------------------------
// Station::getStationID()
//  return StationID 
//----------------------------------------------------------------------

int Station::getStationID()
{
   return stationID;
}

//----------------------------------------------------------------------
// Station::getIndex()
//  return index 
//----------------------------------------------------------------------

int Station::getIndex()
{
   return index;
}

//----------------------------------------------------------------------
// Station::assignSeats(int passagersnum, bool isBusiness);
//  assign seats at the bigmap;
//----------------------------------------------------------------------

void Station::assignSeats(int train_id,int passagersnum, bool isBusiness)
{
   Bitmap* seats = simulator.GetSeats(train_id,stationID,isBusiness);
   int counter = 0;
   while(counter<passagersnum)
   {
       seats->FindAndSet();
       counter++;
   }
}

//----------------------------------------------------------------------
// Station::isavailable(int passagersnum, bool isBusiness);
//  return true if there are available seats for request
//  otherwise false
//----------------------------------------------------------------------

bool Station::isavailable(int train_id,int passagersnum, bool isBusiness)
{
   Bitmap* seats = simulator.GetSeats(train_id,stationID,isBusiness);
   return seats->NumClear() >= passagersnum;
}


//----------------------------------------------------------------------
// Route::Route(int train_ID)
//  Create a route
//----------------------------------------------------------------------

Route::Route(int train_ID)
{
    int tmp[5][17] = {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17},
                      {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19},
                      {5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,0,1},
                      {7,8,9,10,11,12,13,14,15,16,17,18,19,0,1,2,3},
                      {9,10,11,12,13,14,15,16,17,18,19,0,1,2,3,4,5}};
    
    this->stations = new List<Station*> ();
    for(int i = 0; i <17; i++)
    {
        Station * s = new Station(tmp[train_ID][i]);
        if(i != 0){
            s->setArriveTime((i-1)*6+5);
        }
        if(i!=16)
        {
            s->setDepartureTime(i*6);
        }
        s->setIndex(i+1);
        this->stations->Append(s);
    }
}

//----------------------------------------------------------------------
// Route::~Route()
//  Destructor 
//----------------------------------------------------------------------

Route::~Route()
{
    while (!this->stations->IsEmpty())
    {
        this->stations->RemoveFront();
    }
    delete this->stations;
}

//----------------------------------------------------------------------
// Route::Route(const Route &r)
//  Copy constructor
//----------------------------------------------------------------------

Route::Route(const Route &r)
{
    stations = new List<Station*> ();
    for(ListIterator<Station*> iter(r.stations); !iter.IsDone();iter.Next())
    {
        stations->Append(iter.Item());
    }
}

//----------------------------------------------------------------------
// Route::Route(Route &&r)
//  move constructor
//----------------------------------------------------------------------

Route::Route(Route &&r)
{
    stations = r.stations;
    r.stations = NULL;
}

//----------------------------------------------------------------------
// Route::getStation(int num)
//  return the num th station base on 0
//----------------------------------------------------------------------

Station* Route::getStation(int num)
{
    int counter = 0;
    for(ListIterator<Station*> iter(stations); !iter.IsDone();iter.Next())
    {
        if(counter == num)
        {
            return iter.Item();
        }
        counter++;
    }
}

//----------------------------------------------------------------------
// Route::getStations()
//  return stations 
//----------------------------------------------------------------------

List<Station*> * Route::getStations()
{
    return stations;
}
