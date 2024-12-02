#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_STRING_LENGTH 100

// Task structure
typedef struct {
    char date[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char title[MAX_STRING_LENGTH];
    char status[MAX_STRING_LENGTH]; // Task status: "Not Started", "In Progress", "Completed", "Archived"
} Task;

// Function prototypes
void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename);
void saveTasksToFile(const Task tasks[], int taskCount, const char* filename);
void printTasks(const Task tasks[], int taskCount);
void updateTaskStatus(Task tasks[], int taskCount);
void sortTasks(Task tasks[], int taskCount, int sortBy);

int main() {
    Task tasks[MAX_TASKS];
    int taskCount = 0;
    int choice;

    // Load tasks from file
    loadTasksFromFile(tasks, &taskCount, "database.txt");

    while (1) {
        printf("\nTo-Do List\n");
        printf("1. Show Tasks\n");
        printf("2. Update Task Status\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        switch (choice) {
        case 1:
            system("cls"); // Clear the screen (use "cls" for Windows)
            printf("\nSort by:\n1. Date\n2. Type\n3. Title\n4. Status\nChoose an option: ");
            int sortBy;
            scanf("%d", &sortBy);
            getchar(); // Consume the newline character
            sortTasks(tasks, taskCount, sortBy);
            printTasks(tasks, taskCount);
            break;
        case 2:
            updateTaskStatus(tasks, taskCount);
            break;
        case 3:
            saveTasksToFile(tasks, taskCount, "database.txt");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}

void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s for reading.\n", filename);
        return;
    }

    fscanf(file, "%d\n", taskCount); // Read the total number of tasks

    for (int i = 0; i < *taskCount; i++) {
        fscanf(file, "%99[^,], %99[^,], %99[^,], %99[^\n]\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status); // Read date, type, title, status
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

void saveTasksToFile(const Task tasks[], int taskCount, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Could not open file %s for writing.\n", filename);
        return;
    }

    fprintf(file, "%d\n", taskCount); // Write the total number of tasks

    for (int i = 0; i < taskCount; i++) {
        if (strcmp(tasks[i].status, "0") == 0 || (strcmp(tasks[i].status, "Not Started") != 0 && strcmp(tasks[i].status, "In Progress") != 0 && strcmp(tasks[i].status, "Completed") != 0 && strcmp(tasks[i].status, "Archived") != 0)) {
            strcpy(tasks[i].status, "Not Started");
        }
        fprintf(file, "%s, %s, %s, %s\n0\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status); // Write date, type, title, status, and end marker
    }

    fclose(file);
}

void printTasks(const Task tasks[], int taskCount) {
    if (taskCount == 0) {
        printf("No tasks available.\n");
        return;
    }

    printf("\n%-15s %-15s %-30s %-15s\n", "Date", "Type", "Title", "Status");
    printf("-------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < taskCount; i++) {
        printf("%-15s %-15s %-30s %-15s\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status);
    }
}

void updateTaskStatus(Task tasks[], int taskCount) {
    system("cls"); // Clear the screen (use "cls" for Windows)
    if (taskCount == 0) {
        printf("No tasks available to update.\n");
        return;
    }

    printTasks(tasks, taskCount);
    printf("\nEnter the task number to update the status (1 to %d): ", taskCount);
    int index;
    scanf("%d", &index);
    getchar(); // Consume the newline character

    if (index < 1 || index > taskCount) {
        printf("Invalid task number.\n");
        return;
    }

    printf("\nEnter new status (1: Not Started, 2: In Progress, 3: Completed, 4: Archived): ");
    int statusChoice;
    scanf("%d", &statusChoice);
    getchar(); // Consume the newline character

    switch (statusChoice) {
    case 1:
        strcpy(tasks[index - 1].status, "Not Started");
        break;
    case 2:
        strcpy(tasks[index - 1].status, "In Progress");
        break;
    case 3:
        strcpy(tasks[index - 1].status, "Completed");
        break;
    case 4:
        strcpy(tasks[index - 1].status, "Archived");
        break;
    default:
        printf("Invalid status choice.\n");
        return;
    }

    printf("Task status updated successfully.\n");
    saveTasksToFile(tasks, taskCount, "database.txt");
}

int compareByDate(const void* a, const void* b) {
    return strcmp(((Task*)a)->date, ((Task*)b)->date);
}

int compareByType(const void* a, const void* b) {
    return strcmp(((Task*)a)->type, ((Task*)b)->type);
}

int compareByTitle(const void* a, const void* b) {
    return strcmp(((Task*)a)->title, ((Task*)b)->title);
}

int compareByStatus(const void* a, const void* b) {
    const char* statusOrder[] = { "Not Started", "In Progress", "Completed", "Archived" };
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    int indexA = 0, indexB = 0;
    for (int i = 0; i < 4; i++) {
        if (strcmp(taskA->status, statusOrder[i]) == 0) {
            indexA = i;
        }
        if (strcmp(taskB->status, statusOrder[i]) == 0) {
            indexB = i;
        }
    }
    return indexA - indexB;
}

void sortTasks(Task tasks[], int taskCount, int sortBy) {
    switch (sortBy) {
    case 1:
        qsort(tasks, taskCount, sizeof(Task), compareByDate);
        break;
    case 2:
        qsort(tasks, taskCount, sizeof(Task), compareByType);
        break;
    case 3:
        qsort(tasks, taskCount, sizeof(Task), compareByTitle);
        break;
    case 4:
        qsort(tasks, taskCount, sizeof(Task), compareByStatus);
        break;
    default:
        printf("Invalid sort option.\n");
    }
}
