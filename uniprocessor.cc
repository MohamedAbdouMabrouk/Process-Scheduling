#include "uniprocessor.h"

int main()
{
    char tORs[6];
    char type[50];
    int length;
    int pNumber;
    init(tORs, type, &length, &pNumber); // take the input

    Process *processes[pNumber];
    initProc(&pNumber, &length, processes); // build the process

    int task, max = 0;
    char *ptrT = strtok(type, ",");
    while (ptrT != NULL)
    {
        if (strlen(ptrT) > 1)
        {
            task = ptrT[0] - '0';
            max = ptrT[2] - '0';
        }
        else
            task = atoi(ptrT);

        switchTask(task, max, length, pNumber, processes); // send the task to the required scheduling.

        if (strcmp(tORs, "trace") == 0) // trace or stats
            printTrace(length, atoi(ptrT), max, pNumber, processes);
        else
            printStats(length, atoi(ptrT), max, pNumber, processes);
        ptrT = strtok(NULL, ",");

        clear(pNumber, length, processes); // clear the out array of the processes for the next task
    }
    freeEm(&pNumber, processes);
    return 0;
}

void init(char *tORs, char *type, int *length, int *pNumber)
{
    fscanf(stdin, "%s", tORs);
    fscanf(stdin, "%s", type);
    fscanf(stdin, "%d", length);
    fscanf(stdin, "%d", pNumber);
}

void initProc(int *pNumber, int *length, Process **processes)
{
    for (int i = 0; i < *pNumber; i++)
    {
        processes[i] = (Process *)malloc(sizeof(Process));
        fscanf(stdin, "\n%[^,],%d,%d", processes[i]->name, &processes[i]->start, &processes[i]->len);
        processes[i]->counter = processes[i]->len;
        processes[i]->index = i;
        processes[i]->out = (char *)malloc((*length) * sizeof(char));
    }
}

void freeEm(int *pNumber, Process **processes)
{
    for(int i = 0; i < *pNumber; i++)
    {
        free(processes[i]->out);
        free(processes[i]);
    }
}

void clear(int pNumber, int length, Process **processes)
{
    for (int i = 0; i < pNumber; i++)
    {
        for (int j = 0; j < length; j++)
        {
            processes[i]->out[j] = ' ';
        }
        processes[i]->counter = processes[i]->len;
    }
}

void switchTask(int task, int max, int length, int pNumber, Process **processes)
{
    // send the task for the required scheduling
    switch (task)
    {
    case 1:
        FCFS(length, pNumber, processes);
        break;
    case 2:
        RR(length, pNumber, processes, max);
        break;
    case 3:
        SPN(length, pNumber, processes);
        break;
    case 4:
        SRT(length, pNumber, processes);
        break;
    case 5:
        HRRN(length, pNumber, processes);
        break;
    case 6:
        FB_1(length, pNumber, processes);
        break;
    case 7:
        FB_2i(length, pNumber, processes);
        break;
    case 8:
        AGING(length, pNumber, processes, max);
        break;
    default:
        break;
    }
}

void FCFS(int length, int pNumber, Process **processes)
{
    queue<Process> q;                // make a queue
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) // see if any process want to get in the queue
            if (i == processes[j]->start)
                q.push(*processes[j]);

        int index = q.front().index;    // get the first element in the queue
        processes[index]->out[i] = '*'; // put a running flag to its output
        processes[index]->counter--;    // reduce its remainig time
        if (!processes[index]->counter) // finished ?
        {
            processes[index]->finish = i + 1; // save its finining time
            q.pop();
        }
    }
}

