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
void sortTasks(Task tasks[], int taskCount, int sortBy);
void formatDate(const char* rawDate, char* formattedDate);

int main() {
    Task tasks[MAX_TASKS];
    int taskCount = 0;
    int choice;

    // Load tasks from file
    loadTasksFromFile(tasks, &taskCount, "database_todolist.txt");

    while (1) {
        printf("\nTo-Do List\n");
        printf("1. todolist 출력\n");
        printf("2. 나가기\n");
        printf("옵션을 선택해 주세요: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        switch (choice) {
        case 1:
            system("cls"); // Clear the screen (use "cls" for Windows)
            printf("\n정렬 방식:\n1. 날짜\n2. 유형\n3. 내용\n4. 상태\n옵션을 선택해 주세요: ");
            int sortBy;
            scanf("%d", &sortBy);
            getchar(); // Consume the newline character
            sortTasks(tasks, taskCount, sortBy);
            printTasks(tasks, taskCount);
            break;
        case 2:
            saveTasksToFile(tasks, taskCount, "database.txt");
            return 0;
        default:
            printf("잘못된 입력입니다. 다시 시도해주세요.\n");
        }
    }
}

void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("%s 파일을 읽기 형태로 열 수 없습니다.\n", filename);
        return;
    }

    fscanf(file, "%d\n", taskCount); // Read the total number of tasks

    for (int i = 0; i < *taskCount; i++) {
        fscanf(file, "%99s %99s %99s %99s\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status);
        int endMarker;
        fscanf(file, "%d\n", &endMarker); // Read the end marker (0)
        if (endMarker != 0) {
            printf("파일 형식이 잘못되었습니다. 마무리 문자 '0'이 없습니다.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);
}

void saveTasksToFile(const Task tasks[], int taskCount, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("%s 파일을 쓰기 형태로 열 수 없습니다.\n", filename);
        return;
    }

    fprintf(file, "%d\n", taskCount); // Write the total number of tasks

    for (int i = 0; i < taskCount; i++) {
        fprintf(file, "%s %s %s %s\n0\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status);
    }

    fclose(file);
}

void printTasks(const Task tasks[], int taskCount) {
    system("cls"); // Clear the screen (use "cls" for Windows)
    if (taskCount == 0) {
        printf("목록이 존재하지 않습니다.\n");
        return;
    }

    printf("\n%-5s %-15s %-15s %-30s %-15s\n", "번호", "날짜", "유형", "내용", "상태");
    printf("-------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < taskCount; i++) {
        char formattedDate[MAX_STRING_LENGTH];
        formatDate(tasks[i].date, formattedDate); // Format the raw date
        printf("%-5d %-15s %-15s %-30s %-15s\n", i + 1, formattedDate, tasks[i].type, tasks[i].title, tasks[i].status);
    }
}

// Helper function to format the date
void formatDate(const char* rawDate, char* formattedDate) {
    if (strlen(rawDate) != 8) { // Ensure the date is in the correct format (YYYYMMDD)
        strcpy(formattedDate, rawDate);
        return;
    }

    char year[5], month[3], day[3];
    strncpy(year, rawDate, 4);
    year[4] = '\0'; // Null-terminate the string

    strncpy(month, rawDate + 4, 2);
    month[2] = '\0'; // Null-terminate the string

    strncpy(day, rawDate + 6, 2);
    day[2] = '\0'; // Null-terminate the string

    snprintf(formattedDate, MAX_STRING_LENGTH, "%s-%s-%s", year, month, day);
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
    const char* statusOrder[] = { "준비", "진행", "완료", "보관" };
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
        printf("잘못된 선택입니다.\n");
    }
}
