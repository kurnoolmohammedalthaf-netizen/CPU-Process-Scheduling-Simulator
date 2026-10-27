/*
 * Project: CPU Process Scheduling Simulator - Queue
 * Course: CSS2281 - C Programming and Data Structures Lab
 * Algorithm: First Come, First Served (FCFS)
 * Data structure: FIFO Queue
 *
 * Compile: gcc cpu_scheduling.c -o cpu_scheduling
 * Run:     ./cpu_scheduling       (Linux/macOS)
 *          .\cpu_scheduling.exe  (Windows PowerShell)
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int waiting;
    int turnaround;
} Process;

typedef struct {
    Process items[MAX_PROCESSES];
    int front;
    int rear;
    int count;
} Queue;

void initializeQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isEmpty(const Queue *q) {
    return q->count == 0;
}

int isFull(const Queue *q) {
    return q->count == MAX_PROCESSES;
}

int enqueue(Queue *q, Process p) {
    if (isFull(q)) {
        return 0;
    }
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->items[q->rear] = p;
    q->count++;
    return 1;
}

int dequeue(Queue *q, Process *p) {
    if (isEmpty(q)) {
        return 0;
    }
    *p = q->items[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    return 1;
}

void sortByArrival(Process processes[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void addProcesses(Process processes[], int *n, int *scheduled) {
    int amount, i;

    if (*scheduled) {
        printf("\nScheduling has already been performed. Restart the program to add more processes.\n");
        return;
    }

    if (*n >= MAX_PROCESSES) {
        printf("\nThe process list is full (maximum %d processes).\n", MAX_PROCESSES);
        return;
    }

    printf("\nHow many processes do you want to add? ");
    if (scanf("%d", &amount) != 1) {
        printf("Invalid input.\n");
        exit(EXIT_FAILURE);
    }

    if (amount <= 0 || amount > MAX_PROCESSES - *n) {
        printf("Enter a number from 1 to %d.\n", MAX_PROCESSES - *n);
        return;
    }

    for (i = 0; i < amount; i++) {
        Process p = {0};
        p.pid = *n + 1;

        printf("\nProcess P%d\n", p.pid);
        printf("Arrival time (0 or greater): ");
        if (scanf("%d", &p.arrival) != 1) {
            printf("Invalid input.\n");
            exit(EXIT_FAILURE);
        }
        printf("Burst time (greater than 0): ");
        if (scanf("%d", &p.burst) != 1) {
            printf("Invalid input.\n");
            exit(EXIT_FAILURE);
        }

        if (p.arrival < 0 || p.burst <= 0) {
            printf("Invalid values. Arrival time must be >= 0 and burst time must be > 0. Try again.\n");
            i--;
            continue;
        }

        processes[*n] = p;
        (*n)++;
    }

    printf("\nProcess details added successfully.\n");
}

void displayProcesses(const Process processes[], int n, int scheduled) {
    int i;

    if (n == 0) {
        printf("\nNo processes have been added yet.\n");
        return;
    }

    printf("\n%-10s %-15s %-12s\n", "Process", "Arrival Time", "Burst Time");
    printf("------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("P%-9d %-15d %-12d\n",
               processes[i].pid, processes[i].arrival, processes[i].burst);
    }

    if (!scheduled) {
        printf("\nThese processes will be queued by arrival time when scheduling starts.\n");
    }
}

void runScheduling(Process processes[], int n, int *scheduled) {
    Queue readyQueue;
    Process current;
    int i, clock = 0;
    double totalWaiting = 0.0, totalTurnaround = 0.0;

    if (n == 0) {
        printf("\nPlease add processes before scheduling.\n");
        return;
    }
    if (*scheduled) {
        printf("\nScheduling has already been performed.\n");
        return;
    }

    sortByArrival(processes, n);
    initializeQueue(&readyQueue);

    /*
     * FCFS: processes are inserted in ascending arrival-time order.
     * If arrival times tie, their original input order is preserved.
     */
    for (i = 0; i < n; i++) {
        if (!enqueue(&readyQueue, processes[i])) {
            printf("Queue is full; could not enqueue a process.\n");
            return;
        }
    }

    printf("\n========== FCFS CPU SCHEDULING ==========\n");
    printf("Execution order: ");

    i = 0;
    while (dequeue(&readyQueue, &current)) {
        if (clock < current.arrival) {
            clock = current.arrival; /* CPU is idle until this process arrives. */
        }

        current.start = clock;
        current.completion = current.start + current.burst;
        current.waiting = current.start - current.arrival;
        current.turnaround = current.completion - current.arrival;
        clock = current.completion;

        /* Store the calculated values back in the sorted process list. */
        processes[i] = current;
        i++;

        printf("P%d", current.pid);
        if (i < n) {
            printf(" -> ");
        }

        totalWaiting += current.waiting;
        totalTurnaround += current.turnaround;
    }

    printf("\n\n%-8s %-8s %-8s %-8s %-10s %-12s %-12s\n",
           "Process", "Arrival", "Burst", "Start", "Completion", "Waiting", "Turnaround");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("P%-7d %-8d %-8d %-8d %-10d %-12d %-12d\n",
               processes[i].pid,
               processes[i].arrival,
               processes[i].burst,
               processes[i].start,
               processes[i].completion,
               processes[i].waiting,
               processes[i].turnaround);
    }

    printf("\nAverage waiting time    : %.2f time units\n", totalWaiting / n);
    printf("Average turnaround time : %.2f time units\n", totalTurnaround / n);
    printf("=========================================\n");

    *scheduled = 1;
}

int main(void) {
    Process processes[MAX_PROCESSES];
    int n = 0, choice;
    int scheduled = 0;

    printf("============================================\n");
    printf(" CPU PROCESS SCHEDULING SIMULATOR - QUEUE\n");
    printf(" FCFS (First Come, First Served)\n");
    printf("============================================\n");

    while (1) {
        printf("\n--------------- MAIN MENU ---------------\n");
        printf("1. Add processes\n");
        printf("2. Display processes\n");
        printf("3. Run FCFS scheduling\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            return EXIT_FAILURE;
        }

        switch (choice) {
            case 1:
                addProcesses(processes, &n, &scheduled);
                break;
            case 2:
                displayProcesses(processes, n, scheduled);
                break;
            case 3:
                runScheduling(processes, n, &scheduled);
                break;
            case 4:
                printf("\nThank you for using the CPU Process Scheduling Simulator.\n");
                return 0;
            default:
                printf("\nInvalid choice. Please select 1, 2, 3, or 4.\n");
        }
    }
}
