// Design for Lab 3
// Class defines a train who carries passengers in a region
// Train has two class seat, 20 for business and 40 for coach
// Each seat has unique ID. 
// There is a bitmap in a train to count available seats for each stations.
// There are 20 stations in the regions. The train pass them following the route,
// The Route selected by train unique number, details in route.h.

#ifndef TRAIN_H
#define TRAIN_H

#include"../lib/bitmap.h"
#include"../lib/list.h"
#include"route.h"


class Train
{
    public:
        Train(int trainID);
        ~Train();
        Train(const Train & a); //copy constructor
        Train(Train && a); //move costructor
        bool isFinish(); 
        int update();
        void getDepartureInfo();
        int getArrivalInfo();
        int getServeItinerary();
        int getServePassenger();
        
    private:
        int train_id; // Unique id
        Station * currentStation;
        int numthOFCurrentStation;
        int trainState;
        int serveItinerary;
        int servePassenger;

};

#endif
