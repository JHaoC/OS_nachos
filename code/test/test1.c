#include "syscall.h"

int main()
{
    int i;
    for ( i = 0; i < 20; i++)
    {
        Write("test 1: Help test for Task 1 and Task 2", 40, 0);
    }
    Write("test 1: finish 20 rounds", 25, 0);
    Exit(0);
}