#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define RAM_SIZE 2048
#define CPU2_QUANTUM_MEDIUM 8
#define CPU2_QUANTUM_LOW 16

// Process structure
typedef struct {
    char name[5];
    int arrival_time;
    int priority;
    int burst_time;
    int ram;
    int cpu_rate;
} Process;

// Function prototypes
void read_input_file(const char *filename, Process *processes, int *num_processes);
void assign_processes(Process *processes, int num_processes, FILE *output_file);
void print_cpu_queues(Process *processes, int num_processes);

//Helper function prototypes
void sort_processes_by_burst_time(Process *queue, int count);
void round_robin(Process *queue, int count, int quantum, FILE *output_file);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s input.txt\n", argv[0]);
        return 1;
    }

    Process processes[MAX_PROCESSES];
    int num_processes = 0;

    // Read input file
    read_input_file(argv[1], processes, &num_processes);

    // Open output file
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    // Assign processes to CPUs
    assign_processes(processes, num_processes, output_file);

    // Print CPU queues
    print_cpu_queues(processes, num_processes);

    // Close output file
    fclose(output_file);

    return 0;
}

// Read input file and parse process details
void read_input_file(const char *filename, Process *processes, int *num_processes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    while (fscanf(file, "%[^,],%d,%d,%d,%d,%d\n", 
                  processes[*num_processes].name, 
                  &processes[*num_processes].arrival_time, 
                  &processes[*num_processes].priority, 
                  &processes[*num_processes].burst_time, 
                  &processes[*num_processes].ram, 
                  &processes[*num_processes].cpu_rate) != EOF) {
        (*num_processes)++;
    }

    fclose(file);
}

// Assign processes to CPUs
void assign_processes(Process *processes, int num_processes, FILE *output_file) {

    // Temporary arrays to hold processes for different queues
    Process queueCPU1[MAX_PROCESSES];
    int countCPU1 = 0;
    Process queueCPU2_Priority1[MAX_PROCESSES];
    int countCPU2_P1 = 0;
    Process queueCPU2_Priority2[MAX_PROCESSES];
    int countCPU2_P2 = 0;
    Process queueCPU2_Priority3[MAX_PROCESSES];
    int countCPU2_P3 = 0;

    // Separate processes into different queues
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 0 && processes[i].ram <= RAM_SIZE / 4) {
            queueCPU1[countCPU1++] = processes[i];
        } else if (processes[i].priority == 1) {
            queueCPU2_Priority1[countCPU2_P1++] = processes[i];
        } else if (processes[i].priority == 2) {
            queueCPU2_Priority2[countCPU2_P2++] = processes[i];
        } else if (processes[i].priority == 3) {
            queueCPU2_Priority3[countCPU2_P3++] = processes[i];
        }
    }

// Handle CPU-1 (FCFS)
    fprintf(output_file, "CPU-1 Queue Processing:\n");
    for (int i = 0; i < countCPU1; i++) {
        fprintf(output_file, "Process %s is assigned to CPU-1.\n", queueCPU1[i].name);
        // Process execution...
        fprintf(output_file, "Process %s is completed and terminated.\n", queueCPU1[i].name);
    }

    // Handle CPU-2 Priority-1 (SJF)
    sort_processes_by_burst_time(queueCPU2_Priority1, countCPU2_P1);
    fprintf(output_file, "CPU-2 Priority-1 Queue Processing (SJF):\n");
    for (int i = 0; i < countCPU2_P1; i++) {
        fprintf(output_file, "Process %s is assigned to CPU-2.\n", queueCPU2_Priority1[i].name);
        // Process execution...
        fprintf(output_file, "Process %s is completed and terminated.\n", queueCPU2_Priority1[i].name);
    }

    // Handle CPU-2 Priority-2 (RR with quantum 8)
    fprintf(output_file, "CPU-2 Priority-2 Queue Processing (RR-q8):\n");
    round_robin(queueCPU2_Priority2, countCPU2_P2, CPU2_QUANTUM_MEDIUM, output_file);

    // Handle CPU-2 Priority-3 (RR with quantum 16)
    fprintf(output_file, "CPU-2 Priority-3 Queue Processing (RR-q16):\n");
    round_robin(queueCPU2_Priority3, countCPU2_P3, CPU2_QUANTUM_LOW, output_file);
}

//Helper function to sort processes by burst_time for SJF scheduling
void sort_processes_by_burst_time(Process *queue, int count) {
    int i, j, min_idx;

    // Selection sort algorithm
    for (i = 0; i < count - 1; i++) {
        // Get current index as min index
        min_idx = i;

        // Compare elemnts until there is a smaller element
        for (j = i + 1; j < count; j++) {
            if (queue[j].burst_time < queue[min_idx].burst_time) {
                min_idx = j;
            }
        }

        // Changes positions
        if (min_idx != i) {
            Process temp = queue[min_idx];
            queue[min_idx] = queue[i];
            queue[i] = temp;
        }
    }
}

// Helper function to handle Round Robin scheduling
void round_robin(Process *queue, int count, int quantum, FILE *output_file) {
    int time_remaining[count];  // Array to store the remaining burst time of each process

    // Initialize remaining times for each process
    for (int i = 0; i < count; i++) {
        time_remaining[i] = queue[i].burst_time;
    }

    int time = 0; // Current time marker, not necessary but useful for understanding the timeline

    // Process the queue until all jobs are completed
    while (1) {
        int done = 1; // Flag to check if all processes are completed
        for (int i = 0; i < count; i++) {
            if (time_remaining[i] > 0) {
                done = 0; // There's still a process which is not done
                if (time_remaining[i] > quantum) {
                    time += quantum;
                    time_remaining[i] -= quantum;
                    fprintf(output_file, "Process %s runs for %d units.\n", queue[i].name, quantum);
                } else {
                    time += time_remaining[i];
                    fprintf(output_file, "Process %s runs for %d units, completing its execution.\n", queue[i].name, time_remaining[i]);
                    time_remaining[i] = 0;
                    fprintf(output_file, "Process %s is completed and terminated.\n", queue[i].name);
                }
            }
        }
        if (done == 1) {
            break; // Exit loop if all processes are completed
        }
    }
}

// Print CPU queues
void print_cpu_queues(Process *processes, int num_processes) {
    printf("CPU-1 que1(priority-0) (FCFS)→ ");
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 0 && processes[i].ram <= RAM_SIZE / 4) {
            printf("%s-", processes[i].name);
        }
    }
    printf("\n");

    printf("CPU-2 que2(priority-1) (SJF)→ ");
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 1) {
            printf("%s-", processes[i].name);
        }
    }
    printf("\n");

    printf("CPU-2 que3(priority-2) (RR-q8)→ ");
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 2) {
            printf("%s-", processes[i].name);
        }
    }
    printf("\n");

    printf("CPU-2 que4(priority-3) (RR-q16)→ ");
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 3) {
            printf("%s-", processes[i].name);
        }
    }
    printf("\n");
}
