#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int argc, char *argv[])
{
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