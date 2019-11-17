/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include <stdio.h>

void UserFork()
{
    kernel->currentThread->space->RestoreState();
    kernel->currentThread->RestoreUserState();
    kernel->machine->Run();
}

void UserExec(Thread *t)
{
    std::cout << "In user exec " << kernel->currentThread->getName() << std::endl;

    t->space->Execute();
}

void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

int SysRead(int bufferStartAdd, int size)
{
    char *secretword = "This is a test for read system call for the assignment 2.";
    int secretsize = 58;
    char *t = secretword;

    int counter = 0;
    for (int i = 0; i < size; i++)
    {
        if (i < secretsize)
        {
            int value = *(t + i);
            if (!kernel->machine->WriteMem(bufferStartAdd + i, 1, value))
            {
                i--;
                continue;
            }
            counter++;
        }
        else{
            if (!kernel->machine->WriteMem(bufferStartAdd + i, 1, (int)'\0'))
            {
                i--;
                break;
            }
        }
    }
    //delete secretword;
    kernel->stats->numConsoleCharsRead += counter;
    return counter;
}

int SysWrite(int bufferStartAdd, int size)
{
    std::cout << "The Write system call by " << kernel->currentThread->getName() << std::endl;
    char *word = new char[size+1];
    int counter = 0;
    for (int i = 0; i < size; i++)
    {
        int date;
        if (!kernel->machine->ReadMem(bufferStartAdd + i, 1, &date))
        {
            i--;
            continue;
        }

        // check date value
        if (date == (int)'\0')
        {
            word[i] = date;
            counter++;
            break;
        }
        word[i] = date;
        counter++;
    }
    word[size] = '\0';
    std::cout << word << std::endl;
    delete word;
    kernel->stats->numConsoleCharsWritten += counter;
    return counter;
}

ThreadId SysFork(int bufferStartAdd)
{
    Thread *parent = kernel->currentThread;
    Thread *child = new Thread("child thread");
    child->space = new AddrSpace(*parent->space, child->threadID);
    //std::cout << "Main space " << (int)kernel->currentThread->space << std::endl;
    //std::cout << "child space " << (int)child->space << std::endl;
    std::cout << "Fork a child thread, thread id: " << child->threadID << std::endl;
    child->SaveUserState();
    child->SetUserRegister(PCReg, bufferStartAdd);
    child->SetUserRegister(NextPCReg, bufferStartAdd + 4);
    child->Fork((VoidFunctionPtr)UserFork, (void *)0);
    return (ThreadId)child;
}

ThreadId SysExit(int statue)
{
    if (statue == 0)
    {
        printf("Exit the thread %s\n\n", kernel->currentThread->getName());
        kernel->currentThread->Finish();
    }
}

SpaceId SysExec(int exec_name)
{
    char *name = new char[50];
    for (int i = 0; i < 50; i++)
    {
        int word;
        //char a = (char)kernel->machine->mainMemory[exec_name];
        if (!kernel->machine->ReadMem(exec_name + i, 1, &word))
        {
            i--;
            continue;
        }
        name[i] = (char)word;
        // check word value
        if (word == (int)'\0')
        {
            break;
        }
        //name[i] = (char)word;
    }
    std::cout << "In Sysexec creating a new thread " << name << " by " << kernel->currentThread->getName() << std::endl;
    //Save the old thread
    Thread *oldthread = kernel->currentThread;
    Thread *thread = new Thread(name);

    // let new thread as current thread for load()
    kernel->currentThread = thread;
    thread->space = new AddrSpace();
    if (thread->space->Load(name))
    {
        thread->Fork((VoidFunctionPtr)UserExec, (void *)thread);
    }
    // Reset kernel current thread as old thread
    kernel->currentThread = oldthread;
    kernel->currentThread->space->RestoreState();
    //delete name;
    return (SpaceId)thread;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
