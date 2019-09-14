// lab2.cc
//     	Implement method for lab2.h

#include <math.h>
#include <stdio.h>
#include "lab2.h"
#include "../lib/list.h"

//----------------------------------------------------------------------
// Customer::Customer
// Initialize a Customer with a random number of items between 5-40
// estimate SeverTime based on the number of items
// time = roundUp(5s *  # items + 10s)/60 + 1.5min)
//----------------------------------------------------------------------

Customer::Customer()
{
    NumOfItems = rand() % 35 + 5;
    ServerTime = ceil(((5.0 * NumOfItems + 10.0) / 60.0) + 1.5);
    WaitingTime = 0;
}

//----------------------------------------------------------------------
// Customer::~Customer
// deallocation of the Customer
//----------------------------------------------------------------------

Customer::~Customer()
{
}

//----------------------------------------------------------------------
// Customer::Customer(const Customer &c)
// copy constructor
//----------------------------------------------------------------------

Customer::Customer(const Customer &c)
{
    NumOfItems = c.NumOfItems;
    ServerTime = c.ServerTime;
    WaitingTime = c.WaitingTime;
}

//----------------------------------------------------------------------
// Customer::Customer(Customer &&c)
// move constructor
//----------------------------------------------------------------------

Customer::Customer(Customer &&c)
{
    NumOfItems = c.NumOfItems;
    ServerTime = c.ServerTime;
    WaitingTime = c.WaitingTime;
}

//----------------------------------------------------------------------
// Customer::GetCheckoutTime()
//----------------------------------------------------------------------

int Customer::GetCheckoutTime()
{
    return ServerTime;
}

//----------------------------------------------------------------------
// Customer::GetNumOfItems()
// return NumOfItems
//----------------------------------------------------------------------

int Customer::GetNumOfItems()
{
    return NumOfItems;
}

//----------------------------------------------------------------------
// Customer::AddWaitingTime()
// waiting time ++, using for in the waitting queue
//----------------------------------------------------------------------

void Customer::AddWaitingTime()
{
    WaitingTime++;
}

//----------------------------------------------------------------------
// Customer::GetWaitingTime()
// return WaitingTime
//----------------------------------------------------------------------

int Customer::GetWaitingTime()
{
    return WaitingTime;
}

//----------------------------------------------------------------------
// Casher::Casher
//	Initialize a Casher
//----------------------------------------------------------------------

Casher::Casher()
{
    Current_Customer_CheckOutTime = 0;
    Time_stand_over_3 = 0;
    StandingCustomers = new List<Customer *>();
    Current_Customer = NULL;
}

//----------------------------------------------------------------------
// Casher::~Casher
// deallocation of the Casher
//----------------------------------------------------------------------

Casher::~Casher()
{
    while (!StandingCustomers->IsEmpty())
    {
        delete StandingCustomers->RemoveFront();
    }
    delete StandingCustomers;
    if (Current_Customer != NULL)
    {
        delete Current_Customer;
    }
}

//----------------------------------------------------------------------
// Casher::Casher(const Casher &c)
// copy constructor
//----------------------------------------------------------------------

Casher::Casher(const Casher &c)
{
    Current_Customer_CheckOutTime = c.Current_Customer_CheckOutTime;
    Time_stand_over_3 = c.Time_stand_over_3;
    StandingCustomers = new List<Customer*>();
    for(ListIterator<Customer*> iter(c.StandingCustomers); !iter.IsDone();iter.Next())
    {
        StandingCustomers->Append(iter.Item());
    }
    Current_Customer = c.Current_Customer;
}

//----------------------------------------------------------------------
// Casher::Casher(Casher &&c)
// move constructor
//----------------------------------------------------------------------

Casher::Casher(Casher &&c)
{
    Current_Customer_CheckOutTime = c.Current_Customer_CheckOutTime;
    Time_stand_over_3 = c.Time_stand_over_3;
    StandingCustomers = c.StandingCustomers;
    Current_Customer = c.Current_Customer;
    c.StandingCustomers = NULL;
    c.Current_Customer = NULL;
}

//----------------------------------------------------------------------
// Casher::IsCheching
// Current_Customer finishs or not
//----------------------------------------------------------------------

