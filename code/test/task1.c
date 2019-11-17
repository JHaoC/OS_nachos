#include "syscall.h"

void HelpforFork()
{
   Write("help function for testing Fork system call",43,0);
   Exit(0);
}


int
main()
{
  //buffer helping for read system call
  char buffer[100];
  
  //Check write system call
  Write("Test for write system call!", 28, 0);

  //Check read system call
  //There is secret words"This is a test for read system call for the assignment 2.";
  //Read() can read it and partial or entire depending on the size into buffer 
  
  //the size is less than words
  Read(buffer,4,1);
  Write(buffer,4,0);
  // the size larger than words
  Read(buffer,65,1);
  Write(buffer,65,0);
  
  //Check fork system
  Write("Checking for fork system",25,0);
  ThreadFork((void*)HelpforFork);

  // Check exec system call to execute test1, test2 and helpTest
  // In the helpTest there is halt() in the end which shows all stats
  // for TLB missing and page fault
  Exec("../test/test1");
  Exec("../test/helpTest"); 
  Exit(0);
  /* not reached */
}
