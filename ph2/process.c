#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int x, y;

void Processing()
{
    x = getClk();
    printf("Start at time %d\n", x);
    while (remainingtime > 0)
    {
        remainingtime--;
        for (int i = 0; i < 50; i++)
        {
            usleep(20000-1);
        }
    }
}

void handler(int)
{
    // Processing();
}

int main(int argc, char *argv[])
{
    signal(SIGCONT, handler);
    remainingtime = atoi(argv[1]);
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    Processing();
    printf("Process terminate at time %d\n", x);
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
