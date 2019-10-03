//  Design for Lab3 
//  Rounte is a schedule for each train
//  There are selected stations from exited 20 stations (a,b.c ... )
//  Their positions as a circle as below
//  t -> a -> b -> c -> d -> e -> f -> g -> h
//  -> i -> j -> k -> l -> m -> n -> o -> p -> q
//  -> r -> s -> t -> a
//  Trains stay a station 10 mins for passengers in and out,
//  It takes 50 mins from the current station to the next station.
//  There are total 5 trains (unique-ID 1,2,3,4,5) 

//  Schedule table (Arr: Arrival, Dep: Departure) 
//      Time: 04:50am | 05:00am | 05:50am | 06:00am | 06:50am | 07:00am | 07:50am | 08:00am | 08:50am | 09:00am 
//  Train_ID
//     0              |  Dep a  |  Arr b  |  Dep b  |  Arr c  |  Dep c  |  Arr d  |  Dep d  |  Arr e  |  Dep e  
//     1              |  Dep c  |  Arr d  |  Dep d  |  Arr e  |  Dep e  |  Arr f  |  Dep f  |  Arr g  |  Dep g  
//     2              |  Dep e  |  Arr f  |  Dep f  |  Arr g  |  Dep g  |  Arr h  |  Dep h  |  Arr i  |  Dep i  
//     3              |  Dep g  |  Arr h  |  Dep h  |  Arr i  |  Dep i  |  Arr j  |  Dep j  |  Arr k  |  Dep k  
//     4              |  Dep i  |  Arr j  |  Dep j  |  Arr k  |  Dep k  |  Arr l  |  Dep l  |  Arr m  |  Dep m  
//
//      Time: 09:50am | 10:00am | 10:50am | 11:00am | 11:50am | 12:00pm | 12:50pm | 01:00pm | 01:50pm | 02:00pm 
//  Train_ID
//     0       Arr f  |  Dep f  |  Arr g  |  Dep g  |  Arr h  |  Dep h  |  Arr i  |  Dep i  |  Arr j  |  Dep j  
//     1       Arr h  |  Dep h  |  Arr i  |  Dep i  |  Arr j  |  Dep j  |  Arr k  |  Dep k  |  Arr l  |  Dep l  
//     2       Arr j  |  Dep j  |  Arr k  |  Dep k  |  Arr l  |  Dep l  |  Arr m  |  Dep m  |  Arr n  |  Dep n  
//     3       Arr l  |  Dep l  |  Arr m  |  Dep m  |  Arr n  |  Dep n  |  Arr o  |  Dep o  |  Arr p  |  Dep p  
//     4       Arr n  |  Dep n  |  Arr o  |  Dep o  |  Arr p  |  Dep p  |  Arr q  |  Dep q  |  Arr r  |  Dep r  
//
//      Time: 02:50pm | 03:00pm | 03:50pm | 04:00pm | 04:50pm | 05:00pm | 05:50pm | 06:00pm | 06:50pm | 07:00pm 
//  Train_ID
//     0       Arr k  |  Dep k  |  Arr l  |  Dep l  |  Arr m  |  Dep m  |  Arr n  |  Dep n  |  Arr o  |  Dep o  
//     1       Arr m  |  Dep m  |  Arr n  |  Dep n  |  Arr o  |  Dep o  |  Arr p  |  Dep p  |  Arr q  |  Dep q  
//     2       Arr o  |  Dep o  |  Arr p  |  Dep p  |  Arr q  |  Dep q  |  Arr r  |  Dep r  |  Arr s  |  Dep s  
//     3       Arr q  |  Dep q  |  Arr r  |  Dep r  |  Arr s  |  Dep s  |  Arr t  |  Dep t  |  Arr a  |  Dep a  
//     4       Arr s  |  Dep s  |  Arr t  |  Dep t  |  Arr a  |  Dep a  |  Arr b  |  Dep b  |  Arr c  |  Dep c  
//
//      Time: 07:50pm | 08:00pm | 08:50pm | 09:00pm | 09:50pm | 10:00pm | 10:50pm | 11:00pm | 11:50pm 
//  Train_ID
//     0       Arr p  |  Dep p  |  Arr q  |  Dep q  |  Arr r  |  Dep r  |  Arr s  |  Dep s  |  Arr t  
//     1       Arr r  |  Dep r  |  Arr s  |  Dep s  |  Arr t  |  Dep t  |  Arr a  |  Dep a  |  Arr b   
//     2       Arr t  |  Dep t  |  Arr a  |  Dep a  |  Arr b  |  Dep b  |  Arr c  |  Dep c  |  Arr d    
//     3       Arr b  |  Dep b  |  Arr c  |  Dep c  |  Arr d  |  Dep d  |  Arr e  |  Dep e  |  Arr f  
//     4       Arr d  |  Dep d  |  Arr e  |  Dep e  |  Arr f  |  Dep f  |  Arr g  |  Dep g  |  Arr h  
//
//  Stations has a unique_ID. ID-tabel as below
//  Station name  a |  b |  c |  d |  e |  f |  g |  h |  i |  j |  k |  l |  m |  n |  o |  p |  q |  r |  s |  t
//  Station ID    0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 

#ifndef ROUTE_H
#define ROUTE_H

#include"../lib/list.h"

class Station{
    public:
        Station(int station_ID);
        ~Station();
        Station(const Station &s);
        Station(Station &&s);
        void setArriveTime(int time);
        void setDepartureTime(int time);
        void setIndex(int ind);
        int getArriveTime();
        int getDepartureTime();
        int getStationID();
        int getIndex();
        bool isavailable(int train_id, int passagersnum, bool isBusiness);
        void assignSeats(int train_id, int passagersnum, bool isBusiness);
    
    private:
        int stationID;
        int arriveTime;
        int departureTime;
        int index;
};

// Follow the Schedule, the route creates covering 6am-10pm
class Route
{
    public:
        Route();
        Route(int train_ID);
        ~Route();
        Route(const Route &r);
        Route(Route &&r);
        Station* getStation(int num); //return the num th station
        List<Station*> * getStations();
        //Station* ispassStation(int stationID); //return station if pass, otherwise NUll
        // bool checkDepartureStation(std::string &name, int departureTime);
        
    private:
        List<Station*> *stations;
        
};

#endif