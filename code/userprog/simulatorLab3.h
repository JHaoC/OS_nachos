// Design for lab3
// A railway ticket reservation system. It simalutes 
// tickets operations and trains operations from 6am to 10pm.
// There are at less 5 trains working during the 16 hours. The 
// train schedule is designed at route.h. Every ten minutes, 
// there are 5 request generated automatically.
// Following the rule, passangers check in and check out when
// trains arrive stations.

#ifndef SIMULATORLAB3_H
#define SIMULATORLAB3_H

#include"../lib/bitmap.h"
#include"../lib/list.h"
#include"route.h"
#include"request.h"
#include"../threads/thread.h"
#include<map>

// The seats bitmaps and Routes for whole system of trains
// The map of seats records all seats case at each station for each train. 
// map< train_Id, map< station_id, Bitmap>>
// The Routes has all trains' routing records.
// map< train_Id, List< Station>>

class SimulatorLab3{
    public:
        SimulatorLab3();
        ~SimulatorLab3();
        void insertSeats(int train_id, map<int,Bitmap*> details, bool isBusinessSeats);
        void insertRoute(int train_id, Route *route);
        void insertListOnTrainRequest(int train_id);
        void appendRequest(Request * request, int num);
        void appendRequestOnTrain(Request * request, int trainID);
        void appendRequestThreadOnTrain(Thread * request, int trainID);
        void appendTrains(Thread * train);
        Request* RemoveFrontRequestFromTrain(int trainID);
        Thread* RemoveFrontRequestThreadFromTrain(int trainID);
        Bitmap* GetSeats(int train_id, int station_id, bool isBusinessSeats);
        Route* GetRoute(int train_id);
        List<Request*>* getRequestsList(int index);
        List<Request*>* getonTrainRequest(int trainID);
        List<Thread*>* getonTrainRequestsThread(int trainID);
        List<Thread*>* getTrainsList();
        void updateTime();
        int getTime();
        void ResetCounter();
        void AddOneOnCounter();
        int getCounter();

    private:
        map<int, map<int,Bitmap*>> *businessSeats;
        map<int, map<int, Bitmap*>> *couchSeats;
        map<int, Route*> *routes;
        List<Request*> *grandedRequest;
        List<Request*> *refusedRequest;
        map<int,List<Request*>*> *onTrainRequest;
        map<int,List<Thread*>*> *onTrainRequestThread;
        List<Thread*> *blockTrain;
        int time;
        int newRequestCounter;
};

extern SimulatorLab3 simulator;

#endif