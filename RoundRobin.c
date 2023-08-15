/*  task2.c 
    Author: Javeriya Nadaf
    Start date: 9 Oct 2022
    Last modified date: 16 Oct 2022
    This program makes use of the Round Robin process scheudling algorithm to create a simulation of the processes. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct
{
    char process_name[11];

    int processID;   // A s t r i n g t h a t i d e n t i f i e s t h e p r o c e s s
    int entryTime;   // The tim e p r o c e s s e n t e r e d sy s t em
    int serviceTime; // The t o t a l CPU tim e r e q u i r e d by t h e p r o c e s s
    int remainingTime;  // Remainin g s e r v i c e tim e u n t i l c om p l e t i o n .
    int completionTime;  // The completion time of the process
    int turnaround;
    int deadline;
    int wait_time;
    bool not_in_gantt_chart;  // false if the process is in the gantt chart or currently in ready state
    bool not_completed;  // true if the process is not completed
    bool not_in_queue; // false if the process is in the queuue or in ready state
} pcb_t;

typedef struct node
{
    pcb_t process;      
    struct node *next;
} node;

typedef struct
{
    node *head;
    node *tail;
} queue;

void init_queue(queue *q)
{/**
     This function initializes the queue to null. 
     Params: 
        - Queue to iniaitlize.
    **/
    q->head = NULL;
    q->tail = NULL;
}

bool enqueue(queue *q, pcb_t value)
{/**
     This function adds an element to the queue. 
     Params: 
        - pcb_t element to be added to the queue.
    **/

    // create a new node
    node *newnode = malloc(sizeof(node));
    if (newnode == NULL)
    {
        return false;
    }

    newnode->process = value;
    newnode->next = NULL;

    // if there is a tail, conenct that tail to this new node
    if (q->tail != NULL)
    {
        q->tail->next = newnode;
    }
    q->tail = newnode;

    // make sure the head makes sense.
    if (q->head == NULL)
    {
        q->head = newnode;
    }
    return true;
}

node dequeue(queue *q)
{/**
     This function removes an element to the queue. 
     Params: 
        - pcb_t element to be removes from the queue.
    **/

    // Save the pcbt in the result
    node *result = q->head;

    q->head = q->head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    
    return *result;
}

int service_check(pcb_t current_process, pcb_t next_process, int i);
int findIndex(pcb_t a[], size_t size, int value);

