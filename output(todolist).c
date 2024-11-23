#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_STRING_LENGTH 100

// Task structure
typedef struct {
    char date[MAX_STRING_LENGTH];
    char task[MAX_STRING_LENGTH];
    char other[MAX_STRING_LENGTH];
} Task;

// Function prototypes
void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename);
void printTasks(const Task tasks[], int taskCount);

int main() {
    Task tasks[MAX_TASKS];
    int taskCount = 0;

    // Load tasks from file
    loadTasksFromFile(tasks, &taskCount, "database.txt");

    // Show tasks
    printTasks(tasks, taskCount);

    return 0;
}

void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s for reading.\n", filename);
        return;
    }

    fscanf(file, "%d\n", taskCount); // Read the total number of tasks

    for (int i = 0; i < *taskCount; i++) {
        fscanf(file, "%99[^,], %99[^,], %99[^\n]\n", tasks[i].date, tasks[i].task, tasks[i].other); // Read date, task, other
        int endMarker;
        fscanf(file, "%d\n", &endMarker); // Read the end marker (0)
        if (endMarker != 0) {
            printf("Error in file format. Expected end marker '0'.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);
}

void printTasks(const Task tasks[], int taskCount) {
    if (taskCount == 0) {
        printf("No tasks available.\n");
        return;
    }

    printf("\n%-15s %-30s %-30s\n", "Date", "Task", "Other Details");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < taskCount; i++) {
        printf("%-15s %-30s %-30s\n", tasks[i].date, tasks[i].task, tasks[i].other);
    }
}
