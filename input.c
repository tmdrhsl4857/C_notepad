#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 100
char dummy;  // getchar 경고 없애기 용도

void clearScreen() {
    system("cls"); // Windows에서 화면을 지움
}

// 도움말 출력 함수
void printHelp() {
    clearScreen();
    printf("\n========== 도움말 =========="
        "\n이 프로그램은 사용자 항목 관리 기능을 제공합니다. 사용자는 항목을 추가하고, 삭제하고, 수정할 수 있으며, 하위 항목을 탐색할 수 있습니다.\n"
        "1. 새 항목 추가: 새 항목을 추가합니다.\n"
        "2. 항목 삭제: 기존 항목을 삭제합니다.\n"
        "3. 항목 수정: 기존 항목의 이름을 변경합니다.\n"
        "4. 항목 선택: 특정 항목을 선택하여 그 하위 항목을 관리합니다.\n"
        "5. 종료: 프로그램을 종료하고 데이터베이스를 저장합니다.\n"
        "n. 사용 가능한 모듈: 현재 지원 가능한 모듈을 확인하고 실행합니다.\n"
        "==========================\n");
    printf("\n아무 키나 누르면 계속합니다...");
    dummy = getchar();
}

// 항목 구조체 정의
typedef struct Item {
    char name[NAME_LENGTH];
    struct ItemList* children; // 자식 항목 리스트
} Item;

// 항목 리스트 구조체 정의
typedef struct ItemList {
    Item** items;
    int size;
    int capacity;
} ItemList;

// 가계부 레코드 구조체 정의
typedef struct Record {
    char type; // '+' for income, '-' for expense
    int amount;
    char description[NAME_LENGTH];
    char date[NAME_LENGTH];
} Record;

// 리스트 초기화 함수
void initItemList(ItemList* list) {
    list->size = 0;
    list->capacity = 2;  // 초기 용량
    list->items = (Item**)malloc(list->capacity * sizeof(Item*));
    if (list->items == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
}

// 리스트 확장 함수
void resizeItemList(ItemList* list) {
    void* temp = realloc(list->items, list->capacity * 2 * sizeof(Item*));
    if (temp == NULL) {
        fprintf(stderr, "메모리 재할당 실패\n");
        exit(1);
    }
    list->items = temp;
    list->capacity *= 2;
}

// 새로운 항목 추가 함수
void addItem(ItemList* list, const char* itemName) {
    if (list->size >= list->capacity) {
        resizeItemList(list);
    }
    Item* newItem = (Item*)malloc(sizeof(Item));
    if (newItem == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    strcpy(newItem->name, itemName);
    newItem->children = (ItemList*)malloc(sizeof(ItemList));
    if (newItem->children == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    initItemList(newItem->children);
    list->items[list->size++] = newItem;
    printf("항목 '%s' 추가 완료!\n", itemName);
}

// 항목 목록 출력 함수
void printItemList(const ItemList* list) {
    printf("\n항목 목록:\n");
    for (int i = 0; i < list->size; i++) {
        printf("%d. %s\n", i + 1, list->items[i]->name);
    }
}

// 항목 메모리 해제 함수
void freeItem(Item* item) {
    for (int i = 0; i < item->children->size; i++) {
        freeItem(item->children->items[i]);
    }
    free(item->children->items);
    free(item->children);
    free(item);
}

// 리스트 메모리 해제 함수
void freeItemList(ItemList* list) {
    for (int i = 0; i < list->size; i++) {
        freeItem(list->items[i]);
    }
    free(list->items);
}

// 항목 삭제 함수
void deleteItem(ItemList* list, int index) {
    if (index < 0 || index >= list->size) {
        printf("잘못된 인덱스입니다.\n");
        return;
    }
    freeItem(list->items[index]);
    for (int i = index; i < list->size - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->size--;
    printf("항목 삭제 완료!\n");
}

// 항목 이름 수정 함수
void editItemName(Item* item, const char* newName) {
    strcpy(item->name, newName);
    printf("항목 이름이 '%s'로 변경되었습니다.\n", newName);
}

// 데이터베이스를 파일에 저장하는 함수
void saveItemListToFile(const ItemList* list, FILE* file) {
    fprintf(file, "%d\n", list->size);  // 현재 리스트 크기 저장
    for (int i = 0; i < list->size; i++) {
        fprintf(file, "%s\n", list->items[i]->name);  // 항목 이름 저장
        saveItemListToFile(list->items[i]->children, file);
    }
}

void saveDatabaseToFile(const char* filename, const ItemList* rootList) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "파일 저장 실패\n");
        return;
    }
    saveItemListToFile(rootList, file);
    fclose(file);
    printf("데이터베이스가 파일에 저장되었습니다.\n");
}

// 파일에서 데이터베이스를 불러오는 함수
void loadItemListFromFile(ItemList* list, FILE* file) {
    int size;
    if (fscanf(file, "%d\n", &size) != 1) {  // 항목 수를 읽어옴
        return;
    }
    for (int i = 0; i < size; i++) {
        char itemName[NAME_LENGTH];
        if (fgets(itemName, sizeof(itemName), file) == NULL) {
            return;
        }
        itemName[strcspn(itemName, "\n")] = '\0';  // 개행 문자 제거
        addItem(list, itemName);
        loadItemListFromFile(list->items[list->size - 1]->children, file);
    }
}

void loadDatabaseFromFile(const char* filename, ItemList* rootList) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("저장된 데이터베이스 파일이 없거나 파일을 열 수 없습니다.\n");
        return;
    }
    loadItemListFromFile(rootList, file);
    fclose(file);
    printf("데이터베이스가 파일에서 로드되었습니다.\n");
}