void RR(int length, int pNumber, Process **processes, int max)
{
    queue<Process> q; // make a queue
    int maxBU = max;
    int p = -1;
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) // see if any process want to get in the queue
            if (i == processes[j]->start)
                q.push(*processes[j]);

        if (p != -1)
        {
            q.push(*processes[p]); // push it back to the queue
            p = -1;
        }

        int index = q.front().index;    // get the first element in the queue
        processes[index]->out[i] = '*'; // put a running flag to its output
        processes[index]->counter--;    // reduce its remainig time
        max--;                          // decrease the remaining time for the process to run on the CPU
        if (!processes[index]->counter) // Finished?
        {
            processes[index]->finish = i + 1; // save its finining time
            q.pop();
            max = maxBU; // recover the max for the next process
        }
        else if (max == 0) // times up?
        {
            p = index; // save the process information
            q.pop();
            max = maxBU; // recover the max for the next process
        }
    }
}

void SPN(int length, int pNumber, Process **processes)
{
    priority_queue<Process, vector<Process>, CompareLen> q; // make a priority queue
    int index = -1;
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) /// see if any process want to get in the queue
            if (i == processes[j]->start)
                q.push(*processes[j]);

        if (index == -1)
        {
            index = q.top().index; // get the first process in the queue & put it in the CPU
            q.pop();               // remove it form the queue
        }
        processes[index]->out[i] = '*'; // put a running flag to its output
        processes[index]->counter--;    // reduce its remainig time
        if (!processes[index]->counter) // Finished?
        {
            processes[index]->finish = i + 1; // save its finining time
            index = -1;                       // flag to get the next process
        }
    }
}

void SRT(int length, int pNumber, Process **processes)
{
    priority_queue<Process, vector<Process>, CompareLen> q; // make a priority queue (with the lowest running time first)
    for (int i = 0; i < length; i++)                        // time line
    {
        for (int j = 0; j < pNumber; j++) /// see if any process want to get in the queue
            if (i == processes[j]->start)
                q.push(*processes[j]);

        int index = q.top().index;            // get the first process from the queue with the lowest remaingig time
        q.pop();                              // remove it from the queue
        processes[index]->out[i] = '*';       // put a running flag to its output
        processes[index]->counter--;          // reduce its remainig time
        if (!processes[index]->counter)       // finished?
            processes[index]->finish = i + 1; // save its finining time
        else
            q.push(*processes[index]); // push it back
    }
}

void HRRN(int length, int pNumber, Process **processes)
{
    priority_queue<Process, vector<Process>, CompareRatio> q; // make a priority queue (with the greatest ratio first)
    queue<Process> temp;                                      // temp queue to update the ratio
    int index = -1;
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < pNumber; j++) // see if any process want to get in the queue
            if (i == processes[j]->start)
                q.push(*processes[j]);

        if (index == -1)
        {
            // update the ratio for the processes those are in the queue
            while (!q.empty())
            {
                int tempx = q.top().index;
                processes[tempx]->ratio = ((i - processes[tempx]->start) + processes[tempx]->len) / (float)processes[tempx]->len; // calculate its ratio
                temp.push(*processes[tempx]);
                q.pop();
            }
            while (!temp.empty())
            {
                int tempx = temp.front().index;
                q.push(*processes[tempx]);
                temp.pop();
            }
            index = q.top().index; // get the firest process
            q.pop();               // remove it from the queue and put in the CPU
        }
        processes[index]->out[i] = '*'; // put a running flag to its output
        processes[index]->counter--;    // reduce its remainig time
        if (!processes[index]->counter) // finished?
        {
            processes[index]->finish = i + 1; // save its finining time
            index = -1;                       // flag to get new process
        }
    }
}