int main(int argc, char *argv[])
{/**
     This function reads the processes data and implements the round robin algorithm to schduele the processes. The function then saves the wait time, turnaround and deadline information to a file called results-task2.txt.
     Params:
        - Name of file to read processes data from.
    **/

    FILE *fptr;

    int i, total_processes, j, lowest_service, entry_index;
    int TQ = 2;
    pcb_t p[100], temp;
    char ready_queue[100][11];
    char gantt_chart[100][11];

    char buf[100];

    if (argc == 1)
    { // If only 1 argument given use default settings

        fptr = fopen("processes.txt", "r");
    }
    else if (argc == 2)
    { // If 2 arguments given then read from the user specified file

        fptr = fopen(argv[1], "r");
    }

    if (fptr == NULL)
    {
        perror("fopen");
        return 0;
    }

    i = 0;
    // Copying the process names
    while (fscanf(fptr, "%s %*s %*s %*s", buf) == 1)
    {
        strcpy(p[i].process_name, buf);
        p[i].processID = i;
        p[i].not_in_gantt_chart = 1;
        p[i].not_completed = 1;
        p[i].not_in_queue = 1;
        i++;
    }

    fseek(fptr, 0, SEEK_SET);
    i = 0;
    // Copying the entry time
    while (fscanf(fptr, "%*s %s %*s %*s", buf) == 1)
    {
        p[i].entryTime = atoi(buf);
        i++;
    }

    fseek(fptr, 0, SEEK_SET);
    i = 0;
    // Copying the service time
    while (fscanf(fptr, "%*s %*s %s %*s", buf) == 1)
    {
        p[i].serviceTime = atoi(buf);
        p[i].remainingTime = atoi(buf);
        i++;
    }

    fseek(fptr, 0, SEEK_SET);
    i = 0;
    // Copying the deadline
    while (fscanf(fptr, "%*s %*s %*s %s", buf) == 1)
    {
        p[i].deadline = atoi(buf);
        i++;
    }

    fclose(fptr);

    total_processes = i;

    // Iterate through and organize the processes in the ascending order of their arrival times
    for (i = 0; i < total_processes - 1; i++)
    {
        for (j = 0; j < total_processes - 1; j++)
        {
            if (p[j].entryTime > p[j + 1].entryTime)
            {
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }


    // Algorithm
    int processes_completed = 0;
    int gantt_chart_index = 0;
    int time = 0;
    queue q1;
    init_queue(&q1);
    pcb_t tmp;
    node current_process;
    int process_counter = 0;
    int lower_bound_time = 0;

    while (processes_completed < total_processes)
    {
        i = 0;
        // Iterates over each process and adds any process that is ready (has entry time less than current time) to the ready queue.
        while (p[i].entryTime <= time && i < total_processes)
        {
            if (p[i].not_completed == 1 && p[i].not_in_queue == 1)
            {
                // Print enter message for processes that may have arrived between the execution and completion of the current process.
                while (p[process_counter].entryTime <= time && p[process_counter].entryTime >= lower_bound_time && process_counter < total_processes)
                {
                    sleep(2);
                    printf("Time %d: %s has entered the system.\n", p[process_counter].entryTime, p[process_counter].process_name);
                    process_counter += 1;
                }
                lower_bound_time = time;

                if (strcmp(p[i].process_name, gantt_chart[gantt_chart_index - 1]) == 0)
                {
                    // If the process was recently running then add it to temp to add it to the back of the queue.
                    tmp = p[i];
                    p[i].not_in_queue = 0;
                }
                else
                {
                    enqueue(&q1, p[i]);
                    p[i].not_in_queue = 0;
                    i++;
                }
            }
            else
            {
                i++;
            }
        }

        // Enqueue the process previously in the gantt chart to the end of the ready queue
        if (strcmp(tmp.process_name, gantt_chart[gantt_chart_index - 1]) == 0 && time != 0)
        {
            enqueue(&q1, tmp);
        }

        // Get the next process from the ready queue
        current_process = dequeue(&q1);
        int current_process_id = findIndex(p, sizeof(p), current_process.process.processID);
        p[current_process_id].not_in_queue = 1;

        // Adding the current process to the gantt chart
        strcpy(gantt_chart[gantt_chart_index], current_process.process.process_name);
        gantt_chart_index++;
        if (p[current_process_id].not_in_gantt_chart == 1)
        {
            p[current_process_id].not_in_gantt_chart = 0;
            // Update the wait time
            p[current_process_id].wait_time = time - p[current_process_id].entryTime;
        }

        // Update the remaining time if its less than time quantum; else print exit message
        if (p[current_process_id].remainingTime < TQ)
        {
            time = p[current_process_id].remainingTime + time;
            p[current_process_id].completionTime = time;
            p[current_process_id].not_completed = 0;
            sleep(2);
            printf("Time %d: %s has finished execution.\n", time, p[current_process_id].process_name);
            processes_completed++;
        }
        else
        {
            p[current_process_id].remainingTime = p[current_process_id].remainingTime - TQ;
            sleep(2);
            printf("Time %d: %s is in the running state.\n", time, p[current_process_id].process_name);
            time = time + TQ;
            if (p[current_process_id].remainingTime == 0)
            {
                p[current_process_id].completionTime = time;
                p[current_process_id].not_completed = 0;
                sleep(2);
                printf("Time %d: %s has finished execution.\n", time, p[current_process_id].process_name);
                processes_completed++;
            }
        }
    }

    // Calculate the turnaround time for each process.
    for (i = 0; i < total_processes; i++)
    {
        p[i].turnaround = p[i].completionTime - p[i].entryTime;
    }

    FILE *fptr2;

    // opening file in writing mode
    fptr2 = fopen("results-task2.txt", "w");
    // exiting program
    if (fptr2 == NULL)
    {
        printf("Error!");
        exit(1);
    }

    // Determine if the deadline is met and write the process scheduling information to the results file.
    for (i = 0; i < total_processes; i++)
    {
        if (p[i].turnaround <= p[i].deadline)
        {
            p[i].deadline = 1;
        }
        else
        {
            p[i].deadline = 0;
        }

        fprintf(fptr2, "%s\t%d\t%d\t%d\n", p[i].process_name, p[i].wait_time, p[i].turnaround, p[i].deadline);
        
    }
    fclose(fptr2);
}

int service_check(pcb_t current_process, pcb_t next_process, int i)
{/**
   This function is used to determine the process with the shorter service time.
   Paramaters:
      - current_process: Process struct of the current process running.
      - next_process: Process struct of the next process with the same arrival time.
      - i: index of the current process in the processes array.
   */
    if (current_process.serviceTime < next_process.serviceTime)
    {
        return i;
    }
    else
    {
        return i + 1;
    }
}

int findIndex(pcb_t *array, size_t size, int target)
{/**
     This function finds the index of the target element in an array of process blocks. 
     Params: 
        - array: array of proccess blocks.
        - size: size of the array to iterate over.
        - target: the element whose index in the array is to be found.
    **/
    int i = 0;
    while ((i < size) && (array[i].processID != target))
        i++;

    return (i < size) ? (i) : (-1);
}