#include "kernel.h"
#include "main.h"
#include "thread.h"
#include "../userprog/lab2.h"

void
ThreadTest()
{
    printf("Lab2 Supermarket\n");
    Supermarket sm = Supermarket();
    sm.Run();
}
