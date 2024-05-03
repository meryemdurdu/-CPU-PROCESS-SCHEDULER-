#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold process information
typedef struct {
    char name[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram;
    int cpu;
} Process;

// Structure to hold queue information
typedef struct {
    Process *array;
    int front, rear, size;
    unsigned capacity;
} Queue;

// Function prototypes
void initialize(Queue *q, unsigned capacity);
int isFull(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, Process process);
Process dequeue(Queue *q);
void printQueue(Queue *q, FILE *file);


int main() {
    FILE *input_file, *output_file;
    char input_filename[] = "input.txt";
    char output_filename[] = "output.txt";

    input_file = fopen(input_filename, "r");
    output_file = fopen(output_filename, "w");

    if (input_file == NULL || output_file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Define CPUs and queues
    Queue cpu1; // FCFS for priority 0
    Queue cpu2_1; // SJF for priority 1
    Queue cpu2_2; // RR with quantum 8 for priority 2
    Queue cpu2_3; // RR with quantum 16 for priority 3
    initialize(&cpu1, 100);
    initialize(&cpu2_1, 100);
    initialize(&cpu2_2, 100);
    initialize(&cpu2_3, 100);

    // Read and process input file
    Process processes[100];
    int num_processes = 0;

    while (fscanf(input_file, "%[^,],%d,%d,%d,%d,%d\n",
                  processes[num_processes].name,
                  &processes[num_processes].arrival_time,
                  &processes[num_processes].priority,
                  &processes[num_processes].burst_time,
                  &processes[num_processes].ram,
                  &processes[num_processes].cpu) != EOF) {
        num_processes++;
    }

    fclose(input_file);

    // Process scheduler
    int time = 0;
    int completed_processes = 0;
    int cpu2_2_quantum = 8;
    int cpu2_3_quantum = 16;

    while (completed_processes < num_processes) {
        
        // Check for processes arriving at current time and enqueue them
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == time) {
                if (processes[i].priority == 0) {
                    enqueue(&cpu1, processes[i]);
                } else {
                    if (processes[i].priority == 1) {
                        enqueue(&cpu2_1, processes[i]);
                    } else if (processes[i].priority == 2) {
                        enqueue(&cpu2_2, processes[i]);
                    } else if (processes[i].priority == 3) {
                        enqueue(&cpu2_3, processes[i]);
                    }
                }
            }
        }

        // CPU-1 processing (FCFS)
        if (!isEmpty(&cpu1)) {
            Process current_process = dequeue(&cpu1);
            fprintf(output_file, "Process %s is assigned to CPU-1.\n", current_process.name);
            current_process.burst_time--;

            if (current_process.burst_time == 0) {
                fprintf(output_file, "Process %s is completed and terminated.\n", current_process.name);
                completed_processes++;
            } else {
                enqueue(&cpu1, current_process);
            }
        }

        // CPU-2 processing (Priority based)
        if (!isEmpty(&cpu2_1)) {
            Process current_process = dequeue(&cpu2_1);
            fprintf(output_file, "Process %s is assigned to CPU-2.\n", current_process.name);
            current_process.burst_time--;

            if (current_process.burst_time == 0) {
                fprintf(output_file, "Process %s is completed and terminated.\n", current_process.name);
                completed_processes++;
            } else {
                enqueue(&cpu2_1, current_process);
            }
        } else if (!isEmpty(&cpu2_2)) {
            Process current_process = dequeue(&cpu2_2);
            fprintf(output_file, "Process %s is assigned to CPU-2.\n", current_process.name);
            current_process.burst_time--;

            if (current_process.burst_time == 0) {
                fprintf(output_file, "Process %s is completed and terminated.\n", current_process.name);
                completed_processes++;
            } else {
                enqueue(&cpu2_2, current_process);
            }
        } else if (!isEmpty(&cpu2_3)) {
            Process current_process = dequeue(&cpu2_3);
            fprintf(output_file, "Process %s is assigned to CPU-2.\n", current_process.name);
            current_process.burst_time--;

            if (current_process.burst_time == 0) {
                fprintf(output_file, "Process %s is completed and terminated.\n", current_process.name);
                completed_processes++;
            } else {
                enqueue(&cpu2_3, current_process);
            }
        }

        // Print CPU queue information to console
        if (time % 10 == 0) {
            printf("Time: %d\n", time);
            printf("CPU-1 que1(priority-0) (FCFS) -> ");
            printQueue(&cpu1, stdout);
            printf("CPU-2 que2(priority-1) (SJF) -> ");
            printQueue(&cpu2_1, stdout);
            printf("CPU-2 que3(priority-2) (RR-q8) -> ");
            printQueue(&cpu2_2, stdout);
            printf("CPU-2 que4(priority-3) (RR-q16) -> ");
            printQueue(&cpu2_3, stdout);
            printf("\n");
        }
        time++;
    }

    // Print queued processes for each CPU to output file
    fprintf(output_file, "CPU-1 que1(priority-0) (FCFS) -> ");
    printQueue(&cpu1, output_file);
    fprintf(output_file, "CPU-2 que2(priority-1) (SJF) -> ");
    printQueue(&cpu2_1, output_file);
    fprintf(output_file, "CPU-2 que3(priority-2) (RR-q8) -> ");
    printQueue(&cpu2_2, output_file);
    fprintf(output_file, "CPU-2 que4(priority-3) (RR-q16) -> ");
    printQueue(&cpu2_3, output_file);

    fclose(output_file);

    return 0;
}

// Function definitions
void initialize(Queue *q, unsigned capacity) {
    q->capacity = capacity;
    q->front = q->size = 0;
    q->rear = capacity - 1;
    q->array = (Process *)malloc(q->capacity * sizeof(Process));
}

int isFull(Queue *q) {
    return (q->size == q->capacity);
}

int isEmpty(Queue *q) {
    return (q->size == 0);
}

void enqueue(Queue *q, Process process) {
    if (isFull(q)) {
        return;
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->array[q->rear] = process;
    q->size++;
}

Process dequeue(Queue *q) {
    Process process = q->array[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return process;
}

void printQueue(Queue *q, FILE *file) {
    int i;
    for (i = q->front; i < q->size + q->front; i++) {
        fprintf(file, "%s ", q->array[i % q->capacity].name);
    }
    fprintf(file, "\n");
}