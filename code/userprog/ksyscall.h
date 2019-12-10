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

#include "main.h"
#include "syscall.h"
#include "thread.h"
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

int SysRead(int bufferStartAdd, int size, int id)
{
    char * buffer = new char[size];
    int counter = 0;
    
    if(id == ConsoleInputId)
    {
        kernel->io_lock->Acquire();
        while(counter < size)
        {
            buffer[counter] = kernel->synchConsoleIn->GetChar();
            counter++;
        }
        kernel->io_lock->Release();
    }
    else if(id == -1)
    {
        delete buffer;
        return 0;
    }
    else
    {
        FileEntry * fileEntry = kernel->globalFileTable->ReadEntry((OpenFileId)id);
        fileEntry->readLock->Acquire();
        fileEntry->readCounter++;
        if(fileEntry->readCounter== 1) fileEntry->writeLock->Acquire();
        fileEntry->readLock->Release();

        kernel->io_lock->Acquire();
        // Get file pointer from thread filetable
        OpenFile * target = kernel->currentThread->GetFile((OpenFileId)id); 
        if(target != NULL)
        {
            counter = target->Read(buffer,size);
        }
        kernel->io_lock->Release();

        fileEntry->readLock->Acquire();
        fileEntry->readCounter--;
        if(fileEntry->readCounter == 0) fileEntry->writeLock->Release();
        fileEntry->readLock->Release();
    }
    //buffer[size] = '\0';

    // write back to memory
    for (int i = 0; i < size; i++)
    {
        kernel->machine->WriteMem(bufferStartAdd + i, 1, (int)buffer[i]);
    }

    // //delete secretword;
    // // Assigment 2
    // char *secretword = "This is a test for read system call for the assignment 2 and 3";
    // int secretsize = 58;
    // char *t = secretword;

    // int counter = 0;
    // for (int i = 0; i < size; i++)
    // {
    //     if (i < secretsize)
    //     {
    //         int value = *(t + i);
    //         if (!kernel->machine->WriteMem(bufferStartAdd + i, 1, value))
    //         {
    //             i--;
    //             continue;
    //         }
    //         counter++;
    //     }
    //     else{
    //         if (!kernel->machine->WriteMem(bufferStartAdd + i, 1, (int)'\0'))
    //         {
    //             i--;
    //             break;
    //         }
    //     }
    // }
    // //delete secretword;
    delete buffer;
    kernel->stats->numConsoleCharsRead += counter;
    return counter;
}

int SysWrite(int bufferStartAdd, int size, int id)
{
    //std::cout << "The Write system call by " << kernel->currentThread->getName() << std::endl;
    //char *word = new char[size+1];
    char *word = new char[size];
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
    //word[size] = '\0';
    if(id == ConsoleOutputId)
    {
        kernel->io_lock->Acquire();
        for(int i = 0; i < size; i++)
        {
            kernel->synchConsoleOut->PutChar(word[i]);
        }
        kernel->io_lock->Release();
    }
    else if(id == -1)
    {
        delete word;
        return 0;
    }
    else
    {
        // down openfile write lock
        FileEntry * fileEntry = kernel->globalFileTable->ReadEntry(id);
        fileEntry->writeLock->Acquire();

        kernel->io_lock->Acquire();
        OpenFile * target = kernel->currentThread->GetFile(id); 
        if(target != NULL)
        {
            counter = target->Write(word,size);
        }
        kernel->io_lock->Release();
        
        // release lock
        fileEntry->writeLock->Release();
    }
    
    delete word;
    kernel->stats->numConsoleCharsWritten += counter;
    return counter;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int SysCreate(int namestart, int protection)
{
    // Get name
    char *name = new char[50];
    for (int i = 0; i < 50; i++)
    {
        int word;
        //char a = (char)kernel->machine->mainMemory[exec_name];
        if (!kernel->machine->ReadMem(namestart + i, 1, &word))
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
    }
    if(kernel->fileSystem->Create(name,0,protection)) 
    {
        delete name;
        return 1;
    }
    
    delete name;
    return -1;
}

///////////////////////////////////////////////////////////////////////////
int SysRemove(int namestart)
{
    // Get name
    char *name = new char[50];
    for (int i = 0; i < 50; i++)
    {
        int word;
        //char a = (char)kernel->machine->mainMemory[exec_name];
        if (!kernel->machine->ReadMem(namestart + i, 1, &word))
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
    }
    if(kernel->fileSystem->Remove(name)) 
    {
        delete name;
        return 1;
    }
    delete name;
    return -1;
}


///////////////////////////////////////////////////////////////////////////
int SysOpen(int namestart, int mode)
{
    // Get name
    char *name = new char[50];
    for (int i = 0; i < 50; i++)
    {
        int word;
        //char a = (char)kernel->machine->mainMemory[exec_name];
        if (!kernel->machine->ReadMem(namestart + i, 1, &word))
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
    }

    OpenFile* tmp = kernel->fileSystem->Open(name, mode);
    if(tmp == NULL)
    {
        std::cout<< "There is not file named " << name <<std::endl;
        delete name;
        return -1; // failure
    }

    delete name;
    // Update thread openfile table and global file table
    std::cout<< "Open file named " << name <<" Id is "<< tmp->GetOpenFileID()<<std::endl;
    ASSERT((int)kernel->currentThread->openFileTable->Insert(tmp) == tmp->GetOpenFileID())
    return tmp->GetOpenFileID();
}

///////////////////////////////////////////////////////////////////////////
int SysSeek(int position, int id)
{
    OpenFile *tmp = kernel->currentThread->GetFile((OpenFileId)id);
    if(tmp == NULL) return -1;
    tmp->Seek(position);
    return 1;
}

///////////////////////////////////////////////////////////////////////////
int SysClose(int id)
{
    if(kernel->currentThread->openFileTable->Remove(id)== -1)
    {
        return -1;
    }
    return 1;
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

        //wake parent if existing
        Thread *parent = kernel->currentThread->GetParentThread();
        if( parent != NULL)
        {
            parent->RemoveChild(kernel->currentThread);
            IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);	// disable interrupts
            kernel->scheduler->ReadyToRun(parent);
            kernel->interrupt->SetLevel(oldLevel);
        }
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
    //kernel->currentThread = thread;
    thread->space = new AddrSpace();
    if (thread->space->Load(name))
    {
        thread->Fork((VoidFunctionPtr)UserExec, (void *)thread);
        //assign parent for child;
        thread->SetParentThread(oldthread);

        // Append child into parent
        oldthread->AddChild(thread);

        // Reset kernel current thread as old thread
        //kernel->currentThread = oldthread;
        //kernel->currentThread->space->RestoreState();
        kernel->threadTable[thread->threadID] = thread;
        
        delete name;
        return thread->threadID;
    }
    else
    {
        // Reset kernel current thread as old thread
        //kernel->currentThread = oldthread;
        //kernel->currentThread->space->RestoreState();
        //kernel->threadTable[thread->threadID] = thread
        delete thread;
        delete name;
        return -1;
    }
}

int SysJoin(int threadId)
{
    if(threadId==-1) return 0;
    if(kernel->currentThread->IsExitedChild(kernel->threadTable[threadId]))
    {
        //kernel->currentThread->SetWaitingChild((Thread*)threadId);
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);	// disable interrupts
        kernel->currentThread->Sleep(FALSE);
        kernel->interrupt->SetLevel(oldLevel);
        //kernel->currentThread->Sleep(FALSE);
    }
    return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
