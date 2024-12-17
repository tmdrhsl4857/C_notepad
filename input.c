#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // Windows 환경에서 색상 변경에 필요

#define NAME_LENGTH 100
char dummy;  // getchar 경고 없애기 용도

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

// 전역 변수
Todo todos[MAX_TODOS];
int todoCount = 0;

#define MAX_NAME_LEN 50
#define MAX_DATE_LEN 10
#define MAX_RECORDS 100

// 데이터 구조체 정의
typedef struct KcalRecord {
    char foodName[MAX_NAME_LEN];
    int calories;
    char date[MAX_DATE_LEN];
    struct KcalRecord* next; // 연결리스트의 다음 노드
} KcalRecord;

// 연결리스트의 시작 노드
KcalRecord* head = NULL;

#define MAX_DAYS 7
#define MAX_PERIODS 10
#define MAX_LENGTH 100

// 요일 배열
const char* days[] = { "월", "화", "수", "목", "금", "토", "일" };

// 전역 변수: 시간표 배열
char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH] = { 0 };

void clearScreen();
void printHelp();
void initItemList(ItemList* list);
void resizeItemList(ItemList* list);
void addItem(ItemList* list, const char* itemName);
void printItemList(const ItemList* list);
void freeItem(Item* item);
void freeItemList(ItemList* list);
void deleteItem(ItemList* list, int index);
void editItemName(Item* item, const char* newName);
void saveItemListToFile(const ItemList* list, FILE* file);
void saveDatabaseToFile(const char* filename, const ItemList* rootList);
void loadItemListFromFile(ItemList* list, FILE* file);
void loadDatabaseFromFile(const char* filename, ItemList* rootList);
void printAvailableModules();
void runAccountingModule(ItemList* rootList);
void loadTodoListFromFile(const char* filename, Todo todos[], int* count);
void saveTodoListToFile(const char* filename, Todo todos[], int count);
void addTodo();
void displayTodos();
void deleteTodo();
void runTodolistModule();
void navigateItem(Item* item);
void cleanup(ItemList* list);
void printBanner(const char* title);
void setTextColor(int color);
void printMenu();
void handleModuleChoice(ItemList* rootList);

void loadFromFile();


void clearInputBuffer();
void handleInvalidInput();
void clearInputBuffer_();
int isValidDate(const char* date);
void addKcalRecord(const char* foodName, int calories, const char* date);
void printKcalData();
void saveToFile();
void freeMemory();
void runKcalInputModule();
int deleteKcalRecord(const char* foodName, const char* date);


void resetTimetable();
void saveTimetableToFile();
void inputTimetable();
void displayTimetable();
void runTimetableInputModule();
void copyDatabaseFile(const char* sourceFile, const char* destFile);
void loadAccountingData(const char* filename, Record* incomeRecords, int* incomeCount, Record* expenseRecords, int* expenseCount);
void deleteRecord(Record records[], int* count, char* recordType);

