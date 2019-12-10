// kernel.cc
//	Initialization and cleanup routines for the Nachos kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "main.h"
#include "synch.h"
#include "kernel.h"
#include "sysdep.h"
#include "synchlist.h"
#include "libtest.h"
#include "string.h"
#include "synchconsole.h"
#include "synchdisk.h"
#include "filetable.h"
#include "post.h"

//----------------------------------------------------------------------
// Kernel::Kernel
// 	Interpret command line arguments in order to determine flags
//	for the initialization (see also comments in main.cc)
//----------------------------------------------------------------------

Kernel::Kernel(int argc, char **argv)
{
<<<<<<< HEAD
=======
    quantum = TimerTicks;
>>>>>>> assignmentThird
    randomSlice = FALSE;
    debugUserProg = FALSE;
    consoleIn = NULL;  // default is stdin
    consoleOut = NULL; // default is stdout
#ifndef FILESYS_STUB
    formatFlag = FALSE;
#endif
    reliability = 1; // network reliability, default is 1.0
    hostName = 0;    // machine id, also UNIX socket name
                     // 0 is the default machine id
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-rs") == 0)
        {
            ASSERT(i + 1 < argc);
            RandomInit(atoi(argv[i + 1])); // initialize pseudo-random
                // number generator
            randomSlice = TRUE;
            i++;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            debugUserProg = TRUE;
        }
        else if (strcmp(argv[i], "-ci") == 0)
        {
            ASSERT(i + 1 < argc);
            consoleIn = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-co") == 0)
        {
            ASSERT(i + 1 < argc);
            consoleOut = argv[i + 1];
            i++;
<<<<<<< HEAD
#ifndef FILESYS_STUB
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            formatFlag = TRUE;
#endif
        }
=======
        }

        // -randomselect setting 
        else if (strcmp(argv[i], "-Randomselected") == 0) {
            randomselect = TRUE;
            std::cout<< "Seting Random pop page if need"<< std::endl;
        }

#ifndef FILESYS_STUB
        
        else if (strcmp(argv[i], "-f") == 0)
        {
            formatFlag = TRUE;
        }
#endif
        
>>>>>>> assignmentThird
        else if (strcmp(argv[i], "-n") == 0)
        {
            ASSERT(i + 1 < argc); // next argument is float
            reliability = atof(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            ASSERT(i + 1 < argc); // next argument is int
            hostName = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
            cout << "Partial usage: nachos [-rs randomSeed]\n";
            cout << "Partial usage: nachos [-s]\n";
            cout << "Partial usage: nachos [-ci consoleIn] [-co consoleOut]\n";
#ifndef FILESYS_STUB
            cout << "Partial usage: nachos [-nf]\n";
#endif
            cout << "Partial usage: nachos [-n #] [-m #]\n";
        }
    }
}

//----------------------------------------------------------------------
// Kernel::Initialize
// 	Initialize Nachos global data structures.  Separate from the
//	constructor because some of these refer to earlier initialized
//	data via the "kernel" global variable.
//----------------------------------------------------------------------

<<<<<<< HEAD
void Kernel::Initialize()
=======
void Kernel::Initialize(int quantum)
>>>>>>> assignmentThird
{
    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state.
<<<<<<< HEAD
=======

    // set for quantum
    this->quantum = quantum;

    // Global file table setting up
    globalFileTable = new FileEntryTable();
    //globalFileTable->InsertEntry(NULL); // for ConsoleIn
    //globalFileTable->InsertEntry(NULL); //for ConsoleOur

>>>>>>> assignmentThird
    currentThread = new Thread("main");
    currentThread->setStatus(RUNNING);

    stats = new Statistics();       // collect statistics
    interrupt = new Interrupt;      // start up interrupt handling
    scheduler = new Scheduler();    // initialize the ready queue
    alarm = new Alarm(randomSlice); // start up time slicing
    machine = new Machine(debugUserProg);
    synchConsoleIn = new SynchConsoleInput(consoleIn);    // input from stdin
    synchConsoleOut = new SynchConsoleOutput(consoleOut); // output to stdout
<<<<<<< HEAD
    synchDisk = new SynchDisk();                          //
=======
    synchDisk = new SynchDisk();
    io_lock = new Lock("atomic_io_lock");
    directoryFile_lock = new Lock("directoryFile_lock");
    freeMapFile_lock = new Lock("freeMapFile_lock");
    threadTable = new Thread*[40]; // Max_thread can have for userprog
    for(int i = 0; i < 40; i++)
    {
        threadTable[i] = NULL;
    }
    

>>>>>>> assignmentThird
#ifdef FILESYS_STUB
    fileSystem = new FileSystem();

    // create a new file as swap
    if (fileSystem->Create("swapspace"))
    {
        swapspace = fileSystem->Open("swapspace");
    }

#else
    fileSystem = new FileSystem(formatFlag);
    // create a new file as swap
    if((swapspace=fileSystem->Open("swapspace",2))==NULL)
    {
        fileSystem->Create("swapspace", 0, 6);
        swapspace = fileSystem->Open("swapspace",2);
    }
#endif // FILESYS_STUB
<<<<<<< HEAD
    // postOfficeIn = new PostOfficeInput(10);
    // postOfficeOut = new PostOfficeOutput(reliability);
=======
    //postOfficeIn = new PostOfficeInput(10);
    //postOfficeOut = new PostOfficeOutput(reliability);
>>>>>>> assignmentFirst

    swapCounter = 0;
    PageContainer = new List<TranslationEntry *>();
    freeMap = new Bitmap(NumPhysPages);

    interrupt->Enable();
}

