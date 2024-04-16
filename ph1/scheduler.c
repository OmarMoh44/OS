#include "headers.h"

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        printf("Scheduler with algo %d and quantum %d\n", atoi(argv[1]), atoi(argv[2]));
    }
    else
    {
        printf("Scheduler with algo %d\n", atoi(argv[1]));
    }
    initClk();

    // TODO implement the scheduler :)
    while (true)
    {
        /* code */
    }

    // upon termination release the clock resources.
    destroyClk(true);
    return 0;
}