int main() {
    ItemList rootList;
    initItemList(&rootList);
    atexit(cleanup); // 프로그램 종료 시 메모리 해제

    // 도움말 출력
    char helpChoice;
    setTextColor(15);
    printf("프로그램을 시작하기 전에 도움말을 보시겠습니까? (Y/N): ");
    setTextColor(7);
    scanf(" %c", &helpChoice);
    dummy = getchar(); // 버퍼 비우기
    if (helpChoice == 'Y' || helpChoice == 'y') {
        printHelp();
    }

    // 데이터 로드
    loadDatabaseFromFile("database.txt", &rootList);

    char choice; // char로 변경하여 숫자와 문자를 모두 처리

    while (1) {
        printMenu(); // 메뉴 출력
        printf("선택: ");
        scanf(" %c", &choice); // 단일 문자 입력 처리
        dummy = getchar(); // 버퍼 비우기

        switch (choice) {
        case '1': {
            clearScreen();
            setTextColor(11);
            printBanner("새 항목 추가");
            char itemName[NAME_LENGTH];
            setTextColor(10);
            printf("추가할 항목 이름: ");
            setTextColor(7);
            fgets(itemName, sizeof(itemName), stdin);
            itemName[strcspn(itemName, "\n")] = '\0'; // 개행 문자 제거
            addItem(&rootList, itemName);
            break;
        }
        case '2': {
            clearScreen();
            printBanner("항목 삭제");
            if (rootList.size == 0) {
                setTextColor(12); // 빨간색
                printf("항목이 없습니다. 아무 키나 누르면 돌아갑니다...\n");
                dummy = getchar();
                setTextColor(7); // 기본 색상
                break;
            }
            printItemList(&rootList);
            setTextColor(12);
            printf("삭제할 항목 번호를 입력하세요 (0: 취소): ");
            setTextColor(7);
            int deleteIndex;
            scanf("%d", &deleteIndex);
            dummy = getchar();
            if (deleteIndex == 0) {
                setTextColor(12);
                printf("삭제를 취소했습니다.\n");
                setTextColor(7);
            }
            else if (deleteIndex > 0 && deleteIndex <= rootList.size) {
                deleteItem(&rootList, deleteIndex - 1);
            }
            else {
                setTextColor(4);
                printf("잘못된 선택입니다.\n");
                setTextColor(7);
            }
            break;
        }
        case '3': {
            clearScreen();
            printBanner("항목 수정");
            if (rootList.size == 0) {
                setTextColor(12); // 빨간색
                printf("항목이 없습니다. 아무 키나 누르면 돌아갑니다...\n");
                dummy = getchar();
                setTextColor(7); // 기본 색상
                break;
            }
            printItemList(&rootList);
            setTextColor(14);
            printf("수정할 항목 번호를 입력하세요 (0: 취소): ");
            setTextColor(7);
            int editIndex;
            scanf("%d", &editIndex);
            dummy = getchar();
            if (editIndex == 0) {
                setTextColor(12);
                printf("수정을 취소했습니다.\n");
                setTextColor(7);
            }
            else if (editIndex > 0 && editIndex <= rootList.size) {
                char itemName[NAME_LENGTH];
                setTextColor(10);
                printf("새 이름: ");
                setTextColor(7);
                fgets(itemName, sizeof(itemName), stdin);
                itemName[strcspn(itemName, "\n")] = '\0';
                editItemName(rootList.items[editIndex - 1], itemName);
            }
            else {
                setTextColor(4);
                printf("잘못된 선택입니다.\n");
                setTextColor(7);
            }
            break;
        }
        case '4': {
            clearScreen();
            printBanner("항목 선택");
            if (rootList.size == 0) {
                setTextColor(12); // 빨간색
                printf("항목이 없습니다. 아무 키나 누르면 돌아갑니다...\n");
                dummy = getchar();
                setTextColor(7); // 기본 색상
                break;
            }
            printItemList(&rootList);
            setTextColor(14);
            printf("선택할 항목 번호를 입력하세요 (0: 취소): ");
            setTextColor(7);
            int index;
            scanf("%d", &index);
            dummy = getchar();
            if (index > 0 && index <= rootList.size) {
                navigateItem(rootList.items[index - 1]);
            }
            else {
                setTextColor(4);
                printf("잘못된 선택입니다.\n");
                setTextColor(7);
            }
            break;
        }
        case '5':
            clearScreen();
            printBanner("프로그램 종료");
            saveDatabaseToFile("database.txt", &rootList);
            printf("데이터를 저장하고 프로그램을 종료합니다.\n");
            freeItemList(&rootList);
            return 0;
        case 'n':
        case 'N':
            handleModuleChoice(&rootList);
            break;
        default:
            setTextColor(4); // 빨간색
            printf("잘못된 선택입니다. 다시 시도해주세요.\n");
            setTextColor(7); // 기본 색상
            printf("\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
            break;
        }
    }
}


void clearScreen() {
    system("cls"); // Windows에서 화면을 지움
}

// 도움말 출력 함수
void printHelp() {
    clearScreen();
    setTextColor(15);
    printf("\n========== 도움말 =========="
        "\n이 프로그램은 사용자 항목 관리 기능을 제공합니다. \n사용자는 항목을 추가하고, 삭제하고, 수정할 수 있으며, 하위 항목을 탐색할 수 있습니다.\n"
        "\n1. 새 항목 추가: 새 항목을 추가합니다.\n"
        "\n2. 항목 삭제: 기존 항목을 삭제합니다.\n"
        "\n3. 항목 수정: 기존 항목의 이름을 변경합니다.\n"
        "\n4. 항목 선택: 특정 항목을 선택하여 그 하위 항목을 관리합니다.\n"
        "\n5. 종료: 프로그램을 종료하고 데이터베이스를 저장합니다.\n"
        "\nn. 사용 가능한 모듈: 현재 지원 가능한 모듈을 확인하고 실행합니다.\n"
        "\n============================\n");
    printf("\n아무 키나 누르면 계속합니다...");
    dummy = getchar();
}

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
    clearScreen();
    printBanner("항목 목록");
    setTextColor(11); // 청록색
    printf("번호      항목 이름\n");
    printf("---------------------\n");
    for (int i = 0; i < list->size; i++) {
        printf("%-10d%s\n", i + 1, list->items[i]->name);
    }
    printf("---------------------\n");
    setTextColor(7); // 기본 색상
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
    setTextColor(12);
    printf("항목 삭제 완료!\n");
    setTextColor(7);
}

