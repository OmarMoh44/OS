#include "headers.h"

struct processData
{
    int id; // id of process in system
    int arrivaltime;
    int runningtime;
    int priority;
    int remaintime;
    int waittime;
    pid_t pid; // id of forken process (in real system) to run process.out program
};

void clearResources(int);
struct processData *readInputFile(int *count); // reading data of processes and store in a buffer
void SelAlgo(int *algo, int *q);               // read input from user to select algo and parameter if exist
int CountDigit(int x);                         // count number of digits of integer to convert it to string

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    int process_nums = 0;
    struct processData *proc = readInputFile(&process_nums);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    /*  1 -> HPF      2 -> SRTN     3 -> RR */
    int algo = -1;
    int quantum = -1; // for only RR algo
    SelAlgo(&algo, &quantum);

    // 3. Initiate and create the scheduler and clock processes.
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
    for (int i = 0; i < process_nums; i++)
    {
        char st[CountDigit(proc[i].runningtime) + 2];
        sprintf(st, "%d", proc[i].runningtime); // convert runningtime to string to pass it for process program
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
            proc[i].pid = x;
        }
    }

    // 4. Use this function after creating the clock process to initialize clock
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
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);

    // addition
    wait(NULL);                            // for scheduler
    for (int i = 0; i < process_nums; i++) // for processes
    {
        wait(NULL);
    }
    wait(NULL); // for clk
    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption

    // for now
    exit(0);
}

struct processData *readInputFile(int *count)
{
    FILE *input_file;
    input_file = fopen("processes.txt", "r");
    if (input_file == NULL)
    {
        perror("Can't open input file\n");
        exit(-1);
    }
    char c;        // for reading character from input file
    int lines = 0; // count number of processes in txt file
    while ((c = fgetc(input_file)) != EOF)
    {
        if (c == '\n')
            lines++;
    }
    fclose(input_file);
    input_file = fopen("processes.txt", "r"); // for reading process data
    lines--;
    struct processData *proc = (struct processData *)malloc(sizeof(struct processData) * lines);
    int index = 0;
    int i = 0;
    bool ignore = false;
    int data = 0;
    while ((c = fgetc(input_file)) != EOF)
    {
        if (c == '#')
        {
            ignore = true;
        }
        if (c == '\n')
        {
            if (!ignore)
            {
                proc[index].priority = data;
                index++;
            }
            ignore = false;
            i = 0;
            data = 0;
            continue;
        }
        if (!ignore)
        {
            if (c == '\t')
            {
                switch (i)
                {
                case 0:
                    proc[index].id = data;
                    proc[index].waittime = 0;
                    break;
                case 1:
                    proc[index].arrivaltime = data;
                    break;
                case 2:
                    proc[index].runningtime = data;
                    proc[index].remaintime = data;
                    break;
                default:
                    break;
                }
                i++;
                data = 0;
            }
            else
            {
                data = 10 * data + c - '0';
            }
        }
    }
    *count = lines;
    fclose(input_file);
    return proc;
}

void SelAlgo(int *algo, int *q)
{
    bool corrRead = false; // to verfiy reading values
    /*  1 -> HPF      2 -> SRTN     3 -> RR */
    printf("algorithms: 1->HPF, 2->SRTN, 3->RR\n");
    do
    {
        printf("Please choose algorithm: ");
        scanf("%d", algo);
        if ((int)*algo == 3)
        {
            bool corrQ = false; // to verfiy quantum;
            do
            {
                printf("Please write quantum of RR algo ");
                scanf("%d", q);
                if ((int)*q >= 1)
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
        else if ((int)*algo == 1 || (int)*algo == 2)
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
