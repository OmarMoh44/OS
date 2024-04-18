#include "Algorithms.h"

int algo = -1;
int q = -1;
int msq_id;

bool stopRcv = false;    // stop receiving from process generator when receive process data with id = -1
bool finishSched = true; // at first as ready queue is empty and there is no running process

void initialResource(); // create message queue
void AddProcess(struct PData p);
bool runSched();
void ClearResou(int);
void PTerminate(int);
int CountDigit(int x); // count number of digits of integer to convert it to string

int main(int argc, char *argv[])
{
    signal(SIGINT, ClearResou);  // free queues at end of program
    signal(SIGUSR1, PTerminate); // handler action when process terminate and notify parent
    initialResource();
    initClk();
    algo = atoi(argv[1]);
    // TODO implement the scheduler :)
    if (algo == RR)
    {
        q = atoi(argv[2]);
        cirQueue = createCQ();
    }
    else
    {
        priQueue = createPQ();
    }
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
    fprintf(logFile, "#At time x process y state arr w total z remain y wait k\n");
    mainLoop();
    // upon termination release the clock resources.
    destroyClk(true);
    raise(SIGINT);
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

void AddProcess(struct PData p)
{
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
}

bool runSched()
{
    switch (algo)
    {
    case RR:
        return runRR();
    case SRTN:
        return runSRTN();
    case HPF:
        return runHPF();
    default:
        break;
    }
}

void ClearResou(int)
{
    free(priQueue);
    free(cirQueue);
    fclose(logFile);
    fclose(prefFile);
    exit(0);
}

void PTerminate(int)
{
    switch (algo)
    {
    case RR:
        PTerminateRR();
    case SRTN:
        PTerminateSRTN();
    case HPF:
        PTerminateHPF();
    default:
        break;
    }
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

void mainLoop()
{
    while (!stopRcv || !finishSched)
    {
        struct PData process;
        int rcv = msgrcv(msq_id, &process, sizeof(struct PData), 0, IPC_NOWAIT);
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
            int x = getClk();
            printf("Receive process at time %d\n", x);
            process.state = arrived;
            AddProcess(process);
            continue;
        }
        finishSched = runSched();
    }
    printf("OUT of main loop in scheduler\n");
}
