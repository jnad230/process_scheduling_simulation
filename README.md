# Process Scheduling Simulation
Developed scheduling programs for FCFS, RR (Round Robin), and deadline-based algorithms. Ensured processes ran to completion even if deadlines were missed. User documentation provided clear usage instructions, algorithm explanations, and rationale for maximizing deadline meet-ups.

A process schuedling algorithm simulation system in C.

The tasks within the program use various process scheduling algorithm to create a simulation of how processeses would be executed using the given algorithms.
User Commands

Task 1
Usage:
    Use the command line prompt to run the following:

        gcc task1-32095376.c -o task1out 
        ./task1out
        ./task1out [FILENAME]

        - If no file name is specified then a default processes file is used.

    The FIFO algorithm is assumed to run a process uninterrupted upon its arrival. 
    If two processes have the same arrival time then the one with the lower service time gets the precedence.

Task 2
Usage: 
    Use the command line prompt to run the following:

        gcc task2-32095376.c -o task1out 
        ./task1out
        ./task1out [FILENAME]

        - If no file name is specified then a default processes file is used.

    The Round robin algorithm is assumed to have a time quantum of 2. 
    If the process that is currently being executed is ready, it goes at the back of the ready queue.

Task 3
Usage: 
    Use the command line prompt to run the following:

        gcc task2-32095376.c -o task1out 
        ./task1out
        ./task1out [FILENAME]

        - If no file name is specified then a default processes file is used.

        If two processes have the same slack time then the one that is currently being executed gets precedence. If neither of them are running then the one with shorter remaining time gets precedence.

        Least slack time
        How does it work: It assigns priority to the processes based on their slack time which is derived from their deadlines. Least slack time gets highest priority as it indicates the time required to complete the process execution.
         Slack time = current time - deadline - remaining time. 

        Why: Since the least slack time indicates the time required to complete the process execution, prioritizing the same leads to maximised process completions within the given deadline. 




    


    
    
            
