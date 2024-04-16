#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int argc, char *argv[])
{
    printf("I am process with remaining time %d\n", atoi(argv[1]));
    initClk();
    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    remainingtime = atoi(argv[1]);
    while (remainingtime > 0)
    {
        // remainingtime = ??;
    }

    destroyClk(false);
    return 0;
}