// 항목 이름 수정 함수
void editItemName(Item* item, const char* newName) {
    strcpy(item->name, newName);
    setTextColor(14);
    printf("항목 이름이 '%s'로 변경되었습니다.\n", newName);
    setTextColor(7);
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
        setTextColor(4);
        fprintf(stderr, "파일 저장 실패\n");
        setTextColor(7);
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
    setTextColor(11);
    printf("======================");
    printf("\n현재 지원 가능한 모듈:\n");
    printf("1. 가계부 모듈\n");
    printf("2. ToDolist 모듈\n");
    printf("3. Kcal 모듈\n");
    printf("4. 시간표 모듈\n");
    printf("======================\n");
    setTextColor(7);
}

// 가계부 모듈 실행 함수
void runAccountingModule(ItemList* rootList) {
    int incomeCount = 0, expenseCount = 0;
    Record incomeRecords[100]; // 수익 구조체 배열
    Record expenseRecords[100]; // 지출 구조체 배열

    // 데이터 불러오기
    loadAccountingData("database.txt", incomeRecords, &incomeCount, expenseRecords, &expenseCount);

    while (1) {
        clearScreen();
        printf("\n========== 가계부 전용 입력 모듈 ==========\n"
            "1. 수익 입력\n"
            "2. 지출 입력\n"
            "3. 수익/지출 내역 보기\n"
            "4. 수익/지출 삭제\n"
            "5. 종료\n"
            "===========================================\n");
        printf("선택: ");
        int moduleChoice;
        scanf("%d", &moduleChoice);
        getchar(); // 버퍼 비우기

        switch (moduleChoice) {
        case 1: {
            // 수익 입력
            Record newRecord;
            newRecord.type = '+';
            printf("수익 금액: ");
            scanf("%d", &newRecord.amount);
            getchar();

            printf("출처: ");
            fgets(newRecord.description, sizeof(newRecord.description), stdin);
            newRecord.description[strcspn(newRecord.description, "\n")] = '\0';

            printf("날짜 (예: 20240101): ");
            fgets(newRecord.date, sizeof(newRecord.date), stdin);
            newRecord.date[strcspn(newRecord.date, "\n")] = '\0';

            incomeRecords[incomeCount++] = newRecord;
            break;
        }
        case 2: {
            // 지출 입력
            Record newRecord;
            newRecord.type = '-';
            printf("지출 금액: ");
            scanf("%d", &newRecord.amount);
            getchar();

            printf("출처: ");
            fgets(newRecord.description, sizeof(newRecord.description), stdin);
            newRecord.description[strcspn(newRecord.description, "\n")] = '\0';

            printf("날짜 (예: 20240101): ");
            fgets(newRecord.date, sizeof(newRecord.date), stdin);
            newRecord.date[strcspn(newRecord.date, "\n")] = '\0';

            expenseRecords[expenseCount++] = newRecord;
            break;
        }
        case 3:
            // 수익 및 지출 내역 보기
            printf("\n========== 수익 내역 ==========\n");
            for (int i = 0; i < incomeCount; i++) {
                printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
            }

            printf("\n========== 지출 내역 ==========\n");
            for (int i = 0; i < expenseCount; i++) {
                printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
            }
            printf("\n아무 키나 누르면 계속합니다...");
            getchar();
            break;
        case 4: {
            printf("\n1. 수익 삭제\n2. 지출 삭제\n선택: ");
            int deleteChoice;
            scanf("%d", &deleteChoice);
            getchar();

            if (deleteChoice == 1) {
                deleteRecord(incomeRecords, &incomeCount, "수익");
            }
            else if (deleteChoice == 2) {
                deleteRecord(expenseRecords, &expenseCount, "지출");
            }
            else {
                printf("잘못된 선택입니다.\n");
            }
            break;
        }
        case 5: {
            FILE* file = fopen("database.txt", "w");
            if (file) {
                fprintf(file, "2\n+\n%d\n", incomeCount); // 수익 정보 저장
                for (int i = 0; i < incomeCount; i++) {
                    fprintf(file, "%d %s %s\n0\n", incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                fprintf(file, "-\n%d\n", expenseCount); // 지출 정보 저장
                for (int i = 0; i < expenseCount; i++) {
                    fprintf(file, "%d %s %s\n0\n", expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                fclose(file);
                copyDatabaseFile("database.txt", "database_budget.txt");
                printf("모든 데이터가 저장되었습니다.\n");
                
                exit(0);
            }
            else {
                printf("파일 저장 중 오류가 발생했습니다.\n");
            }
            
        }
        default:
            printf("잘못된 선택입니다. 다시 입력해주세요.\n");
        }
    }
}

// 파일에서 Todo 데이터를 불러오는 함수
void loadTodoListFromFile(const char* filename, Todo todos[], int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("저장된 파일이 없거나 열 수 없습니다: %s\n", filename);
        return;
    }

    // 데이터 개수 읽기
    if (fscanf(file, "%d\n", count) != 1 || *count < 0) {
        printf("잘못된 파일 형식입니다.\n");
        fclose(file);
        return;
    }

    // Todo 데이터를 구조체 배열에 읽어오기
    for (int i = 0; i < *count; i++) {
        todos[i].date[0] = '\0';
        todos[i].type[0] = '\0';
        todos[i].task[0] = '\0';
        todos[i].status = 0;

        if (fscanf(file, "%s %s %s %d",
            todos[i].date, todos[i].type, todos[i].task, &todos[i].status) != 4) {
            printf("데이터 읽기 오류: 항목 %d가 잘못되었습니다.\n", i + 1);
            fclose(file);
            return;
        }

        int endMarker;
        if (fscanf(file, "%d", &endMarker) != 1 || endMarker != 0) { // '0' 확인
            printf("항목 끝에 '0'이 누락되었거나 잘못된 형식입니다 (항목 %d)\n", i + 1);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("파일에서 %d개의 항목을 불러왔습니다.\n", *count);
    printf("\n아무 키나 입력해주세요...");
    getchar();
}


// Todo 데이터를 파일에 저장하는 함수
void saveTodoListToFile(const char* filename, Todo todos[], int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        return;
    }

    // 파일 맨 앞에 항목 개수 기록
    fprintf(file, "%d\n", count);

    // Todo 항목 저장
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s %d\n",
            todos[i].date, todos[i].type, todos[i].task, todos[i].status);
        fprintf(file, "0\n"); // 항목의 끝에 '0' 추가
    }

    fclose(file);
    printf("Todo 항목이 파일에 저장되었습니다: %s\n", filename);
}

// 작업 추가 함수
void addTodo() {
    if (todoCount >= MAX_TODOS) {
        printf("할 일 목록이 가득 찼습니다!\n");
        return;
    }

    printf("날짜를 입력해주세요 (예: 20240101): ");
    scanf("%s", todos[todoCount].date);
    getchar(); // 버퍼 비우기

    printf("타입을 입력해주세요: ");
    fgets(todos[todoCount].type, NAME_LENGTH, stdin);
    todos[todoCount].type[strcspn(todos[todoCount].type, "\n")] = '\0';

    printf("내용을 입력해주세요: ");
    fgets(todos[todoCount].task, NAME_LENGTH, stdin);
    todos[todoCount].task[strcspn(todos[todoCount].task, "\n")] = '\0';

    printf("상태를 선택해주세요 (0: 준비, 1: 진행, 2: 완료, 3: 보관): ");
    scanf("%d", &todos[todoCount].status);

    printf("새로운 작업이 추가되었습니다!\n");
    todoCount++;

    printf("\n\n아무 키나 입력해주세요...");
    getchar();
    getchar();
}

// 작업 보기 함수
void displayTodos() {
    if (todoCount == 0) {
        printf("등록된 작업이 없습니다.\n");
        printf("\n\n아무 키나 입력해주세요...");
        getchar();
        return;
    }

    printf("\n==== Todo List ====\n");
    for (int i = 0; i < todoCount; i++) {
        printf("%d. 날짜: %s, 타입: %s, 내용: %s, 상태: %s\n",
            i + 1,
            todos[i].date,
            todos[i].type,
            todos[i].task,
            STATUS_TEXT[todos[i].status]);
    }
    printf("====================\n");

    printf("\n\n아무 키나 입력해주세요...");
    getchar();
}

// 작업 삭제 함수
void deleteTodo() {
    if (todoCount == 0) {
        printf("삭제할 작업이 없습니다.\n");

        printf("\n\n아무 키나 입력해주세요...");
        getchar();
        return;
    }

    int index;
    displayTodos();
    printf("삭제할 작업 번호를 입력하세요: ");
    scanf("%d", &index);

    if (index < 1 || index > todoCount) {
        printf("잘못된 작업 번호입니다.\n");
        return;
    }

    for (int i = index - 1; i < todoCount - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todoCount--;
    printf("작업이 삭제되었습니다.\n");

    printf("\n\n아무 키나 입력해주세요...");
    getchar();
}

// TodoList 메인 함수
void runTodolistModule() {

    printf("ToDoList 모듈을 시작합니다.\n");

    // 파일에서 데이터 불러오기
    loadTodoListFromFile("database_todolist.txt", todos, &todoCount);

    int choice;
    while (1) {
        clearScreen();
        printf("\n==== TodoList ====\n");
        printf("1. 작업 추가\n");
        printf("2. 작업 보기\n");
        printf("3. 작업 삭제\n");
        printf("4. 종료\n");
        printf("==================\n선택: ");
        scanf("%d", &choice);
        getchar(); // 버퍼 비우기

        switch (choice) {
        case 1:  // 할 일 추가
            addTodo();
            break;
        case 2:
            displayTodos();
            break;
        case 3:
            deleteTodo();
            break;
        case 4:
            saveTodoListToFile("database.txt", todos, todoCount);
            copyDatabaseFile("database.txt", "database_todolist.txt");
            printf("ToDoList 데이터를 저장하고 종료합니다.\n");
            exit(0);
            break;
        default:
            printf("잘못된 선택입니다. 다시 입력하세요.\n");
        }
    }
}

void navigateItem(Item* item) {
    int choice;
    char itemName[NAME_LENGTH];

    while (1) {
        clearScreen();
        printf("\n현재 위치: %s\n", item->name);
        printf("1. 새 하위 항목 추가\n");
        printf("2. 하위 항목 출력\n");
        printf("3. 하위 항목 삭제\n");
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

void printBanner(const char* title) {
    setTextColor(14); // 노란색
    printf("\n=================================\n");
    printf("||%-29s||\n", title);
    printf("=================================\n\n");
    setTextColor(7); // 기본 색상
}

void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void printMenu() {
    clearScreen();
    printBanner("메인 메뉴");
    setTextColor(11); // 청록색
    printf("1. 새 항목 추가\n");
    printf("2. 항목 삭제\n");
    printf("3. 항목 수정\n");
    printf("4. 항목 선택\n");
    printf("5. 종료\n");
    printf("n. 사용 가능한 모듈\n");
    setTextColor(7); // 기본 색상
    printf("==========================\n");
}


void handleModuleChoice(ItemList* rootList) {
    while (1) {
        clearScreen();
        printBanner("사용 가능한 모듈");
        printAvailableModules();
        printf("\n모듈을 선택하세요 (1, 2, 3, 4, q=종료): ");

        char moduleChoice;
        scanf(" %c", &moduleChoice);
        clearInputBuffer();

        if (moduleChoice >= '1' && moduleChoice <= '4') {
            char moduleFilename[50];
            const char* moduleNames[] = {
                "database_budget.txt",
                "database_todolist.txt",
                "database_kcal.txt",
                "database_schedule.txt"
            };

            // 모듈 데이터베이스 파일 선택
            snprintf(moduleFilename, sizeof(moduleFilename), "%s", moduleNames[moduleChoice - '1']);

            printf("%s에서 정보를 불러옵니다...\n", moduleFilename);
            copyDatabaseFile(moduleFilename, "database.txt");

            // 모듈 실행
            switch (moduleChoice) {
            case '1': runAccountingModule(rootList); break;
            case '2': runTodolistModule(); break;
            case '3': runKcalInputModule(); break;
            case '4': runTimetableInputModule(); break;
            }
        }
        else if (moduleChoice == 'q' || moduleChoice == 'Q') {
            printf("모듈 선택을 종료합니다.\n");
            printf("\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
            break;
        }
        else {
            fprintf(stderr, "잘못된 선택입니다. 다시 시도해주세요.\n");
            printf("\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
        }
    }
}

void loadFromFile() {
    FILE* file = fopen("database_kcal.txt", "r");
    if (!file) {
        printf("파일을 불러올 수 없습니다: database_kcal.txt\n");
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }

    freeMemory(); // 기존 연결리스트 초기화
    head = NULL;

    int recordCount;
    char line[256];
    char foodName[MAX_NAME_LEN];
    int calories;
    char date[MAX_DATE_LEN];

    // 파일 검증
    if (fgets(line, sizeof(line), file) == NULL || line[0] != '1') {
        printf("잘못된 파일 형식입니다.\n");
        fclose(file);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }

    if (fgets(line, sizeof(line), file) == NULL || strncmp(line, "kcal", 4) != 0) {
        printf("kcal 데이터 형식이 아닙니다.\n");
        fclose(file);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }

    // 데이터 개수 읽기
    if (fscanf(file, "%d", &recordCount) != 1 || recordCount <= 0) {
        printf("잘못된 데이터 개수입니다.\n");
        fclose(file);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }
    fgetc(file); // 개행 문자 처리

    // 데이터 읽어서 연결리스트에 저장
    for (int i = 0; i < recordCount; i++) {
        if (fgets(line, sizeof(line), file) && sscanf(line, "%s %d %s", foodName, &calories, date) == 3) {
            addKcalRecord(foodName, calories, date);
        }
        fgets(line, sizeof(line), file); // '0' 읽기
    }

    fclose(file);
    printf("데이터가 성공적으로 불러와졌습니다.\n");
    printf("\n아무 키나 누르면 계속합니다...");
    dummy = getchar();
}


void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// 연결리스트에 저장된 데이터를 출력하는 함수
void printKcalData() {
    if (!head) {
        printf("저장된 데이터가 없습니다.\n");
        return;
    }

    printf("\n==== 저장된 데이터 ====\n");
    KcalRecord* temp = head;
    while (temp) {
        printf("%s - %d kcal - %s\n", temp->foodName, temp->calories, temp->date);
        temp = temp->next;
    }
    printf("========================\n");

    printf("\n출력이 완료되었습니다.\n");
    getchar(); // 대기
}

void handleInvalidInput() {
    fprintf(stderr, "잘못된 입력입니다. 다시 시도해주세요.\n");
    clearInputBuffer_();
}

void clearInputBuffer_() {
    while (getchar() != '\n' && !feof(stdin));
}

// 유효한 날짜인지 확인하는 함수
int isValidDate(const char* date) {
    if (strlen(date) != 8) return 0;
    for (int i = 0; i < 8; i++) {
        if (!isdigit(date[i])) return 0;
    }
    return 1;
}

// 입력된 데이터를 연결리스트에 추가하는 함수
void addKcalRecord(const char* foodName, int calories, const char* date) {
    KcalRecord* newRecord = (KcalRecord*)malloc(sizeof(KcalRecord));
    if (!newRecord) {
        fprintf(stderr, "메모리 할당 실패\n");
        return;
    }

    strcpy(newRecord->foodName, foodName);
    newRecord->calories = calories;
    strcpy(newRecord->date, date);
    newRecord->next = NULL;

    if (!head) {
        head = newRecord;
    }
    else {
        KcalRecord* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newRecord;
    }
}

// 연결리스트의 데이터를 파일에 저장하는 함수
void saveToFile() {
    FILE* file = fopen("database.txt", "w");
    if (!file) {
        fprintf(stderr, "파일을 열 수 없습니다: database.txt\n");
        return;
    }

    fprintf(file, "1\nkcal\n");

    int recordCount = 0;
    KcalRecord* temp = head;

    // 데이터 개수를 계산하며 저장
    while (temp) {
        recordCount++;
        temp = temp->next;
    }

    fprintf(file, "%d\n", recordCount);
    temp = head;

    while (temp) {
        fprintf(file, "%s %d %s\n0\n", temp->foodName, temp->calories, temp->date);
        temp = temp->next;
    }

    fclose(file);
    printf("모든 데이터가 파일에 저장되었습니다.\n");
}

// 연결리스트 메모리를 해제하는 함수
void freeMemory() {
    KcalRecord* temp = head;
    while (temp) {
        KcalRecord* next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
}

// 칼로리 입력 모듈
void runKcalInputModule() {
    char resetChoice;

    loadFromFile();

    while (1) {
        clearScreen();

        printf("\n==== 칼로리 입력 모듈 ====\n");
        printf("1. 데이터 입력\n");
        printf("2. 입력 데이터 출력\n");
        printf("3. 데이터 삭제\n");
        printf("4. 프로그램 종료\n");
        printf("==========================\n");

        int choice;
        printf("\n선택: ");
        if (scanf("%d", &choice) != 1) {
            handleInvalidInput();
            continue;
        }

        switch (choice) {
        case 1: { // 데이터 입력
            char foodName[MAX_NAME_LEN];
            int calories;
            char date[MAX_DATE_LEN];

            printf("음식 이름: ");
            scanf(" %[^\n]", foodName);
            printf("칼로리: ");
            if (scanf("%d", &calories) != 1 || calories < 0) {
                handleInvalidInput();
                continue;
            }
            printf("날짜 (YYYYMMDD): ");
            scanf(" %[^\n]", date);

            if (!isValidDate(date)) {
                printf("유효하지 않은 날짜 형식입니다.\n");
                continue;
            }

            addKcalRecord(foodName, calories, date);
            printf("데이터가 추가되었습니다.\n");
            getchar();
            break;
        }
        case 2: // 데이터 출력
            printKcalData();
            break;
        case 3: { // 데이터 삭제
            if (!head) {
                printf("삭제할 데이터가 없습니다.\n");
                break;
            }
            char foodName[MAX_NAME_LEN];
            char date[MAX_DATE_LEN];
            printf("삭제할 음식 이름: ");
            scanf(" %[^\n]", foodName);
            printf("삭제할 날짜 (YYYYMMDD): ");
            scanf(" %[^\n]", date);

            if (deleteKcalRecord(foodName, date)) {
                printf("데이터가 성공적으로 삭제되었습니다.\n");
            }
            else {
                printf("삭제할 데이터를 찾을 수 없습니다.\n");
            }
            break;
        }
        case 4: // 종료
            saveToFile();
            copyDatabaseFile("database.txt", "database_kcal.txt");
            freeMemory();
            printf("프로그램을 종료합니다.\n");
            exit(0);
        default:
            printf("잘못된 선택입니다. 다시 입력해주세요.\n");
            
        }
        printf("\n아무 키나 누르면 계속합니다...\n");
        getchar();
    }
}


int deleteKcalRecord(const char* foodName, const char* date) {
    if (!head) return 0; // 데이터가 비어있으면 삭제 불가

    KcalRecord* current = head, * prev = NULL;

    while (current) {
        if (strcmp(current->foodName, foodName) == 0 && strcmp(current->date, date) == 0) {
            if (prev) {
                prev->next = current->next;
            }
            else {
                head = current->next; // 삭제할 데이터가 첫 노드인 경우
            }
            free(current);
            return 1; // 삭제 성공
        }
        prev = current;
        current = current->next;
    }
    return 0; // 삭제 실패
}

// 시간표 초기화
void resetTimetable() {
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            schedule[i][j][0] = '\0';
        }
    }
    printf("시간표가 초기화되었습니다.\n");
}

// 시간표 데이터 저장
void saveTimetableToFile() {
    FILE* file = fopen("database.txt", "w");
    if (!file) {
        fprintf(stderr, "파일을 열 수 없습니다: database.txt\n");
        return;
    }

    int count = 0;
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                count++;
            }
        }
    }

    fprintf(file, "%d\n", count); // 총 데이터 개수
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                fprintf(file, "%s %d교시: %s\n0\n", days[i], j + 1, schedule[i][j]);
            }
        }
    }

    fclose(file);
    printf("시간표가 저장되었습니다.\n");
}

