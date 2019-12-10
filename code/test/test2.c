#include "syscall.h"

int main()
{
    OpenFileId a;
    int i;
    i = 20;
    while(i!=0)
    {
        a = Open("TestFile",1);
        if(a != -1)
        {
            Close(a);
            Remove("TestFile");
            break;
        }
        i--;
    }
    Exit(0);
}