#include "DataStructure.h"
struct PCQueue *cirQueue;
struct PPQueue *priQueue;
struct PData *runningProcess = NULL;
int quantum, remainingQuantum;
FILE *logFile, *prefFile;
int lastClock;
void logProcessInfo(struct PData *p,int x);

int processCount = 0;
int sumWT = 0;
int sumWTA = 0;
int AvgWTA = 0;
int AvgWT = 0;
int sumRT = 0;
struct Queue WTAQ;

/*    HPF      */
bool runHPF(int x)
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
        printf("Wake up process %d at time %d\n", runningProcess->id, x);
        runningProcess->waittime = x - runningProcess->arrivaltime;
        logProcessInfo(runningProcess,x);
        lastClock = x;
    }else{
        if(lastClock != x){
            lastClock = x;
            runningProcess->remaintime--;
        }
    }
    return false;
}

void PTerminateHPF()
{
    runningProcess->state = finished;
    int x = getClk();
    printf("Terminate process %d at time %d\n", runningProcess->id, x);
    logProcessInfo(runningProcess,x);
    free(runningProcess);
    runningProcess = NULL;
}



/*    SRTN          */
bool runSRTN(int x)
{
    if (runningProcess == NULL && priQueue->count == 0)
    {
        return true;
    }
    if (runningProcess == NULL){
        runningProcess = malloc(sizeof(struct PData));
        frontPQ(priQueue, runningProcess);
        dequeuePQ(priQueue);
        if(runningProcess->state==arrived){
            printf("Wake up process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = started;
            runningProcess->waittime = x - runningProcess->arrivaltime;
        }else if(runningProcess->state==stopped){
            printf("Resume process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = resumed;
        }
        lastClock = x;
        kill(runningProcess->pid, SIGCONT);
        logProcessInfo(runningProcess,x);
        
    }else{
       if(lastClock!=x){
        lastClock=x;
        runningProcess->remaintime--;
       }
        struct PData *gProcess = NULL;
        gProcess = malloc(sizeof(struct PData));
        frontPQ(priQueue, gProcess);
        if( priQueue->count == 0 ||(priQueue->count != 0)&& (runningProcess->remaintime <= gProcess->remaintime)){
            free(gProcess);
            return false;
        }else{
            if (runningProcess->remaintime == 0)
            {
                free(gProcess);
                return false;
            }
            printf("Stop process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = stopped;
            logProcessInfo(runningProcess,x);
            kill(runningProcess->pid, SIGSTOP);
            enqueuePQ(*runningProcess,runningProcess->remaintime, priQueue);
            free(runningProcess);
            runningProcess = NULL;
        }
        free(gProcess);
    }
    
    return false;
}

void PTerminateSRTN()
{
    runningProcess->state = finished;
    int x = getClk();
    printf("Terminate process %d at time %d\n", runningProcess->id, x);
    logProcessInfo(runningProcess,x);
    struct PData *temp = runningProcess;
    runningProcess = NULL;
    free(temp);
}

/*    RR      */
bool runRR(int x)
{
   

    // if there is no ruunning process
    // happens after termination of a process and at first time
    // and after stopping a process
    if (runningProcess == NULL)
    {

        // if there is no process in the queue return
        if (cirQueue->count == 0)
        {
            return true;
        }
        // if we have a process in the queue
        runningProcess = malloc(sizeof(struct PData));
        // get the first process in the queue
        frontCQ(cirQueue, runningProcess);
        dequeueCQ(cirQueue);
        if (runningProcess->state == arrived)
        {
            // reset the quantum
            remainingQuantum = quantum;
            lastClock = x;
            printf("Wake up process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = started;
            runningProcess->waittime = x - runningProcess->arrivaltime;
        }
        else if (runningProcess->state == stopped)
        {
            // reset the quantum
            remainingQuantum = quantum;
            lastClock = x;
            printf("Resume process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = resumed;
        }
        kill(runningProcess->pid, SIGCONT);
        logProcessInfo(runningProcess,x);
    }
    else
    {
        // if there is a running process
        // check if the quantum is finished
        // and process is not going to terminate
        if (remainingQuantum == 0 && runningProcess != NULL)
        {
            if (runningProcess->remaintime == 0)
            {
                return false;
            }
            // stop the process
            printf("Stop process %d at time %d\n", runningProcess->id, x);
            runningProcess->state = stopped;
            logProcessInfo(runningProcess,x);
            kill(runningProcess->pid, SIGSTOP);

            // add the process to the queue
            enqueueCQ(*runningProcess, cirQueue);
            free(runningProcess);
            runningProcess = NULL;
        }
        else
        {

            // decrement the quantum
            if (lastClock != x)
            {
                if (runningProcess != NULL)
                {
                    runningProcess->remaintime--;
                }
                remainingQuantum--;
                lastClock = x;
            }
        }
    }
    return false;
}

void PTerminateRR()
{
    runningProcess->state = finished;
    int x = getClk();
    printf("Terminate process %d at time %d\n", runningProcess->id, x);
    logProcessInfo(runningProcess,x);
    struct PData *temp = runningProcess;
    runningProcess = NULL;
    free(temp);
}

void logProcessInfo(struct PData *p,int x)
{
    int wait = x - p->arrivaltime - p->runningtime + p->remaintime;
    switch (p->state)
    {
    case 1: // started
        fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
                x, p->id, p->arrivaltime, p->runningtime,
                p->remaintime, wait);
        break;

    case 2: // resumed
        fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                x, p->id, p->arrivaltime, p->runningtime, p->remaintime, wait);
        break;

    case 3: // stopped
        fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
                x, p->id, p->arrivaltime, p->runningtime, p->remaintime, wait);
        break;

    case 4: // finished
        int TA = x - p->arrivaltime;
        float WTA = (TA + 0.0) / p->runningtime;
        sumWTA += WTA;
        sumWT += wait;
        enQueue(&WTAQ, WTA);
        fprintf(logFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n",
                x, p->id, p->arrivaltime, p->runningtime, wait, TA, WTA);
        break;
    }
}