// 시간표 입력
void inputTimetable() {
    char day[MAX_LENGTH];
    int period;
    char subject[MAX_LENGTH];

    printf("시간표를 입력하세요 (예: 월 1 수학): ");
    scanf(" %s %d %[^\n]", day, &period, subject);

    // 요일 인덱스 찾기
    int dayIndex = -1;
    for (int i = 0; i < MAX_DAYS; i++) {
        if (strcmp(day, days[i]) == 0) {
            dayIndex = i;
            break;
        }
    }

    if (dayIndex == -1 || period < 1 || period > MAX_PERIODS) {
        fprintf(stderr, "잘못된 입력입니다. 다시 시도해주세요.\n");
        return;
    }

    strcpy(schedule[dayIndex][period - 1], subject);
    printf("%s %d교시가 %s로 저장되었습니다.\n", days[dayIndex], period, subject);
}

// 시간표 출력
void displayTimetable() {
    printf("\n==== 현재 시간표 ====\n");
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                printf("%s %d교시: %s\n", days[i], j + 1, schedule[i][j]);
            }
        }
    }
    printf("=====================\n");
}

// 시간표 입력 모듈 실행
void runTimetableInputModule() {
    char resetChoice;

    // 경고 문구 추가
    printf("시간표 입력 모듈 실행 시 기존 데이터를 초기화할 수 있습니다. 초기화하시겠습니까? (Y/N): ");
    scanf(" %c", &resetChoice);
    clearInputBuffer_();

    if (resetChoice == 'Y' || resetChoice == 'y') {
        resetTimetable(); // 시간표 초기화
    }
    else if (resetChoice == 'N' || resetChoice == 'n') {
        printf("선택을 취소하고 초기 화면으로 돌아갑니다.\n");
        return; // 초기 화면으로 돌아가기
    }
    else {
        fprintf(stderr, "잘못된 선택입니다. 다시 시도해주세요.\n");
        return; // 초기 화면으로 돌아가기
    }

    int choice;

    while (1) {
        clearScreen();
        printf("\n==== 시간표 입력 모듈 ====\n");
        printf("1. 시간표 입력\n");
        printf("2. 시간표 보기\n");
        printf("3. 시간표 초기화\n");
        printf("4. 시간표 저장 및 종료\n");
        printf("==========================\n");
        printf("선택: ");
        scanf("%d", &choice);
        clearInputBuffer_(); // 입력 버퍼 비우기

        switch (choice) {
        case 1:
            inputTimetable();
            break;
        case 2:
            displayTimetable();
            break;
        case 3:
            resetTimetable();
            break;
        case 4:
            saveTimetableToFile();
            copyDatabaseFile("database.txt", "database_schedule.txt");
            printf("프로그램을 종료합니다.\n");
            exit(0); // 프로그램 종료
        default:
            fprintf(stderr, "잘못된 선택입니다. 다시 시도해주세요.\n");
        }
    }
}