// 모듈 목록 출력 함수
void printAvailableModules() {
    printf("\n현재 지원 가능한 모듈:\n");
    printf("1. 가계부 모듈\n");
    printf("2. ToDolist 모듈\n");
}

// 가계부 모듈 실행 함수
void runAccountingModule(ItemList* rootList) {
    // 가계부 모듈 시작 시 기존 정보 초기화
    freeItemList(rootList);
    initItemList(rootList);

    char choice;
    printf("가계부 모듈 실행 시 현재 저장 파일이 훼손될 가능성이 있습니다. 진행하시겠습니까? (Y/N): ");
    scanf(" %c", &choice);
    dummy = getchar(); // 버퍼 비우기
    if (choice == 'Y' || choice == 'y') {
        FILE* file = fopen("database.txt", "w");
        if (file == NULL) {
            fprintf(stderr, "파일 저장 실패\n");
            return;
        }

        int moduleChoice;
        int incomeCount = 0, expenseCount = 0;

        Record incomeRecords[100];  // 수익 저장 공간
        Record expenseRecords[100]; // 지출 저장 공간

        while (1) {
            clearScreen();
            printf("\n========== 가계부 전용 입력 모듈 =========="
                "\n1. 수익 입력"
                "\n2. 지출 입력"
                "\n3. 수익/지출 내역 보기"
                "\n4. 수익 정정(수정 및 삭제)"
                "\n5. 지출 정정(수정 및 삭제)"
                "\n6. 종료"
                "\n===========================================\n");
            printf("선택: ");
            scanf("%d", &moduleChoice);
            dummy = getchar(); // 버퍼 비우기

            if (moduleChoice == 6) {
                // 종료 시 현재 입력된 정보를 파일에 기록
                fprintf(file, "2\n+");
                fprintf(file, "\n%d\n", incomeCount);
                for (int i = 0; i < incomeCount; i++) {
                    fprintf(file, "%d %s %s\n0\n", incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                fprintf(file, "-\n%d\n", expenseCount);
                for (int i = 0; i < expenseCount; i++) {
                    fprintf(file, "%d %s %s\n0\n", expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                fclose(file);
                printf("가계부 모듈을 종료합니다. 모든 입력된 정보가 저장되었습니다.\n");
                printf("\n아무 키나 누르면 계속합니다...");
                dummy = getchar();
                exit(0); // 프로그램 종료
            }

            Record newRecord;

            switch (moduleChoice) {
            case 1:
                newRecord.type = '+';
                // 금액 입력
                printf("금액을 입력해주세요: ");
                scanf("%d", &newRecord.amount);
                dummy = getchar(); // 버퍼 비우기

                // 출처 입력
                printf("출처를 입력해주세요: ");
                fgets(newRecord.description, sizeof(newRecord.description), stdin);
                newRecord.description[strcspn(newRecord.description, "\n")] = '\0';  // 개행 문자 제거

                // 날짜 입력
                printf("날짜를 입력해주세요 (예시: 20250101): ");
                fgets(newRecord.date, sizeof(newRecord.date), stdin);
                newRecord.date[strcspn(newRecord.date, "\n")] = '\0';  // 개행 문자 제거

                // 입력된 정보를 저장
                incomeRecords[incomeCount++] = newRecord;
                break;
            case 2:
                newRecord.type = '-';
                // 금액 입력
                printf("금액을 입력해주세요: ");
                scanf("%d", &newRecord.amount);
                dummy = getchar(); // 버퍼 비우기

                // 출처 입력
                printf("출처를 입력해주세요: ");
                fgets(newRecord.description, sizeof(newRecord.description), stdin);
                newRecord.description[strcspn(newRecord.description, "\n")] = '\0';  // 개행 문자 제거

                // 날짜 입력
                printf("날짜를 입력해주세요 (예시: 20250101): ");
                fgets(newRecord.date, sizeof(newRecord.date), stdin);
                newRecord.date[strcspn(newRecord.date, "\n")] = '\0';  // 개행 문자 제거

                // 입력된 정보를 저장
                expenseRecords[expenseCount++] = newRecord;
                break;
            case 3:
                // 수익/지출 내역 보기
                printf("\n========== 수익 내역 =========="
                    "\n총 %d개의 수익 내역이 있습니다.\n\n", incomeCount);
                for (int i = 0; i < incomeCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                printf("\n========== 지출 내역 =========="
                    "\n총 %d개의 지출 내역이 있습니다.\n\n", expenseCount);
                for (int i = 0; i < expenseCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                printf("\n아무 키나 누르면 계속합니다...");
                dummy = getchar();
                break;
            case 4:
                // 수익 정정(수정 및 삭제)
                if (incomeCount == 0) {
                    printf("수정할 수익 항목이 없습니다.\n아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                    break;
                }
                printf("\n========== 수익 내역 수정 =========="
                    "\n총 %d개의 수익 내역이 있습니다.\n\n", incomeCount);
                for (int i = 0; i < incomeCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                int incomeIndex;
                printf("\n수정 또는 삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                scanf("%d", &incomeIndex);
                dummy = getchar();
                if (incomeIndex == 0 || incomeIndex > incomeCount) {
                    printf("잘못된 선택이거나 취소를 선택하셨습니다. 아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                    break;
                }
                printf("\n1. 수정\n2. 삭제\n선택: ");
                int action;
                scanf("%d", &action);
                dummy = getchar();
                if (action == 1) {
                    // 수정 기능
                    printf("\n새 금액을 입력해주세요: ");
                    scanf("%d", &incomeRecords[incomeIndex - 1].amount);
                    dummy = getchar();

                    printf("새 출처를 입력해주세요: ");
                    fgets(incomeRecords[incomeIndex - 1].description, sizeof(incomeRecords[incomeIndex - 1].description), stdin);
                    incomeRecords[incomeIndex - 1].description[strcspn(incomeRecords[incomeIndex - 1].description, "\n")] = '\0';

                    printf("새 날짜를 입력해주세요 (예시: 20250101): ");
                    fgets(incomeRecords[incomeIndex - 1].date, sizeof(incomeRecords[incomeIndex - 1].date), stdin);
                    incomeRecords[incomeIndex - 1].date[strcspn(incomeRecords[incomeIndex - 1].date, "\n")] = '\0';
                }
                else if (action == 2) {
                    // 삭제 기능
                    for (int i = incomeIndex - 1; i < incomeCount - 1; i++) {
                        incomeRecords[i] = incomeRecords[i + 1];
                    }
                    incomeCount--;
                }
                else {
                    printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                }
                break;
            case 5:
                // 지출 정정(수정 및 삭제)
                if (expenseCount == 0) {
                    printf("수정할 지출 항목이 없습니다.\n아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                    break;
                }
                printf("\n========== 지출 내역 수정 =========="
                    "\n총 %d개의 지출 내역이 있습니다.\n\n", expenseCount);
                for (int i = 0; i < expenseCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                int expenseIndex;
                printf("\n수정 또는 삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                scanf("%d", &expenseIndex);
                dummy = getchar();
                if (expenseIndex == 0 || expenseIndex > expenseCount) {
                    printf("잘못된 선택이거나 취소를 선택하셨습니다. 아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                    break;
                }
                printf("\n1. 수정\n2. 삭제\n선택: ");
                scanf("%d", &action);
                dummy = getchar();
                if (action == 1) {
                    // 수정 기능
                    printf("\n새 금액을 입력해주세요: ");
                    scanf("%d", &expenseRecords[expenseIndex - 1].amount);
                    dummy = getchar();

                    printf("새 출처를 입력해주세요: ");
                    fgets(expenseRecords[expenseIndex - 1].description, sizeof(expenseRecords[expenseIndex - 1].description), stdin);
                    expenseRecords[expenseIndex - 1].description[strcspn(expenseRecords[expenseIndex - 1].description, "\n")] = '\0';

                    printf("새 날짜를 입력해주세요 (예시: 20250101): ");
                    fgets(expenseRecords[expenseIndex - 1].date, sizeof(expenseRecords[expenseIndex - 1].date), stdin);
                    expenseRecords[expenseIndex - 1].date[strcspn(expenseRecords[expenseIndex - 1].date, "\n")] = '\0';
                }
                else if (action == 2) {
                    // 삭제 기능
                    for (int i = expenseIndex - 1; i < expenseCount - 1; i++) {
                        expenseRecords[i] = expenseRecords[i + 1];
                    }
                    expenseCount--;
                }
                else {
                    printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                }
                break;
            default:
                printf("잘못된 선택입니다. 다시 시도해주세요.\n");
                printf("\n아무 키나 누르면 계속합니다...");
                dummy = getchar();
                break;
            }
        }
    }
    else {
        printf("가계부 모듈 실행이 취소되었습니다.\n");
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
    }
}

#define NAME_LENGTH 100
#define MAX_TODOS 100

// 상태 상수 정의
const char* STATUS_TEXT[] = { "준비", "진행", "완료", "보관" };

// ToDolist 구조체 정의
typedef struct Todo {
    char date[NAME_LENGTH];   // 날짜
    char type[NAME_LENGTH];   // 타입
    char task[NAME_LENGTH];   // 내용
    int status;               // 상태: 0: 준비, 1: 진행, 2: 완료, 3: 보관
} Todo;

// ToDolist 데이터 로드 함수
void loadTodoListFromFile(const char* filename, Todo todos[], int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("저장된 ToDolist 파일이 없거나 파일을 열 수 없습니다.\n");
        return;
    }
    *count = 0;
    while (fscanf(file, "%s %s %[^\n] %d", todos[*count].date, todos[*count].type, todos[*count].task, &todos[*count].status) == 4) {
        (*count)++;
    }
    fclose(file);
}

// Todolist 데이터 저장 함수
void saveTodoListToFile(const char* filename, Todo todos[], int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "ToDolist 파일 저장 실패\n");
        return;
    }

    // 파일의 첫 줄에 데이터 개수를 기록
    fprintf(file, "%d\n", count);

    // 데이터 저장
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s\n0\n", todos[i].date, todos[i].type, todos[i].task);
    }

    fclose(file);
    printf("ToDolist 파일 저장 완료.\n");
}

// ToDolist 출력 함수
void printTodoList(Todo todos[], int count) {
    printf("\n========== 할 일 목록 ==========\n");
    for (int i = 0; i < count; i++) {
        printf("%d. 날짜: %s, 타입: %s, 내용: %s, 상태: %s\n",
            i + 1, todos[i].date, todos[i].type, todos[i].task, STATUS_TEXT[todos[i].status]);
    }
    printf("\n================================\n");
}

// ToDolist 항목 추가 함수
void addTodo(Todo todos[], int* count) {
    if (*count >= MAX_TODOS) {
        printf("할 일 목록이 가득 찼습니다!\n");
        return;
    }
    Todo newTodo;

    printf("날짜를 입력해주세요 (예시: 20250101): ");
    fgets(newTodo.date, sizeof(newTodo.date), stdin);
    newTodo.date[strcspn(newTodo.date, "\n")] = '\0';

    printf("타입을 입력해주세요: ");
    fgets(newTodo.type, sizeof(newTodo.type), stdin);
    newTodo.type[strcspn(newTodo.type, "\n")] = '\0';

    printf("내용을 입력해주세요: ");
    fgets(newTodo.task, sizeof(newTodo.task), stdin);
    newTodo.task[strcspn(newTodo.task, "\n")] = '\0';

    printf("상태를 선택해주세요 (0: 준비, 1: 진행, 2: 완료, 3: 보관): ");
    scanf("%d", &newTodo.status);
    getchar(); // 버퍼 비우기

    if (newTodo.status < 0 || newTodo.status > 3) {
        printf("잘못된 상태 선택입니다. 기본값(준비)으로 설정합니다.\n");
        newTodo.status = 0; // 기본값: 준비
    }

    todos[(*count)++] = newTodo;
    printf("새로운 할 일이 추가되었습니다!\n");
}

// ToDolist 모듈 실행 함수
void runTodolistModule() {
    Todo todos[MAX_TODOS];
    int todoCount = 0;
    loadTodoListFromFile("database.txt", todos, &todoCount);

    int choice;
    while (1) {
        clearScreen();
        printf("\n========== ToDolist 입력 모듈 ==========\n"
            "1. 할 일 입력\n"
            "2. 할 일 목록\n"
            "3. 상태 수정\n"
            "4. 할 일 정정(수정 및 삭제)\n"
            "5. 종료\n"
            "======================================\n");
        printf("선택: ");
        scanf("%d", &choice);
        getchar(); // 버퍼 비우기
        switch (choice) {
        case 1:
            addTodo(todos, &todoCount);
            break;
        case 2:
            printTodoList(todos, todoCount);
            break;
        case 3:
            updateTodoStatus(todos, todoCount);
            break;
        case 4:
            editTodo(todos, &todoCount);
            break;
        case 5:
            saveTodoListToFile("database.txt", todos, todoCount);
            printf("ToDolist 모듈을 종료합니다. 모든 변경 사항이 저장되었습니다.\n");
            return;
        default:
            printf("잘못된 선택입니다. 다시 시도해주세요.\n");
        }
    }
}

void navigateItem(Item* item) {
    int choice;
    char itemName[NAME_LENGTH];

    while (1) {
        clearScreen();
        printf("\n현재 위치: %s\n", item->name);
        printf("1. 새 항목 추가\n");
        printf("2. 항목 목록 출력\n");
        printf("3. 항목 삭제\n");
        printf("4. 상위 메뉴로 돌아가기\n");
        printf("선택: ");
        scanf("%d", &choice);
        dummy = getchar(); // 버퍼 비우기

        switch (choice) {
        case 1:
            printf("\n추가할 항목 이름을 입력하세요: ");
            fgets(itemName, sizeof(itemName), stdin);
            itemName[strcspn(itemName, "\n")] = '\0';  // 개행 문자 제거
            addItem(item->children, itemName);
            break;
        case 2:
            printItemList(item->children);
            printf("\n\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
            break;
        case 3:
            if (item->children->size == 0) {
                printf("삭제할 항목이 없습니다. 아무 키나 누르면 메뉴로 돌아갑니다...");
                dummy = getchar();
                break;
            }
            printItemList(item->children);
            printf("\n삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
            int deleteIndex;
            scanf("%d", &deleteIndex);
            dummy = getchar();  // 버퍼 비우기
            if (deleteIndex == 0) {
                printf("삭제를 취소했습니다.\n");
                dummy = getchar();
                break;
            }
            if (deleteIndex > 0 && deleteIndex <= item->children->size) {
                deleteItem(item->children, deleteIndex - 1);
            }
            else {
                printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                dummy = getchar();
            }
            break;
        case 4:
            return;
        default:
            printf("잘못된 선택입니다. 다시 시도해주세요.\n");
            printf("\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
        }
    }
}

void cleanup(ItemList* list) {
    freeItemList(list);
}

int main() {
    ItemList rootList;
    initItemList(&rootList);
    atexit(cleanup);  // 프로그램 종료 시 메모리 해제

    char helpChoice;
    printf("프로그램을 시작하기 전에 도움말을 보시겠습니까? (Y/N): ");
    scanf(" %c", &helpChoice);
    dummy = getchar(); // 버퍼 비우기
    if (helpChoice == 'Y' || helpChoice == 'y') {
        printHelp();
    }

    loadDatabaseFromFile("database.txt", &rootList);  // 프로그램 시작 시 데이터 로드

    char itemName[NAME_LENGTH];
    int choice;

    while (1) {
        clearScreen();
        printf("\n========== 메뉴 ==========");
        printf("\n1. 새 항목 추가\n");
        printf("2. 항목 삭제\n");
        printf("3. 항목 수정\n");
        printf("4. 항목 선택\n");
        printf("5. 종료\n");
        printf("n. 사용 가능한 모듈\n");
        printf("==========================\n");
        printf("선택: ");
        if (scanf("%d", &choice) == 1) {
            dummy = getchar();  // 버퍼 비우기

            switch (choice) {
            case 1:
                printf("\n추가할 항목 이름을 입력하세요: ");
                fgets(itemName, sizeof(itemName), stdin);
                itemName[strcspn(itemName, "\n")] = '\0';  // 개행 문자 제거
                addItem(&rootList, itemName);
                break;
            case 2:
                if (rootList.size == 0) {
                    printf("항목이 없습니다. 아무 키나 누르면 메뉴로 돌아갑니다...");
                    dummy = getchar();
                    break;
                }
                printItemList(&rootList);
                printf("\n삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                int deleteIndex;
                scanf("%d", &deleteIndex);
                dummy = getchar();  // 버퍼 비우기
                if (deleteIndex == 0) {
                    printf("삭제를 취소했습니다.\n");
                    dummy = getchar();
                    break;
                }
                deleteItem(&rootList, deleteIndex - 1);
                break;
            case 3:
                if (rootList.size == 0) {
                    printf("항목이 없습니다. 아무 키나 누르면 메뉴로 돌아갑니다...");
                    dummy = getchar();
                    break;
                }
                printItemList(&rootList);
                printf("\n수정할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                int editIndex;
                scanf("%d", &editIndex);
                dummy = getchar();  // 버퍼 비우기
                if (editIndex == 0) {
                    printf("수정을 취소했습니다.\n");
                    dummy = getchar();
                    break;
                }
                if (editIndex > 0 && editIndex <= rootList.size) {
                    printf("새로운 항목 이름을 입력하세요: ");
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = '\0';  // 개행 문자 제거
                    editItemName(rootList.items[editIndex - 1], itemName);
                }
                else {
                    printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                }
                break;
            case 4:
                if (rootList.size == 0) {
                    printf("항목이 없습니다. 아무 키나 누르면 메뉴로 돌아갑니다...");
                    dummy = getchar();
                    break;
                }
                printItemList(&rootList);
                printf("\n몇 번째 항목을 선택하시겠습니까? (번호 입력): ");
                int index;
                scanf("%d", &index);
                dummy = getchar();  // 버퍼 비우기

                if (index > 0 && index <= rootList.size) {
                    navigateItem(rootList.items[index - 1]);
                }
                else {
                    printf("잘못된 선택입니다. 아무 키나 누르면 메뉴로 돌아갑니다...");
                    dummy = getchar();
                }
                break;

            case 5:
                saveDatabaseToFile("database.txt", &rootList);  // 프로그램 종료 시 데이터 저장
                printf("프로그램을 종료합니다.\n");
                freeItemList(&rootList);
                return 0;
            default:
                printf("잘못된 선택입니다. 다시 시도해주세요.\n");
                printf("\n아무 키나 누르면 계속합니다...");
                dummy = getchar();
            }
        }
        else {
            char moduleChoice;
            dummy = getchar();  // 버퍼 비우기
            if (dummy == 'n' || dummy == 'N') {
                printAvailableModules();
                printf("\n모듈을 선택하세요 (1): ");
                scanf(" %c", &moduleChoice);
                dummy = getchar(); // 버퍼 비우기
                if (moduleChoice == '1') {
                    runAccountingModule(&rootList);
                }
                else {
                    printf("잘못된 선택입니다.\n아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                }
            }
        }
    }

    return 0;
}