void FB_1(int length, int pNumber, Process **processes)
{
    queue<Process> RQ[pNumber + 1]; // make queues
    int max, index, PIS = 0;
    Process *p = NULL;
    bool idle = true;                // flag for the CPU
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) // see if any process want to get in the queue
        {
            if (processes[j]->start == i)
            {
                RQ[0].push(*processes[j]);
                processes[j]->level = 0;
                PIS++; // increament the number of the processess those are in the system
            }
        }

        if (PIS && p != NULL) // there are another processes waiting and there is process got time out?
        {
            processes[index]->level++;                           // increment its level
            RQ[processes[index]->level].push(*processes[index]); // push to the next level
            PIS++;                                               // increament the number of the processess those are in the system
            p = NULL;
        }
        else if (!PIS && p != NULL) // there are no other processes waiting and there is process got time out?
        {
            // keep the procces running
            idle = false;
            max = 1; // reset its time
            p = NULL;
        }

        if (idle) // there is no process running in the cpu?
            for (int j = 0; j < pNumber + 1; j++)
                // check the queues with the highst priority first
                if (!RQ[j].empty())
                {
                    index = RQ[j].front().index; // get the first process in this queue
                    RQ[j].pop();                 // remove it and put it in the CPU
                    PIS--;                       // decreament the number of the processess those are in the system
                    max = 1;                     // put the max time
                    idle = false;
                    break;
                }

        processes[index]->out[i] = '*'; // put a running flag to its output
        processes[index]->counter--;    // reduce its remainig time
        max--;                          // reduce its remaining time in the CPU
        if (!processes[index]->counter) // Finished?
        {
            processes[index]->finish = i + 1; // save its finining time
            idle = true;                      // cpu flag to get the next process
        }
        else if (max == 0) // times up?
        {
            p = processes[index]; // save process information
            idle = true;          // cpu flag to get the next process
        }
    }
}

void FB_2i(int length, int pNumber, Process **processes)
{

    queue<Process> RQ[pNumber + 1]; // make queues
    int max, index, PIS = 0;
    Process *p = NULL;
    bool idle = true;                // flag for the CPU
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) // see if any process want to get in the queue
        {
            if (processes[j]->start == i)
            {
                RQ[0].push(*processes[j]);
                processes[j]->level = 0;
                PIS++; // increament the number of the processess those are in the system
            }
        }

        if (PIS && p != NULL) // there are another processes waiting and there is process got time out?
        {
            processes[index]->level++;                           // increment its level
            RQ[processes[index]->level].push(*processes[index]); // push to the next level
            PIS++;                                               // increament the number of the processess those are in the system
            p = NULL;
        }
        else if (!PIS && p != NULL)
        {
            idle = false;
            max = pow(2, processes[index]->level); // calculate the max time accourding to its level
            p = NULL;
        }

        if (idle)
            for (int j = 0; j < pNumber + 1; j++)
                if (!RQ[j].empty())
                {
                    index = RQ[j].front().index;
                    RQ[j].pop();
                    PIS--;                                 // decreament the number of the processes in the system
                    max = pow(2, processes[index]->level); // calculate the max time accourding to its level
                    idle = false;
                    break;
                }

        processes[index]->out[i] = '*';
        processes[index]->counter--;
        max--;
        if (!processes[index]->counter)
        {

            processes[index]->finish = i + 1; // save its finining time
            idle = true;                      // cpu flag to get the next process
        }
        else if (max == 0)
        {
            p = processes[index]; // save process information
            idle = true;
        }
    }
}

void AGING(int length, int pNumber, Process **processes, int max)
{
    priority_queue<Process, vector<Process>, ComparePriority> q; // make a priority queue (with the highest priority first)
    queue<Process> temp;
    int index = -1, maxBU = max;
    bool idle = true;
    for (int i = 0; i < length; i++) // time line
    {
        for (int j = 0; j < pNumber; j++) /// see if any process want to get in the queue
            if (i == processes[j]->start)
            {
                q.push(*processes[j]);
                processes[j]->finish = length;
            }

        if (idle)
        {
            int j = pNumber; // to order the processes with the same priority
            while (!q.empty())
            {
                int tempx = q.top().index;
                processes[tempx]->counter++;   // increment the process priority thoes are in the queue
                processes[tempx]->level = j--; // to order the processes with the same priority
                temp.push(*processes[tempx]);
                q.pop();
            }
            while (!temp.empty())
            {
                int tempx = temp.front().index;
                q.push(*processes[tempx]);
                temp.pop();
            }

            if (index != -1)
                q.push(*processes[index]); // push the process back

            index = q.top().index; // get the process with the highest proiorty
            q.pop();               // remove it from the queue and put it in the cpu
            idle = false;
        }
        processes[index]->out[i] = '*'; //
        max--;                          // decrement the remaining tiem
        if (max == 0)                   // times up?
        {
            max = maxBU;                                       // reset the quantom
            processes[index]->counter = processes[index]->len; // reset the proiorty of the running queue
            processes[index]->level = -1;                      // in the end of the queue
            idle = true;                                       // flag for the cpu to get a new process
        }
    }
}