void copyDatabaseFile(const char* sourceFile, const char* destFile) {
    FILE* source, * dest;
    char buffer[1024]; // 데이터 복사 버퍼
    size_t bytesRead;

    // 원본 파일 열기
    source = fopen(sourceFile, "rb");
    if (!source) {
        fprintf(stderr, "원본 파일을 열 수 없습니다: %s\n", sourceFile);
        return;
    }

    // 대상 파일 열기
    dest = fopen(destFile, "wb");
    if (!dest) {
        fprintf(stderr, "대상 파일을 열 수 없습니다: %s\n", destFile);
        fclose(source);
        return;
    }

    // 데이터 복사
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytesRead, dest);
    }

    // 파일 닫기
    fclose(source);
    fclose(dest);

    printf("파일이 성공적으로 복사되었습니다: %s -> %s\n", sourceFile, destFile);
}

void loadAccountingData(const char* filename, Record* incomeRecords, int* incomeCount, Record* expenseRecords, int* expenseCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        return;
    }

    int count = 0;
    char type;

    // 첫 번째 줄 확인 (2로 고정)
    if (fscanf(file, "%d", &count) != 1 || count != 2) {
        printf("잘못된 파일 형식입니다.\n");
        fclose(file);
        return;
    }

    // 수익 데이터 읽기
    if (fscanf(file, " %c", &type) == 1 && type == '+') {
        fscanf(file, "%d", incomeCount);
        for (int i = 0; i < *incomeCount; i++) {
            fscanf(file, "%d %s %s", &incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
            incomeRecords[i].type = '+'; // type 필드 설정
            fscanf(file, "%*d"); // '0' 읽기 (구분자)
        }
    }

   // 지출 데이터 읽기
    if (fscanf(file, " %c", &type) == 1 && type == '-') {
        if (fscanf(file, "%d", expenseCount) == 1) {
            for (int i = 0; i < *expenseCount; i++) {
                if (fscanf(file, "%d %s %s", &expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date) == 3) {
                    expenseRecords[i].type = '-';
                    fscanf(file, "%*d"); // '0' 구분자 처리
                }
            }
        }
    }

    fclose(file);
    printf("데이터를 성공적으로 불러왔습니다: %s\n", filename);
}

void deleteRecord(Record records[], int* count, char* recordType) {
    if (*count == 0) {
        printf("삭제할 %s 항목이 없습니다.\n", recordType);
        return;
    }

    printf("\n========== %s 내역 ==========\n", recordType);
    for (int i = 0; i < *count; i++) {
        printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, records[i].amount, records[i].description, records[i].date);
    }

    printf("\n삭제할 항목 번호를 입력하세요 (0 입력 시 취소): ");
    int deleteIndex;
    scanf("%d", &deleteIndex);
    getchar();

    if (deleteIndex <= 0 || deleteIndex > *count) {
        printf("잘못된 선택입니다. 삭제를 취소합니다.\n");
        return;
    }

    // 삭제 처리: 선택한 항목 뒤의 데이터를 앞으로 옮김
    for (int i = deleteIndex - 1; i < *count - 1; i++) {
        records[i] = records[i + 1];
    }
    (*count)--;
    printf("%s 항목이 삭제되었습니다.\n", recordType);
}
