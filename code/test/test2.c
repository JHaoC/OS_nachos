#include "syscall.h"

int main()
{
    int i;
    for ( i = 0; i < 10; i++)
    {
        Write("test 2: Help test for Task 2", 29, 0);
    }
    Write("test 2: finish 10 rounds", 25, 0);
    Exit(0);
}