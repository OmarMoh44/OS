#include "DataStructure.h"
struct PCQueue *cirQueue;
struct PPQueue *priQueue;
struct PData *runningProcess = NULL;
FILE *logFile, *prefFile;

void mainLoop();

void writeOutput()
{
    logFile = fopen("scheduler.log.txt", "w");
    if (logFile == NULL)
    {
        perror("Can not open scheduler.log file\n");
        exit(-1);
    }
    prefFile = fopen("scheduler.pref.txt", "w");
    if (prefFile == NULL)
    {
        perror("Can not open scheduler.pref file\n");
        exit(-1);
    }
}

/*    HPF      */
bool runHPF()
{
    if (runningProcess == NULL && priQueue->count == 0)
    {
        return true;
    }
    if (runningProcess == NULL)
    {
        runningProcess = malloc(sizeof(struct PData));
        frontPQ(priQueue, runningProcess);
        dequeuePQ(priQueue);
        runningProcess->state = started;
        kill(runningProcess->pid, SIGCONT);
        int x = getClk();
        printf("Wake up process %d at time %d\n", runningProcess->id, x);
        runningProcess->waittime = x - runningProcess->arrivaltime;

        fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
                x, runningProcess->id, runningProcess->arrivaltime, runningProcess->runningtime,
                runningProcess->remaintime, runningProcess->waittime);
    }
    return false;
}

void PTerminateHPF()
{
    runningProcess->state = finished;
    int x = getClk();
    printf("Terminate process %d at time %d\n", runningProcess->id, x);
    fprintf(logFile, "At time %d process %d finished arr %d total %d remain 0 wait %d\n",
            x, runningProcess->id, runningProcess->arrivaltime,
            runningProcess->runningtime, runningProcess->waittime);
    free(runningProcess);
    runningProcess = NULL;
}

/*    SRTN          */
bool runSRTN()
{
}

void PTerminateSRTN()
{
}

/*    RR      */
bool runRR()
{
}

void PTerminateRR()
{
}
