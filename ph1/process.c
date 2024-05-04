#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int x, y;

void Processing()
{

    x = getClk();
    printf("Start at time %d with remaining %d , process id %d\n", x, remainingtime, getpid());
    while (remainingtime > 0)
    {
        y = getClk();
        if (y == x)
            continue;
        if (y - x == 1)
        {
            remainingtime--;
            printf("At time %d with remaining %d , process id %d\n", y, remainingtime, getpid());
            // usleep(250000);
        }
        x = y;
    }
}

void handler(int signum)
{
    x = getClk() + 1;
    signal(SIGCONT, handler);
}

int main(int argc, char *argv[])
{
    signal(SIGCONT, handler);
    remainingtime = atoi(argv[1]);
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    Processing();
    printf("Process terminate at time %d , process id %d\n", x, getpid());
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
