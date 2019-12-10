// scheduler.cc
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would
//	end up calling FindNextToRun(), and that would put us in an
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"
#include <iostream>

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//  Using RBT to set up Readyqueue
//----------------------------------------------------------------------

Scheduler::Scheduler()
{
<<<<<<< HEAD
    readyList = new RedBlackTree<Thread *>;
=======
    readyList = new List<Thread *>;
>>>>>>> assignmentThird
    toBeDestroyed = NULL;
}

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{
    delete readyList;
}

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//  wrap thread to node and insert into ready list, for later scheduling onto the CPU.
//  update Virtual running time as the value to sort for RBT
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void Scheduler::ReadyToRun(Thread *thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    thread->setStatus(READY);

    // Update VRTime for the thread and using the value to insert the RBT
    int timeticker = 100000 / (readyList->GetNumofNodes() + 1);
    thread->UpdateVRtime(timeticker);
    Node<Thread *> *tmp = new Node<Thread *>(thread);
    tmp->item_value = thread->GetVRtime();
    readyList->Insert(tmp);
    printf("Put the thread: %s into Ready queue.\n\n", thread->getName());

}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyList->IsEmpty())
    {
        return NULL;
    }
    else
    {
<<<<<<< HEAD
        // Set Time Interrupt
        int delay = 100000 / (readyList->GetNumofNodes());
        Thread *res = readyList->Delele()->item;
        res->UpdateRRtime(delay);
        return res;
=======
        return readyList->RemoveFront();
>>>>>>> assignmentThird
    }
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void Scheduler::Run(Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentThread;
<<<<<<< HEAD
    printf("Context Switch happen from %s to %s, the new thread will run on CPU %d.\n\n",oldThread->getName(),nextThread->getName(), nextThread->GetRRtime());
=======
>>>>>>> assignmentThird

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing)
    { // mark that we need to delete current thread
        ASSERT(toBeDestroyed == NULL);
        toBeDestroyed = oldThread;
    }

    if (oldThread->space != NULL)
    {                               // if this thread is a user program,
        oldThread->SaveUserState(); // save the user's CPU registers
        oldThread->space->SaveState();
    }

    oldThread->CheckOverflow(); // check if the old thread
                                // had an undetected stack overflow

    kernel->currentThread = nextThread; // switch to the next thread
    nextThread->setStatus(RUNNING);     // nextThread is now running

<<<<<<< HEAD
=======
    // Set the pagetable for new thread

    //kernel->currentThread->space->RestoreState();

>>>>>>> assignmentThird
    DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);

    // we're back, running oldThread

    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyed(); // check if thread we were running
                          // before this one has finished
                          // and needs to be cleaned up
<<<<<<< HEAD

=======
    
>>>>>>> assignmentThird
    if (oldThread->space != NULL)
    {                                  // if there is an address space
        oldThread->RestoreUserState(); // to restore, do it.
        oldThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL)
    {
<<<<<<< HEAD
=======
        // reset bitmap of mainmemory
        ListIterator<TranslationEntry*> *item = new ListIterator<TranslationEntry*>(kernel->PageContainer);
        for (; !item->IsDone(); item->Next()) {
            if (item->Item()->threadID == toBeDestroyed->threadID) {
                kernel->freeMap->Clear(item->Item()->physicalPage); // clear corresponding bit
            }
        }

        // // check  TLB no not
        // for(int i = 0; i < TLBSize; i++)
        // {
        //     if(kernel->machine->tlb[i].threadID == toBeDestroyed->threadID)
        //     {
        //         kernel->machine->tlb[i].valid = FALSE;
        //         kernel->machine->tlb[i].physicalPage = -1;
        //         break;
        //     }
        // }

        // space will delete by destructor of thread
>>>>>>> assignmentThird
        delete toBeDestroyed;
        toBeDestroyed = NULL;
    }
}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void Scheduler::Print()
{
    cout << "Ready list contents:\n";
    //readyList->PrintTree();
    //readyList->Apply(ThreadPrint);
}
