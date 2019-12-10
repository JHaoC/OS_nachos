#include "syscall.h"

void HelpforFork()
{
   Write("help function for testing Fork system call",43,0);
   Exit(0);
}


int
main()
{
  char buffer[100];
  OpenFileId a;
  OpenFileId b;
  //OpenFileId c;

  Write("Task1 start!!\n",15,ConsoleOutputId);
  a = Open("TestFile", 2);
  Write("Test for for reading and writing system call\n",46,a);

  Create("TestFile", 6);
  a = Open("TestFile", 2);
  b = Open("TestFile", 2);
  
  Write("Test for for reading and writing system call\n",46,a);
  Read(buffer, 46,a);
  Write(buffer, 46, ConsoleOutputId);

  Close(a);
  Close(b);
  //Remove("TestFile");
  //c = Open("TestFile", 2);
 
  
  Exit(0);
}
