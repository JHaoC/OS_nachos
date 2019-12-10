#include "syscall.h"


int main()
{
   int i;
   Write("Help test for exec system call and halt machine to show stats including TLB and Page fault !", 93, 0);
   for (i = 0; i < 30; i++)
   {
      Write("Help test in dumming", 21, 0);
   }
   Write("Help test finishs halt and shows stats", 39, 0);
   Halt();
}
