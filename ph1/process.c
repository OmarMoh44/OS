#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int x, y;

void Decrement(int)
{
    remainingtime--;
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, Decrement);
    remainingtime = atoi(argv[1]);
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    while (remainingtime > 0)
    {
    }
    printf("Process terminate at time %d , process id %d\n", x, getpid());
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
