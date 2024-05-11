#include "Algorithms.h"

int algo = -1;
int msq_id;

bool stopRcv = false;    // stop receiving from process generator when receive process data with id = -1
bool finishSched = true; // at first as ready queue is empty and there is no running process
void mainLoop();
void initialResource(); // create message queue
bool AddProcess(struct PData p);
bool runSched(int x);
void ClearResou(int);
void PTerminate(int);
void intitateFiles();
int CountDigit(int x); // count number of digits of integer to convert it to string
void ChangeFlag();
FILE *memoryfile;
int main(int argc, char *argv[])
{
    createTree();
    WTAQ.front = NULL;
    WTAQ.rear = NULL;
    WTAQ.count = 0;
    signal(SIGINT, ClearResou);  // free queues at end of program
    signal(SIGUSR1, ChangeFlag); // handler action when process terminate and notify parent
    initialResource();
    initClk();

    waitingQueue = createPQ();
    algo = atoi(argv[1]);
    // TODO implement the scheduler :)
    if (algo == RR)
    {
        quantum = atoi(argv[2]);
        cirQueue = createCQ();
    }
    else
    {
        priQueue = createPQ();
    }

    printf("Scheduler start\n");

    intitateFiles();
    mainLoop();
    // upon termination release the clock resources.
    destroyClk(true);
    // raise(SIGINT);
    killpg(getpgrp(), SIGINT);
}

void intitateFiles()
{
    memoryfile = fopen("memory.log", "w");
    if (memoryfile == NULL)
    {
        perror("Can not open memory.log file\n");
        exit(-1);
    }

    fprintf(memoryfile, "#At time x allocated y bytes for process z from i to j\n");

    logFile = fopen("scheduler.log", "w");
    if (logFile == NULL)
    {
        perror("Can not open scheduler.log file\n");
        exit(-1);
    }
    fprintf(logFile, "#At time x process y state arr w total z remain y wait k\n");
    prefFile = fopen("scheduler.pref", "w");
    if (prefFile == NULL)
    {
        perror("Can not open scheduler.pref file\n");
        exit(-1);
    }
}

void initialResource()
{
    int msq_id_key = ftok("keyfile.txt", 'A');
    msq_id = msgget(msq_id_key, IPC_CREAT | 0644);
    if (msq_id == -1)
    {
        perror("Error in initializing message queue\n");
        exit(-1);
    }
}

bool AddProcess(struct PData p)
{

    // allocate memory
    bool canAllocate = allocateMemory(&p);
    if (!canAllocate)
    {
        printf("Process %d can't allocate memory waiting with pid %d at time %d\n", p.id, p.pid, getClk());
        // add to waiting list
        enqueuePQ(p, p.memorySize, waitingQueue);
        return false;
    }
    int y = getClk();

    fprintf(memoryfile, "At time %d allocated %d bytes from process %d from %d to %d\n", y, p.memorySize, p.id, p.memoryStart, p.memoryEnd);

    char st[CountDigit(p.runningtime) + 2];
    sprintf(st, "%d", p.runningtime); // convert runningtime to string to pass it for process program
    pid_t x = fork();
    if (x == -1)
    {
        perror("Error in forking process to execute process\n");
        exit(EXIT_FAILURE);
    }
    else if (x == 0)
    {
        execl("./process.out", "process.out", st, NULL);
        perror("Error in executing process.out\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        p.state = arrived;
        p.pid = x;
    }
    switch (algo)
    {
    case RR:
        enqueueCQ(p, cirQueue);
        break;
    case SRTN:
        enqueuePQ(p, p.remaintime, priQueue);
        break;
    case HPF:
        enqueuePQ(p, p.priority, priQueue);
        break;
    default:
        break;
    }
    kill(x, SIGTSTP);
    return true;
}

bool runSched(int x)
{
    switch (algo)
    {
    case RR:
        return runRR(x);
    case SRTN:
        return runSRTN(x);
    case HPF:
        return runHPF(x);
    default:
        break;
    }
}

void ClearResou(int)
{
    signal(SIGINT, ClearResou);
    free(priQueue);
    free(cirQueue);
    fclose(logFile);
    fclose(prefFile);
    fclose(memoryfile);
    exit(0);
}

void PTerminate(int)
{

    // remove the process from the memory
    deallocateMemory(runningProcess);
    int x = getClk();

    fprintf(memoryfile, "At time %d freed %d bytes from process %d from %d to %d\n",
            x, runningProcess->memorySize, runningProcess->id, runningProcess->memoryStart, runningProcess->memoryEnd);

    switch (algo)
    {
    case RR:
        PTerminateRR();
        break;
    case SRTN:
        PTerminateSRTN();
        break;
    case HPF:
        PTerminateHPF();
        break;
    default:
        break;
    }
    bool reAllocate = true;
    while (waitingQueue->count != 0 && reAllocate)
    {
        // take one from waiting list and try to add it
        struct PData *p = malloc(sizeof(struct PData));
        frontPQ(waitingQueue, p);
        dequeuePQ(waitingQueue);
        reAllocate = AddProcess(*p);
    }
    printf("YES, DDDod\n");
    // signal(SIGUSR1, PTerminate);
}

int CountDigit(int x)
{
    int count = 0;
    while (x >= 10)
    {
        count++;
        x /= 10;
    }
    return ++count;
}

void ChangeFlag()
{
    printf("ChangeFlag\n");
    flag = true;
    signal(SIGUSR1, ChangeFlag);
}

void mainLoop()
{

    while (!stopRcv || !finishSched || waitingQueue->count != 0)
    {
        struct PData process;
        int rcv = msgrcv(msq_id, &process, sizeof(struct PData), 0, IPC_NOWAIT);
        int x = getClk();
        if (rcv == -1)
        {
        }
        else if (process.id == -1)
        {
            printf("Finish receiveing\n");
            stopRcv = true;
        }
        else
        {

            printf("Receive process at time %d\n", x);
            printf(" process memory %d\n", process.memorySize);
            process.state = arrived;
            sumRT += process.runningtime;
            AddProcess(process);
            continue;
        }
        finishSched = runSched(x);
        if (flag)
        {
            PTerminate(x);
            flag = false;
        }
    }
    float avgWTA = (sumWTA) / WTAQ.count;
    printf("sumWTA = %f\n", sumWTA);
    float avgWT = (sumWT + 0.0) / (float)WTAQ.count;
    printf("sumTA = %f\n", sumWTA);
    float SD = 0;
    int tempCount = WTAQ.count;
    printf("tempCount = %d\n", tempCount);
    float cpuUTI = (float)sumRT / (float)getClk() * 100;
    while (WTAQ.count != 0)
    {
        float x = deQueue(&WTAQ);
        SD += pow(x - avgWTA, 2);
        printf("power = %f with x = %f\n", pow(x - avgWTA, 2), x);
    }
    printf("SD = %f\n", SD);
    SD = sqrt(SD / tempCount);
    printf("Std = %f\n", SD);
    fprintf(prefFile, "CPU Utilization = %f%% \nAvg WTA = %f\nAvg WT = %f\nStd WTA = %f\n", cpuUTI, avgWTA, avgWT, SD);
    printf("OUT of main loop in scheduler\n");
}