bool Casher::IsChecking()
{
    return Current_Customer_CheckOutTime != 0;
}

//----------------------------------------------------------------------
// Casher::IsAddable
// Current_Customer finishs or not
// StandingCustomers is full or not
//----------------------------------------------------------------------

bool Casher::IsAddable()
{
    if (IsChecking())
    {
        if (this->StandingCustomers->NumInList() >= 4)
        {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------
// Casher::AddCustomer
// Add new Customer in the line
//----------------------------------------------------------------------

void Casher::AddCustomer(Customer *customer)
{
    if(customer!=NULL)
    {
    if (this->Current_Customer == NULL)
    {
        this->Current_Customer = customer;
        this->Current_Customer_CheckOutTime = customer->GetCheckoutTime();
    }
    else
    {
        StandingCustomers->Append(customer);
    }
    }
}

//----------------------------------------------------------------------
// Casher::SwitchCustomer
// Return Null if Current Customer is cheching out
// Return Current Customer if the Customer finished
// And Switch next Customer from Standing Line
//----------------------------------------------------------------------

Customer *Casher::SwitchCustomer()
{
    Customer *tmp = NULL;
    if (!IsChecking()){
        if (this->StandingCustomers->IsEmpty())
        {
            tmp = this->Current_Customer;
            this->Current_Customer = NULL;
        }
        else
        {
            tmp = this->Current_Customer;
            this->Current_Customer = this->StandingCustomers->RemoveFront();
            Current_Customer_CheckOutTime = this->Current_Customer->GetCheckoutTime();
        }
    }
    return tmp;
}

//----------------------------------------------------------------------
// Casher::GetCustomerNum
// Return # Customers including Current checking out and standing
//----------------------------------------------------------------------

int Casher::GetCustomerNum()
{
    int res = this->StandingCustomers->NumInList();
    if (this->Current_Customer != NULL)
    {
        res++;
    }
    return res;
}

//----------------------------------------------------------------------
// AddWaitting
// Help function for list->Apply() at Casher::Run
//----------------------------------------------------------------------

void AddWaitting(Customer *customer)
{
    customer->AddWaitingTime();
}

//----------------------------------------------------------------------
// Casher::Run
// waiting time ++ for Customers who in the standing queue
// Current_Customer_CheckOutTime --
//----------------------------------------------------------------------

void Casher::Run()
{
    this->StandingCustomers->Apply(AddWaitting);
    this->Current_Customer_CheckOutTime--;
    if(this->StandingCustomers->NumInList()>3)
    {
        Time_stand_over_3++;
    }
}

//----------------------------------------------------------------------
// Casher::Reset
// Time_stand_over_3 = 0
//----------------------------------------------------------------------

void Casher::Reset()
{
    this->Time_stand_over_3 = 0;
}

//----------------------------------------------------------------------
// Casher::GetTimeStand3
// return Time_stand_over_3
//----------------------------------------------------------------------

int Casher::GetTimeStand3()
{
    return Time_stand_over_3;
}

//----------------------------------------------------------------------
// CasherCompare
//	Compare to Casher based on which should occur first.
//----------------------------------------------------------------------

static int
CasherCompare(Casher *x, Casher *y)
{
    if (x->GetCustomerNum() < y->GetCustomerNum())
    {
        return -1;
    }
    else if (x->GetCustomerNum() > y->GetCustomerNum())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------
// Supermarket::Supermarket
//	Initialize a Supermarket
//----------------------------------------------------------------------

Supermarket::Supermarket()
{
    Cashers = new SortedList<Casher *>(CasherCompare);
    WaitingQueue = new List<Customer *>();
    for (int i = 0; i < 2; i++)
    {
        Casher *tmp = new Casher();
        Cashers->Insert(tmp);
    }
}

//----------------------------------------------------------------------
// Supermarket::~Supermarket
// deallocation of the Supermarket
//----------------------------------------------------------------------

Supermarket::~Supermarket()
{
    while (!Cashers->IsEmpty())
    {
        delete Cashers->RemoveFront();
    }
    delete Cashers;

    while (!WaitingQueue->IsEmpty())
    {
        delete WaitingQueue->RemoveFront();
    }
    delete WaitingQueue;
}

//----------------------------------------------------------------------
// Supermarket:: InsertCustomer
// Insert a new customer.
// The rule is insert to less standing customers line,
// Insert the waiting queue if all lines are full,
// Open a new Line if more than 10 customers at the waiting queue
//----------------------------------------------------------------------

void Supermarket::InsertCustomer(Customer *customer)
{
    this->WaitingQueue->Append(customer);
    if (this->Cashers->Front()->IsAddable())
    {
        Casher *casher = this->Cashers->RemoveFront();
        Customer *tmp = this->WaitingQueue->RemoveFront();
        casher->AddCustomer(tmp);
        this->Cashers->Insert(casher);
    }
    if (this->WaitingQueue->NumInList() > 10 && this->Cashers->NumInList() < 10)
    {
        Casher *new_casher = new Casher();
        for (int i = 0; i < 5; i++)
        {
            Customer *tmp = this->WaitingQueue->RemoveFront();
            new_casher->AddCustomer(tmp);
        }
        this->Cashers->Insert(new_casher);
    }
}

//----------------------------------------------------------------------
// CasherRun()
// Help function for list->Apply() at Supermarket::Run()
//----------------------------------------------------------------------

void CasherRun(Casher *casher)
{
    casher->Run();
}

//----------------------------------------------------------------------
// Supermarket::GenerateNumCustomers
// Simulation the chech out situation at every minute
// Peak hours(4-6), simulating from 2-7, Peak minutes from 120 to 240
//----------------------------------------------------------------------

int Supermarket::GenerateNumCustomers(int time)
{

    if (time >= 120 && time < 240)
    {
        return rand() % 5 + 5;
    }
    else
    {
        return rand() % 5;
    }
}

//----------------------------------------------------------------------
// Supermarket::CheckCashers
// Delete Casher if need
//----------------------------------------------------------------------

void Supermarket::CheckCashers()
{
    if (this->WaitingQueue->NumInList() == 0)
    {
        while (this->Cashers->NumInList() > 2)
        {
            Casher *tmp = this->Cashers->Front();
            if (!tmp->IsChecking() && tmp->GetCustomerNum() < 1)
            {
                this->Cashers->RemoveFront();
            }
            else
            {
                break;
            }
        }
    }
}

//----------------------------------------------------------------------
// Supermarket::Run
// Simulate from 2pm - 7pm
//----------------------------------------------------------------------

void Supermarket::Run()
{
    int shortest_waiting_time_en = 400;
    int longest_waiting_time_en = 0;
    int average_waiting_time_en = 0;
    int shortest_service_time_en = 400;
    int longest_service_time_en = 0;
    int average_service_time_en = 0;
    int maxNumCosWaitingQueeu = 0;
    for (int hour = 2; hour < 7; hour++)
    {
        int Num_Customers_Checkout = 0;
        int Num_Customers_Done = 0;
        int shortest_waiting_time = 400;
        int longest_waiting_time = 0;
        int average_waiting_time = 0;
        int shortest_service_time = 400;
        int longest_service_time = 0;
        int average_service_time = 0;
        int average_num_cashers = 0;
        int max_num_cashers = 0;
        int average_standing3 = 0;
        int smallest_Cust_wait = 100;
        int largest_Cust_wait = 0;
        int average_Cust_wait = 0;

        for (int minu = 0; minu < 60; minu++)
        {
            // Check all cashers
            for (ListIterator<Casher *> iter(this->Cashers); !iter.IsDone(); iter.Next())
            {
                Customer *tmp = iter.Item()->SwitchCustomer();
                if (tmp == NULL)
                {
                    continue;
                }
                else
                {
                    Num_Customers_Done++;
                    average_service_time = average_service_time + tmp->GetCheckoutTime();
                    average_waiting_time = average_waiting_time + tmp->GetWaitingTime();
                    if (longest_service_time < tmp->GetCheckoutTime())
                    {
                        longest_service_time = tmp->GetCheckoutTime();
                    }
                    if (longest_waiting_time < tmp->GetWaitingTime())
                    {
                        longest_waiting_time = tmp->GetWaitingTime();
                    }
                    if (shortest_service_time > tmp->GetCheckoutTime())
                    {
                        shortest_service_time = tmp->GetCheckoutTime();
                    }
                    if (shortest_waiting_time > tmp->GetWaitingTime())
                    {
                        shortest_waiting_time = tmp->GetWaitingTime();
                    }
                }
            }

            // New Customers
            int time = (hour - 2) * 60 + minu;
            int Num_New_Customers = GenerateNumCustomers(time);
            Num_Customers_Checkout = Num_Customers_Checkout + Num_New_Customers;
            for (int i = 0; i < Num_New_Customers; i++)
            {
                Customer *customer = new Customer();
                InsertCustomer(customer);
            }

            // Check Cashers
            this->CheckCashers();

            average_num_cashers = average_num_cashers + this->Cashers->NumInList();
            if (max_num_cashers < this->Cashers->NumInList())
            {
                max_num_cashers = this->Cashers->NumInList();
            }
            average_Cust_wait = average_Cust_wait + this->WaitingQueue->NumInList();
            if (smallest_Cust_wait > this->WaitingQueue->NumInList())
            {
                smallest_Cust_wait = this->WaitingQueue->NumInList();
            }
            if (largest_Cust_wait < this->WaitingQueue->NumInList())
            {
                largest_Cust_wait = this->WaitingQueue->NumInList();
            }
            // update waiting time
            this->Cashers->Apply(CasherRun);
            this->WaitingQueue->Apply(AddWaitting);
        }
        for (ListIterator<Casher *> iter(this->Cashers); !iter.IsDone(); iter.Next())
        {
            average_standing3 = average_standing3 + iter.Item()->GetTimeStand3();
            iter.Item()->Reset();
        }

        printf("From %d PM to %d PM\n", hour, hour+1);
        printf("Avarage number of customers arriving for checkout %G per minutes\n", Num_Customers_Checkout/60.0);
        printf("Avarage waiting time %G minutes\n", (double)average_waiting_time / Num_Customers_Done);
        printf("Shortest waiting time %d minutes\n", shortest_waiting_time);
        printf("Longest waiting time %d minutes\n", longest_waiting_time);
        printf("Avarage service time %G minutes\n", (double)average_service_time / Num_Customers_Done);
        printf("Shortest service time %d minutes\n", shortest_service_time);
        printf("Longest service time %d minutes\n", longest_service_time);
        printf("Avarage number of open line %G\n", (double)average_num_cashers / 60);
        printf("Maximum number of open line %d\n", max_num_cashers);
        printf("Avarage time each casher will have more than 3 customers standing in line %G minutes\n", (double)average_standing3 * 60 / (double)average_num_cashers);
        printf("Avarage number of customers in the Waiting queue %G\n", (double)average_Cust_wait / 60);
        printf("Smallest number of customers in the Waiting queue %d\n", smallest_Cust_wait);
        printf("Largest number of customers in the Waiting queue %d\n", largest_Cust_wait);

        average_waiting_time_en = average_waiting_time_en + (double)average_waiting_time / Num_Customers_Done;
        average_service_time_en = average_service_time_en + (double)average_service_time / Num_Customers_Done;
        if (shortest_waiting_time_en > shortest_waiting_time)
        {
            shortest_waiting_time_en = shortest_waiting_time;
        }
        if (shortest_service_time_en > shortest_service_time)
        {
            shortest_service_time_en = shortest_service_time;
        }
        if (longest_waiting_time_en < longest_waiting_time)
        {
            longest_waiting_time_en = longest_waiting_time;
        }
        if (longest_service_time_en < longest_service_time)
        {
            longest_service_time_en = longest_service_time;
        }
        if (maxNumCosWaitingQueeu < largest_Cust_wait)
        {
            maxNumCosWaitingQueeu = largest_Cust_wait;
        }
    }
    printf("\n Summary \n");
    printf("Avarage waiting time %G\n", (double)average_waiting_time_en / 5);
    printf("Shortest waiting time %d\n", shortest_waiting_time_en);
    printf("Longest waiting time %d\n", longest_waiting_time_en);
    printf("Avarage service time %G\n", (double)average_service_time_en / 5);
    printf("Shortest waiting time %d\n", shortest_service_time_en);
    printf("Longest waiting time %d\n", longest_service_time_en);
    printf("Maximun number of customers in the waiting quese %d\n", maxNumCosWaitingQueeu);
}