//----------------------------------------------------------------------
// Kernel::~Kernel
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------

Kernel::~Kernel()
{
    delete stats;
    delete interrupt;
    delete scheduler;
    delete alarm;
    delete machine;
    delete synchConsoleIn;
    delete synchConsoleOut;
    delete synchDisk;
    delete fileSystem;
    delete postOfficeIn;
    delete postOfficeOut;
<<<<<<< HEAD

=======
    delete swapspace;
    delete freeMap;
    delete PageContainer;
    delete io_lock;
    delete directoryFile_lock;
    delete freeMapFile_lock;
    delete globalFileTable;
    delete threadTable;
>>>>>>> assignmentThird
    Exit(0);
}

//----------------------------------------------------------------------
// Kernel::ThreadSelfTest
//      Test threads, semaphores, synchlists
//----------------------------------------------------------------------

void Kernel::ThreadSelfTest()
{
    Semaphore *semaphore;
    SynchList<int> *synchList;
<<<<<<< HEAD

    LibSelfTest();             // test library routines
    currentThread->SelfTest(); // test thread switching

=======

    LibSelfTest();             // test library routines
    currentThread->SelfTest(); // test thread switching

>>>>>>> assignmentThird
    // test semaphore operation
    semaphore = new Semaphore("test", 0);
    semaphore->SelfTest();
    delete semaphore;

    // test locks, condition variables
    // using synchronized lists
    synchList = new SynchList<int>;
    synchList->SelfTest(9);
    delete synchList;
}

//----------------------------------------------------------------------
// Kernel::ConsoleTest
//      Test the synchconsole
//----------------------------------------------------------------------

void Kernel::ConsoleTest()
{
    char ch;

    cout << "Testing the console device.\n"
         << "Typed characters will be echoed, until ^D is typed.\n"
         << "Note newlines are needed to flush input through UNIX.\n";
    cout.flush();

    do
    {
        ch = synchConsoleIn->GetChar();
        if (ch != EOF)
            synchConsoleOut->PutChar(ch); // echo it!
    } while (ch != EOF);

    cout << "\n";
}

//----------------------------------------------------------------------
// Kernel::NetworkTest
//      Test whether the post office is working. On machines #0 and #1, do:
//
//      1. send a message to the other machine at mail box #0
//      2. wait for the other machine's message to arrive (in our mailbox #0)
//      3. send an acknowledgment for the other machine's message
//      4. wait for an acknowledgement from the other machine to our
//          original message
//
//  This test works best if each Nachos machine has its own window
//----------------------------------------------------------------------

void Kernel::NetworkTest()
{

    if (hostName == 0 || hostName == 1)
    {
        // if we're machine 1, send to 0 and vice versa
        int farHost = (hostName == 0 ? 1 : 0);
        PacketHeader outPktHdr, inPktHdr;
        MailHeader outMailHdr, inMailHdr;
        char *data = "Hello there!";
        char *ack = "Got it!";
        char buffer[MaxMailSize];

        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        outPktHdr.to = farHost;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOfficeOut->Send(outPktHdr, outMailHdr, data);

        // Wait for the first message from the other machine
        postOfficeIn->Receive(0, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box "
             << inMailHdr.from << "\n";
        cout.flush();

        // Send acknowledgement to the other machine (using "reply to" mailbox
        // in the message that just arrived
        outPktHdr.to = inPktHdr.from;
        outMailHdr.to = inMailHdr.from;
        outMailHdr.length = strlen(ack) + 1;
        postOfficeOut->Send(outPktHdr, outMailHdr, ack);

        // Wait for the ack from the other machine to the first message we sent
        postOfficeIn->Receive(1, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box "
             << inMailHdr.from << "\n";
        cout.flush();
    }

    // Then we're done!
}
