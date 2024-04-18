#include "DataStructure.h"

void clearResources(int);

struct PQueue *readInputFile(); // reading data of processes and store in a buffer
void SelAlgo();                 // read input from user to select algo and parameter if exist
int CountDigit(int x);          // count number of digits of integer to convert it to string
void initialResource();         // create message queue

int algo = -1;
int quantum = -1; // for only RR algo
struct PQueue *proc;
int msq_id; // message queue to send processes at appropriate time

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    proc = readInputFile();
    int process_nums = proc->count;

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    /*  1 -> HPF      2 -> SRTN     3 -> RR */
    SelAlgo();

    // 3. Initiate and create the scheduler and clock processes.
    pid_t clk_pid = fork();
    if (clk_pid == -1)
    {
        perror("Error in forking process to execute clk\n");
        exit(EXIT_FAILURE);
    }
    else if (clk_pid == 0)
    {
        execl("./clk.out", "clk.out", NULL);
        perror("Error in executing clk.out\n");
        exit(EXIT_FAILURE);
    }

    pid_t sched_pid = fork();
    if (sched_pid == -1)
    {
        perror("Error in forking process to execute scheduler\n");
        exit(EXIT_FAILURE);
    }
    else if (sched_pid == 0)
    {
        char st[2];
        sprintf(st, "%d", algo); // convert algo to string to pass it for scheduler program
        if (algo != 3)
        {
            execl("./scheduler.out", "scheduler.out", st, NULL);
        }
        else
        {
            char st2[CountDigit(quantum) + 2];
            sprintf(st2, "%d", quantum); // convert quantum to string to pass it for scheduler program
            execl("./scheduler.out", "scheduler.out", st, st2, NULL);
        }

        perror("Error in executing scheduler.out\n");
        exit(EXIT_FAILURE);
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    initialResource(); // initialize IPCs
    while (proc->count != 0)
    {
        x = getClk();
        if (proc->head->val.arrivaltime <= x)
        {
            struct PData process;
            frontQ(proc, &process);
            msgsnd(msq_id, &process, sizeof(struct PData), !IPC_NOWAIT);
            dequeueQ(proc);
            printf("Send process to scheduler at time %d\n", x);
        }
    }
    struct PData process;
    process.id = -1;
    x = getClk();
    msgsnd(msq_id, &process, sizeof(struct PData), !IPC_NOWAIT);
    printf("Finish sending at time %d\n", x);

    // addition
    wait(NULL); // for scheduler
    wait(NULL); // for clk
    // 7. Clear clock resources
    destroyClk(true);
    raise(SIGINT);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    // for now
    deleteQ(proc);
    msgctl(msq_id, IPC_RMID, (struct msqid_ds *)0);
    exit(0);
}

struct PQueue *readInputFile()
{
    FILE *input_file;
    struct PQueue *queue = createQ();
    input_file = fopen("processes.txt", "r");
    if (input_file == NULL)
    {
        perror("Can't open input file\n");
        exit(-1);
    }
    char s[50];
    fgets(s, 50, input_file);
    while (!feof(input_file))
    {
        struct PData data;
        fscanf(input_file, "%d\t%d\t%d\t%d\n", &data.id, &data.arrivaltime, &data.runningtime, &data.priority);
        data.remaintime = data.runningtime;
        data.waittime = 0;
        data.pid = -1;
        enqueueQ(data, queue);
    }
    fclose(input_file);
    return queue;
}

void SelAlgo()
{
    bool corrRead = false; // to verfiy reading values
    /*  1 -> HPF      2 -> SRTN     3 -> RR */
    printf("algorithms: 1->HPF, 2->SRTN, 3->RR\n");
    do
    {
        printf("Please choose algorithm: ");
        scanf("%d", &algo);
        if (algo == RR)
        {
            bool corrQ = false; // to verfiy quantum;
            do
            {
                printf("Please write quantum of RR algo ");
                scanf("%d", &quantum);
                if ((int)quantum >= 1)
                {
                    corrQ = true;
                }
                else
                {
                    printf("Error in quantum value. It should be greater than or equal 1.\n");
                }
            } while (!corrQ);
            corrRead = true;
        }
        else if (algo == HPF || algo == SRTN)
        {
            corrRead = true;
        }
        else
        {
            printf("Error in algo value not [1,2,3].\n");
        }
    } while (!corrRead);
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