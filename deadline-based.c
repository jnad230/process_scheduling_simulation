/*  task3.c 
    Author: Javeriya Nadaf
    Start date: 9 Oct 2022
    Last modified date: 16 Oct 2022
    This program is a deadline based process scheudling algorithm inspired by the Least Slack time algorithm to create a simulation of the processes in order to maximise the total_processesber of processes that meet their deadlines. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct
{
    char process_name[11];  // A s t r i n g t h a t i d e n t i f i e s t h e p r o c e s s

    int processID;   // The tim e p r o c e s s e n t e r e d sy s t em
    int entryTime;   // The t o t a l CPU tim e r e q u i r e d by t h e p r o c e s s
    int serviceTime; // burst
    int deadline;
    int remainingTime;  // Remainin g s e r v i c e tim e u n t i l c om p l e t i o n .
    int completionTime;  // The completion time of the process
    int turnaround;
    int wait_time;
    bool not_in_gantt_chart;  // false if the process is in the gantt chart or currently in ready state
    bool not_completed;   // true if the process is not completed
    bool terminated;    // false if the process is in the queuue or in ready state
} pcb_t;

typedef struct least_slack_time
{
    int value;  //Least slack time value
    int index;  // Process index of process with least slack time
} least_slack_time;

void init_lst(least_slack_time *lst)
{/**
     This function initializes the least slack time to null. 
     Params: 
        - Queue to iniaitlize.
    **/
    lst->value = NULL;
    lst->index = NULL;
}

int service_check(pcb_t current_process, pcb_t next_process, int i);
int findIndex(pcb_t a[], size_t size, int value);

int main(int argc, char *argv[])
{/**
     This function reads the processes data and implements the least slack time algorithm to schduele the processes within their deadlines. The function then saves the wait time, turnaround and deadline information to a file called results-task2.txt.
     Params:
        - Name of file to read processes data from.
    **/

    FILE *fptr;

    int i, total_processes, j, lowest_service, entry_index;
    int time, processes_completed, TQ;
    float avg = 0.0, sum = 0.0;
    pcb_t p[100], temp;
    char ready_queue[100][11];
    char gantt_chart[100][11];
    int gantt_index;

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
        p[i].not_completed = 1;
        p[i].not_in_gantt_chart = 1;
        p[i].terminated = 0;
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

    // Iterate through and organize the processes in the ascending order of their arrival times
    total_processes = i;

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
    least_slack_time lst;
    init_lst(&lst);
    time = 0;
    processes_completed = 0;
    int slack_time;
    gantt_index = 0;
    TQ = 1;
    while (processes_completed < total_processes)
    {
        i = 0;
        // Iterates over each process to evaluate their slack time and adds the process with the least slack time to the gantt chart, executing it.
        while (p[i].entryTime <= time && i < total_processes)
        {
            if (p[i].not_completed == 1 && p[i].terminated == 0)
            {

                if (p[i].entryTime == time)
                {
                    sleep(1);
                    printf("Time %d: %s has entered the system.\n", p[i].entryTime, p[i].process_name);
                }

                slack_time = p[i].deadline - time - p[i].remainingTime;

                if (slack_time <= 0)
                {
                    // If the slack time is less than or equal to zero, the process has failed to meet its deadline under the current conditions and gets terminated.

                    p[i].terminated = 1;
                    sleep(1);
                    printf("Time %d: %s has timed out.\n", time, p[i].process_name);
                    processes_completed += 1;
                }
                
                else if (lst.value == NULL || lst.value > slack_time)
                {
                    // Update least slack time process 
                    lst.value = slack_time;
                    lst.index = i;
                }
                else if (lst.value == slack_time)
                {

                    // If the slack times are same and neither of them are currently running then precedence is given to the one with lower remaining time.
                    if (p[lst.index].remainingTime > p[i].remainingTime)
                    {
                        lst.index = i;
                    }
                    else if (p[lst.index].not_completed == 0)
                    {
                        lst.index = i;
                    }
                }
            }
            i++;
        }

        // Update the gantt chart with the current process's id
        strcpy(gantt_chart[gantt_index], p[lst.index].process_name);
        gantt_index += 1;
        if (p[lst.index].not_in_gantt_chart == 1)
        {
            p[lst.index].not_in_gantt_chart = 0;
            // Update the wait time
            p[lst.index].wait_time = time - p[lst.index].entryTime;
        }
        sleep(1);
        printf("Time %d: %s is in the running state.\n", time, p[lst.index].process_name);

        // Update the remaining time
        p[lst.index].remainingTime = p[lst.index].remainingTime - TQ;

        if (p[lst.index].remainingTime == 0)
        {
            p[lst.index].not_completed = 0;
            time = time + TQ;
            p[lst.index].completionTime = time;
            sleep(1);
            printf("Time %d: %s has finished execution.\n", time, p[lst.index].process_name);
            processes_completed++;
        }
        else
        {
            time = time + TQ;
        }
    }

    // Calculate the turnaround time for each process.
    for (i = 0; i < total_processes; i++)
    {
        p[i].turnaround = p[i].completionTime - p[i].entryTime;
    }

    FILE *fptr2;

    // opening file in writing mode
    fptr2 = fopen("results-task3.txt", "w");
    // exiting program
    if (fptr2 == NULL)
    {
        printf("Error!");
        exit(1);
    }
    
    // Determine if the deadline is met and write the process scheduling information to the results file.
    for (i = 0; i < total_processes; i++)
    {
        if (p[i].terminated == 1)
        {   // Turnaround time is zero if process timed out.
            p[i].wait_time = p[i].deadline;
            p[i].deadline = 0;
            p[i].turnaround = 0;
        }
        else
        {
            p[i].deadline = 1;
        }

        fprintf(fptr2, "%s\t%d\t%d\t%d\n", p[i].process_name, p[i].wait_time, p[i].turnaround, p[i].deadline);
        sum += p[i].turnaround;
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