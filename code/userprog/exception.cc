// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	if (which == SyscallException)
	{

		if (type == SC_Halt)
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
		}
		else if (type == SC_Add)
		{

			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();
		}

		/* Read systemcall*/
		else if (type == SC_Read)
		{
			DEBUG(dbgSys, "Read from console to buffer" << kernel->machine->ReadRegister(4) << " total size is " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysRead Systemcall*/
			int result;
			result = SysRead(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Read returning num of bytes actually read with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

		}

		/* Write systemcall*/
		else if (type == SC_Write)
		{
			/* code */
			DEBUG(dbgSys, "Write buffer into console" << kernel->machine->ReadRegister(4) << " total size is " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysWrite Systemcall*/
			int result;
			result = SysWrite(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			DEBUG(dbgSys, "Read returning num of bytes actually read with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;

			ASSERTNOTREACHED();
		}

		/* Fork systemcall*/
		else if (type == SC_ThreadFork)
		{
			/* code */
			DEBUG(dbgSys, "Fork" << kernel->machine->ReadRegister(4) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysFork(kernel->machine->ReadRegister(4));
			DEBUG(dbgSys, "returning threadID if success otherwise -1 " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;

			ASSERTNOTREACHED();
		}

		/* Exit systemcall*/
		else if (type == SC_Exit)
		{
			/* code */
			DEBUG(dbgSys, "Exit" << kernel->machine->ReadRegister(4) << "\n");

			/* Process SysAdd Systemcall*/
			int statue = kernel->machine->ReadRegister(4);
			SysExit(statue);
			DEBUG(dbgSys, "Exit thread\n");
			/* Modify return point */
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;

			ASSERTNOTREACHED();
		}

		/* Exev systemcall*/
		else if (type == SC_Exec)
		{
			/* code */
			DEBUG(dbgSys, "Exec" << kernel->machine->ReadRegister(4) << "\n");

			/* Process Sysexev Systemcall*/
			SpaceId res = SysExec(kernel->machine->ReadRegister(4));
			DEBUG(dbgSys, "Exec" << res<< "\n");
			/* Modify return point */
			kernel->machine->WriteRegister(2, (int)res);
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;

			ASSERTNOTREACHED();
		}

		else
		{
			cerr << "Unexpected system call " << type << "\n";
		}
	}
	// else if(which == TLBMissException)
	// {
	// 	// increase TLB miss counter
	// 	kernel->stats->numTLBMiss++;

	// 	std::cout << "TLB MissException by "<< kernel->currentThread->getName()<<" " << kernel->currentThread->threadID <<std::endl;
	// 	int virAdd = kernel->machine->ReadRegister(BadVAddrReg);
	// 	// Get VPN
	// 	unsigned int vpn = (unsigned)virAdd/PageSize;

	// 	// if in the page table update TLB
	// 	if (kernel->machine->pageTable[vpn].valid)
	// 	{
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].virtualPage = vpn;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].physicalPage = kernel->machine->pageTable[vpn].physicalPage;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].valid = kernel->machine->pageTable[vpn].valid;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].readOnly = kernel->machine->pageTable[vpn].readOnly;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].dirty = kernel->machine->pageTable[vpn].dirty;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].threadID = kernel->machine->pageTable[vpn].threadID;
	// 		kernel->machine->tlb[kernel->machine->tlbCounter].use = kernel->machine->pageTable[vpn].use;
			

	// 		std::cout << "Program on the mainMemory and Using "<< kernel->machine->tlbCounter <<" TLB" <<std::endl<<std::endl;
			
	// 		kernel->machine->tlbCounter++;
	// 		if(kernel->machine->tlbCounter == TLBSize)
	// 		{
	// 			kernel->machine->tlbCounter = 0;
	// 		}
	// 		return;
	// 	}
		
	// 	//std::cout << "Program miss on the pagetable"<<std::endl;
	// 	ExceptionHandler(PageFaultException);
	// 	return;

	// }
	else if(which == PageFaultException)
	{
		// increase Pagefault miss counter
		kernel->stats->numPageFaults++;

		std::cout << "PageFaultException MissException by "<< kernel->currentThread->getName()<< " " << kernel->currentThread->threadID <<std::endl;
		int pagetableIndex = kernel->freeMap->FindAndSet();
		
		if(pagetableIndex == -1)
		{
			// select a page depend on the page container
			TranslationEntry* victimpage;
			if(kernel->randomselect)
			{
				int index = rand() % kernel->PageContainer->NumInList();
				ListIterator<TranslationEntry*>* item = new ListIterator<TranslationEntry*>(kernel->PageContainer);
				for(int i = 0; i < index; i ++)
				{
					item->Next();
				}
				victimpage = item->Item();
				kernel->PageContainer->Remove(victimpage);
				delete item;
			}
			else
			{
				victimpage = kernel->PageContainer->RemoveFront();
			}
			pagetableIndex = victimpage->physicalPage;

			// Chech write back swap if dirty 
			if(victimpage->dirty)
			{
				// rewrite back swap and update victim page
				std::cout << "Page "<< pagetableIndex << " is writed, need write back to swap space"<<std::endl;

            	kernel->swapspace->WriteAt(&(kernel->machine->mainMemory[victimpage->physicalPage * PageSize]), PageSize, victimpage->virtualPage * PageSize);
				victimpage->dirty = FALSE;
				
			}

			// modify the page
			victimpage->valid = FALSE;
			victimpage->physicalPage = -1;

			// // check victimpage is in TLB no not
			// for(int i = 0; i < TLBSize; i++)
			// {
			// 	if(kernel->machine->tlb[i].virtualPage == pagetableIndex && kernel->machine->tlb[i].threadID == victimpage->threadID)
			// 	{
			// 		kernel->machine->tlb[i].valid = FALSE;
			// 		kernel->machine->tlb[i].physicalPage = -1;
			// 		break;
			// 	}
			// }
			
		}
		
		// Set new page and load page into mainmemory
		// Get current user program virtual address
		int virAdd = kernel->machine->ReadRegister(BadVAddrReg);
		// Get VPN
		unsigned int vpn = (unsigned)virAdd/PageSize;

		// Set pageTable 
		kernel->machine->pageTable[vpn].physicalPage = pagetableIndex;
		kernel->machine->pageTable[vpn].valid = TRUE;

		// update page container
		if(kernel->PageContainer->IsInList(&(kernel->machine->pageTable[vpn])))
		{
			kernel->PageContainer->Remove(&(kernel->machine->pageTable[vpn]));
		}
		kernel->PageContainer->Append(&(kernel->machine->pageTable[vpn]));

		// // update TLB
		// kernel->machine->tlb[kernel->machine->tlbCounter].virtualPage = vpn;
		// kernel->machine->tlb[kernel->machine->tlbCounter].physicalPage = kernel->machine->pageTable[vpn].physicalPage;
		// kernel->machine->tlb[kernel->machine->tlbCounter].valid = kernel->machine->pageTable[vpn].valid;
		// kernel->machine->tlb[kernel->machine->tlbCounter].readOnly = kernel->machine->pageTable[vpn].readOnly;
		// kernel->machine->tlb[kernel->machine->tlbCounter].dirty = kernel->machine->pageTable[vpn].dirty;
		// kernel->machine->tlb[kernel->machine->tlbCounter].threadID = kernel->machine->pageTable[vpn].threadID;
		// kernel->machine->tlb[kernel->machine->tlbCounter].use = kernel->machine->pageTable[vpn].use;
		
		// //std::cout << "write into TLB on "<< kernel->machine->tlbCounter <<std::endl << std::endl;

		// kernel->machine->tlbCounter++;
		// if(kernel->machine->tlbCounter == TLBSize)
		// {
		// 	kernel->machine->tlbCounter = 0;
		// }


		// clear the mainMemory which are before write
		//bzero(kernel->machine->mainMemory+(kernel->machine->pageTable[vpn].physicalPage * PageSize), PageSize);
		// Load code into memory
		kernel->swapspace->ReadAt(
				&(kernel->machine->mainMemory[kernel->machine->pageTable[vpn].physicalPage * PageSize]),
				PageSize, kernel->machine->pageTable[vpn].virtualPage * PageSize);
		
		std::cout << "Switch page "<< pagetableIndex<< std::endl;
		return;
		

	}
	
	else
	{
		/* code */
		cerr << "Unexpected user mode exception" << (int)which << "\n";
	}
	ASSERTNOTREACHED();
}