void printTrace(int length, int type, int max, int pNumber, Process **processes)
{
    char* name = translate(type);
    if (type == 2)
        printf("%s-%d  ", name, max);
    else if (type == 3 || type == 4)
        printf("%s   ", name);
    else if (type == 7 || type == 8)
        printf("%s ", name);
    else
        printf("%s  ", name);

    for (int i = 0; i < length + 1; i++)
    {
        printf("%d ", i % 10);
    }
    printf("\n------------------------------------------------\n");
    for (int i = 0; i < pNumber; i++)
    {
        printf("%s     ", processes[i]->name);
        for (int j = 0; j < length; j++)
        {
            if (processes[i]->start <= j && processes[i]->finish - 1 >= j)
                printf("|%c", processes[i]->out[j] != '*' ? '.' : processes[i]->out[j]);
            else
                printf("| ");
        }
        printf("| \n");
    }
    printf("------------------------------------------------\n\n");
    free(name);
}

void printStats(int length, int type, int max, int pNumber, Process **processes)
{   
    char *name = translate(type);
    type == 2 ? printf("%s-%d\n", name, max) : printf("%s\n", name);
    printf("Process    |");
    for (int i = 0; i < pNumber; i++)
    {
        printf("  %s  |", processes[i]->name);
    }
    printf("\n");
    printf("Arrival    |");
    for (int i = 0; i < pNumber; i++)
    {
        printf("%s", processes[i]->start > 9 ? " " : "  ");
        printf("%d  |", processes[i]->start);
    }
    printf("\n");
    printf("Service    |");
    for (int i = 0; i < pNumber; i++)
    {
        printf("%s", processes[i]->len > 9 ? " " : "  ");
        printf("%d  |", processes[i]->len);
    }
    printf(" Mean|\n");
    printf("Finish     |");
    for (int i = 0; i < pNumber; i++)
    {
        printf("%s", processes[i]->finish > 9 ? " " : "  ");
        printf("%d  |", processes[i]->finish);
    }
    printf("-----|\n");
    printf("Turnaround |");
    float mean = 0;
    for (int i = 0; i < pNumber; i++)
    {
        int turnaround = processes[i]->finish - processes[i]->start;
        mean += turnaround;
        printf("%s", turnaround > 9 ? " " : "  ");
        printf("%d  |", turnaround);
    }
    mean /= pNumber;
    mean > 9 ? printf("%0.2f|\n", mean) : printf(" %0.2f|\n", mean);
    printf("NormTurn   |");
    mean = 0;
    for (int i = 0; i < pNumber; i++)
    {
        float nT = (processes[i]->finish - processes[i]->start) / (float)processes[i]->len;
        mean += nT;
        printf(" %0.2f|", nT);
    }
    mean /= pNumber;
    mean > 9 ? printf("%0.2f|\n", mean) : printf(" %0.2f|\n", mean);
    printf("\n");
    free(name);
}

char *translate(int task)
{
    char *name = (char *)malloc(6 * sizeof(char));
    switch (task)
    {
    case 1:
        strcpy(name, "FCFS");
        break;
    case 2:
        strcpy(name, "RR");
        break;
    case 3:
        strcpy(name, "SPN");
        break;
    case 4:
        strcpy(name, "SRT");
        break;
    case 5:
        strcpy(name, "HRRN");
        break;
    case 6:
        strcpy(name, "FB-1");
        break;
    case 7:
        strcpy(name, "FB-2i");
        break;
    case 8:
        strcpy(name, "Aging");
        break;
    default:
        break;
    }
    return name;
}
