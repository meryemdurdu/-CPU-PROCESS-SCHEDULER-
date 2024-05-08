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

void sort_processes_by_burst_time(Process *queue, int count);

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
    // Assign processes to CPU-1 (FCFS)
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority == 0 && processes[i].ram <= RAM_SIZE / 4) {
            fprintf(output_file, "Process %s is queued to be assigned to CPU-1.\n", processes[i].name);
            // Assign to CPU-1
            fprintf(output_file, "Process %s is assigned to CPU-1.\n", processes[i].name);
            // Process execution...
            fprintf(output_file, "Process %s is completed and terminated.\n", processes[i].name);
        } else {
            // Assign to CPU-2
            fprintf(output_file, "Process %s is queued to be assigned to CPU-2.\n", processes[i].name);
            // Assign to appropriate queue and execute based on priority and scheduling algorithm
            // Process execution...
            fprintf(output_file, "Process %s is completed and terminated.\n", processes[i].name);
        }
    }
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
