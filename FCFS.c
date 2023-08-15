/*  task1.c
    Author: Javeriya Nadaf
    Start date: 9 Oct 2022
    Last modified date: 16 Oct 2022
    This program makes use of the first in first out process scheudling algorithm to create a simulation of the processes. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    char process_name[11]; // A s t r i n g t h a t i d e n t i f i e s t h e p r o c e s s

    int entryTime;      // The tim e p r o c e s s e n t e r e d sy s t em
    int serviceTime;    // The t o t a l CPU tim e r e q u i r e d by t h e p r o c e s s
    int remainingTime;  // Remainin g s e r v i c e tim e u n t i l c om p l e t i o n .
    int completionTime; // The completion time of the process
    int turnaround;
    int wait_time;
    int deadline;

} pcb_t;

void pline(int x);
int service_check(pcb_t current_process, pcb_t next_process, int i);

int main(int argc, char *argv[])
{
    /**
     This function reads the processes data and implements the first in first out algorithm to schduele the processes. The function then saves the wait time, turnaround and deadline information to a file called results-task1.txt.
     Params:
        - Name of file to read processes data from.
    **/

    FILE *fptr;

    int i, total_processes, j, lowest_service, entry_index;
    float avg = 0.0, sum = 0.0;
    pcb_t p[100], temp;
    char ready_queue[100][12];

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
        perror("Failed to open the file\n");
        return 0;
    }

    fseek(fptr, 0, SEEK_SET); // Set the file pointer to the start of the file.
    i = 0;
    // Copying the process names
    while (fscanf(fptr, "%s %*s %*s %*s", buf) == 1)
    {
        strcpy(p[i].process_name, buf);
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

    total_processes = i; // Total total_processesber of processes

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
    int current_process = 0;
    int time = 0;
    int process_counter = 1;
    int lower_bound_time = 0;

    // The processes are aranged in the ascending order of their arrival hence iterating through them results in a first in first out execution.
    while (current_process < total_processes)
    {
        if (p[current_process].entryTime <= time)
        {

            if (p[current_process].entryTime == time)
            {
                sleep(2);
                printf("Time %d: %s has entered the system.\n", p[current_process].entryTime, p[current_process].process_name);
            }

            // If two processes have the same entry time then execute the one with the lower service time
            if (p[current_process].entryTime == p[current_process + 1].entryTime)
            {
                int current = current_process;
                // Check if the upcoming processes have the same time then execute the one with the lower service time
                while (p[current].entryTime == p[current + 1].entryTime)
                {
                    lowest_service = service_check(p[current_process], p[current_process + 1], current_process);
                    current++;
                }
                // Update the wait time
                p[current_process].wait_time = time - p[current_process].entryTime;

                // Execute the process directly by incrementing the completion time
                sleep(2);
                printf("Time %d: %s is in the running state.\n", time, p[current_process].process_name);

                p[lowest_service].completionTime = time + p[lowest_service].serviceTime;

                int process_counter = current_process;

                // Print enter message for processes that may have arrived between the execution and completion of the current process.
                while (p[process_counter].entryTime <= time && p[process_counter].entryTime >= lower_bound_time && process_counter < total_processes)
                {
                    sleep(2);
                    printf("Time %d: %s has entered the system.\n", p[process_counter].entryTime, p[process_counter].process_name);
                    process_counter += 1;
                }
                lower_bound_time = time;

                sleep(2);
                printf("Time %d: %s has finished execution.\n", time, p[current_process].process_name);

                time = p[lowest_service].completionTime;
                current_process++;
            }
            else
            {
                // Update the wait time
                p[current_process].wait_time = time - p[current_process].entryTime;

                // Execute the process directly by incrementing the completion time
                sleep(2);
                printf("Time %d: %s is in the running state.\n", time, p[current_process].process_name);

                p[current_process].completionTime = time + p[current_process].serviceTime;
                time = p[current_process].completionTime;

                // Print enter message for processes that may have arrived between the execution and completion of the current process.
                while (p[process_counter].entryTime <= time && p[process_counter].entryTime >= lower_bound_time && process_counter < total_processes)
                {
                    sleep(2);
                    printf("Time %d: %s has entered the system.\n", p[process_counter].entryTime, p[process_counter].process_name);
                    process_counter += 1;
                }
                lower_bound_time = time;

                sleep(2);
                printf("Time %d: %s has finished execution.\n", time, p[current_process].process_name);
                current_process++;
            }
        }
        else
        {
            time++;
        }
    }

    // Calculate the turnaround time for each process.
    for (i = 0; i < total_processes; i++)
    {
        p[i].turnaround = p[i].completionTime - p[i].entryTime;
    }
    
    FILE *fptr2;

    // opening file in writing mode
    fptr2 = fopen("results-task1.txt", "w");
    // exiting program
    if (fptr2 == NULL)
    {
        printf("Error!");
        exit(1);
    }
    i = 0;
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
{ /**
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
