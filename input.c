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
void printTodoList(Todo todos[], int count);
void addTodo(Todo todos[], int* count);
void updateTodoStatus(Todo todos[], int count);
void editTodo(Todo todos[], int* count);
void runTodolistModule();
void navigateItem(Item* item);
void cleanup(ItemList* list);
void printBanner(const char* title);
void setTextColor(int color);
void printMenu();
void handleModuleChoice(ItemList* rootList);
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
        "2. 항목 삭제: 기존 항목을 삭제합니다.\n"
        "3. 항목 수정: 기존 항목의 이름을 변경합니다.\n"
        "4. 항목 선택: 특정 항목을 선택하여 그 하위 항목을 관리합니다.\n"
        "5. 종료: 프로그램을 종료하고 데이터베이스를 저장합니다.\n"
        "n. 사용 가능한 모듈: 현재 지원 가능한 모듈을 확인하고 실행합니다.\n"
        "============================\n");
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
    printf("======================\n");
    setTextColor(7);
}

// 가계부 모듈 실행 함수
void runAccountingModule(ItemList* rootList) {
    // 가계부 모듈 시작 시 기존 정보 초기화
    freeItemList(rootList);
    initItemList(rootList);

    char choice;
    setTextColor(4);
    printf("가계부 모듈 실행 시 현재 저장 파일이 훼손될 가능성이 있습니다. 진행하시겠습니까? (Y/N): ");
    setTextColor(7);
    scanf(" %c", &choice);
    dummy = getchar(); // 버퍼 비우기
    if (choice == 'Y' || choice == 'y') {
        FILE* file = fopen("database.txt", "w");
        if (file == NULL) {
            setTextColor(4);
            fprintf(stderr, "파일 저장 실패\n");
            setTextColor(7);
            return;
        }

        int moduleChoice;
        int incomeCount = 0, expenseCount = 0;

        Record incomeRecords[100];  // 수익 저장 공간
        Record expenseRecords[100]; // 지출 저장 공간

        while (1) {
            clearScreen();
            setTextColor(11);
            printf("\n========== 가계부 전용 입력 모듈 =========="
                "\n1. 수익 입력"
                "\n2. 지출 입력"
                "\n3. 수익/지출 내역 보기"
                "\n4. 수익 정정(수정 및 삭제)"
                "\n5. 지출 정정(수정 및 삭제)"
                "\n6. 종료"
                "\n===========================================\n");
            setTextColor(7);
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
                setTextColor(2);
                printf("수익 금액을 입력해주세요: ");
                scanf("%d", &newRecord.amount);
                dummy = getchar(); // 버퍼 비우기

                // 출처 입력
                setTextColor(6);
                printf("출처를 입력해주세요: ");
                fgets(newRecord.description, sizeof(newRecord.description), stdin);
                newRecord.description[strcspn(newRecord.description, "\n")] = '\0';  // 개행 문자 제거

                // 날짜 입력
                setTextColor(7);
                printf("날짜를 입력해주세요 (예시: 20250101): ");
                fgets(newRecord.date, sizeof(newRecord.date), stdin);
                newRecord.date[strcspn(newRecord.date, "\n")] = '\0';  // 개행 문자 제거

                // 입력된 정보를 저장
                incomeRecords[incomeCount++] = newRecord;
                break;
            case 2:
                newRecord.type = '-';
                // 금액 입력
                setTextColor(2);
                printf("지출 금액을 입력해주세요: ");
                scanf("%d", &newRecord.amount);
                dummy = getchar(); // 버퍼 비우기

                // 출처 입력
                setTextColor(6);
                printf("출처를 입력해주세요: ");
                fgets(newRecord.description, sizeof(newRecord.description), stdin);
                newRecord.description[strcspn(newRecord.description, "\n")] = '\0';  // 개행 문자 제거

                // 날짜 입력
                setTextColor(7);
                printf("날짜를 입력해주세요 (예시: 20250101): ");
                fgets(newRecord.date, sizeof(newRecord.date), stdin);
                newRecord.date[strcspn(newRecord.date, "\n")] = '\0';  // 개행 문자 제거

                // 입력된 정보를 저장
                expenseRecords[expenseCount++] = newRecord;
                break;
            case 3:
                // 수익/지출 내역 보기
                setTextColor(2);
                printf("\n========== 수익 내역 =========="
                    "\n총 %d개의 수익 내역이 있습니다.\n\n", incomeCount);
                setTextColor(10);
                for (int i = 0; i < incomeCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                setTextColor(4);
                printf("\n========== 지출 내역 =========="
                    "\n총 %d개의 지출 내역이 있습니다.\n\n", expenseCount);
                setTextColor(12);
                for (int i = 0; i < expenseCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                setTextColor(7);
                printf("\n아무 키나 누르면 계속합니다...");
                dummy = getchar();
                break;
            case 4:
                // 수익 정정(수정 및 삭제)
                if (incomeCount == 0) {
                    setTextColor(12);
                    printf("수정할 수익 항목이 없습니다.\n아무 키나 누르면 계속합니다...");
                    dummy = getchar();
                    break;
                }
                setTextColor(2);
                printf("\n========== 수익 내역 수정 =========="
                    "\n총 %d개의 수익 내역이 있습니다.\n\n", incomeCount);
                setTextColor(10);
                for (int i = 0; i < incomeCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, incomeRecords[i].amount, incomeRecords[i].description, incomeRecords[i].date);
                }
                int incomeIndex;
                setTextColor(14);
                printf("\n수정 또는 삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                setTextColor(7);
                scanf("%d", &incomeIndex);
                dummy = getchar();
                if (incomeIndex == 0 || incomeIndex > incomeCount) {
                    setTextColor(12);
                    printf("잘못된 선택이거나 취소를 선택하셨습니다. 아무 키나 누르면 계속합니다...");
                    setTextColor(7);
                    dummy = getchar();
                    break;
                }
                setTextColor(14);
                printf("\n1. 수정");
                setTextColor(12);
                printf("\n2. 삭제");
                setTextColor(7);
                printf("\n선택: ");
                int action;
                scanf("%d", &action);
                dummy = getchar();
                if (action == 1) {
                    // 수정 기능
                    setTextColor(2);
                    printf("\n새 수익 금액을 입력해주세요: ");
                    scanf("%d", &incomeRecords[incomeIndex - 1].amount);
                    dummy = getchar();

                    setTextColor(6);
                    printf("새 출처를 입력해주세요: ");
                    fgets(incomeRecords[incomeIndex - 1].description, sizeof(incomeRecords[incomeIndex - 1].description), stdin);
                    incomeRecords[incomeIndex - 1].description[strcspn(incomeRecords[incomeIndex - 1].description, "\n")] = '\0';

                    setTextColor(7);
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
                    setTextColor(4);
                    printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                    setTextColor(7);
                    dummy = getchar();
                }
                break;
            case 5:
                // 지출 정정(수정 및 삭제)
                if (expenseCount == 0) {
                    setTextColor(12);
                    printf("수정할 지출 항목이 없습니다.\n아무 키나 누르면 계속합니다...");
                    setTextColor(7);
                    dummy = getchar();
                    break;
                }
                setTextColor(4);
                printf("\n========== 지출 내역 수정 =========="
                    "\n총 %d개의 지출 내역이 있습니다.\n\n", expenseCount);
                setTextColor(12);
                for (int i = 0; i < expenseCount; i++) {
                    printf("%d. 금액: %d, 출처: %s, 날짜: %s\n", i + 1, expenseRecords[i].amount, expenseRecords[i].description, expenseRecords[i].date);
                }
                setTextColor(7);
                int expenseIndex;
                setTextColor(14);
                printf("\n수정 또는 삭제할 항목 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
                setTextColor(7);
                scanf("%d", &expenseIndex);
                dummy = getchar();
                if (expenseIndex == 0 || expenseIndex > expenseCount) {
                    setTextColor(12);
                    printf("잘못된 선택이거나 취소를 선택하셨습니다. 아무 키나 누르면 계속합니다...");
                    setTextColor(7);
                    dummy = getchar();
                    break;
                }
                setTextColor(14);
                printf("\n1. 수정");
                setTextColor(12);
                printf("\n2. 삭제");
                setTextColor(7);
                printf("\n선택: ");
                scanf("%d", &action);
                dummy = getchar();
                if (action == 1) {
                    // 수정 기능
                    setTextColor(2);
                    printf("\n새 금액을 입력해주세요: ");
                    scanf("%d", &expenseRecords[expenseIndex - 1].amount);
                    dummy = getchar();
                    setTextColor(6);
                    printf("새 출처를 입력해주세요: ");
                    fgets(expenseRecords[expenseIndex - 1].description, sizeof(expenseRecords[expenseIndex - 1].description), stdin);
                    expenseRecords[expenseIndex - 1].description[strcspn(expenseRecords[expenseIndex - 1].description, "\n")] = '\0';
                    setTextColor(7);
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
                    setTextColor(4);
                    printf("잘못된 선택입니다. 아무 키나 누르면 계속합니다...");
                    setTextColor(7);
                    dummy = getchar();
                }
                break;
            default:
                setTextColor(4);
                printf("잘못된 선택입니다. 다시 시도해주세요.\n");
                printf("\n아무 키나 누르면 계속합니다...");
                setTextColor(7);
                dummy = getchar();
                break;
            }
        }
    }
    else {
        setTextColor(12);
        printf("가계부 모듈 실행이 취소되었습니다.\n");
        setTextColor(7);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
    }
}

// ToDolist 데이터 로드 함수
void loadTodoListFromFile(const char* filename, Todo todos[], int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        setTextColor(4);
        printf("저장된 ToDolist 파일이 없거나 파일을 열 수 없습니다.\n");
        setTextColor(7);
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
        setTextColor(4);
        fprintf(stderr, "ToDolist 파일 저장 실패\n");
        setTextColor(7);
        return;
    }

    // 파일의 첫 줄에 데이터 개수를 기록
    fprintf(file, "%d\n", count);

    // 데이터 저장
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s\n0\n", todos[i].date, todos[i].type, todos[i].task);
    }

    fclose(file);
    setTextColor(4);
    printf("ToDolist 파일 저장 완료.\n");
    setTextColor(7);
}

// ToDolist 출력 함수
void printTodoList(Todo todos[], int count) {
    setTextColor(13);
    printf("\n========== 할 일 목록 ==========\n");
    for (int i = 0; i < count; i++) {
        printf("%d. 날짜: %s, 타입: %s, 내용: %s, 상태: %s\n",
            i + 1, todos[i].date, todos[i].type, todos[i].task, STATUS_TEXT[todos[i].status]);
    }
    printf("\n================================\n");
    setTextColor(7);
    printf("\n아무 키나 누르면 계속합니다...");
    dummy = getchar();
}

// ToDolist 항목 추가 함수
void addTodo(Todo todos[], int* count) {
    if (*count >= MAX_TODOS) {
        setTextColor(12);
        printf("할 일 목록이 가득 찼습니다!\n");
        setTextColor(7);
        return;
    }
    Todo newTodo;
    setTextColor(1);
    printf("날짜를 입력해주세요 (예시: 20250101): ");
    fgets(newTodo.date, sizeof(newTodo.date), stdin);
    newTodo.date[strcspn(newTodo.date, "\n")] = '\0';
    setTextColor(13);
    printf("타입을 입력해주세요 (해야할 것): ");
    fgets(newTodo.type, sizeof(newTodo.type), stdin);
    newTodo.type[strcspn(newTodo.type, "\n")] = '\0';
    setTextColor(15);
    printf("내용을 입력해주세요: ");
    fgets(newTodo.task, sizeof(newTodo.task), stdin);
    newTodo.task[strcspn(newTodo.task, "\n")] = '\0';
    setTextColor(6);
    printf("상태를 선택해주세요 (0: 준비, 1: 진행, 2: 완료, 3: 보관): ");
    scanf("%d", &newTodo.status);
    getchar(); // 버퍼 비우기

    if (newTodo.status < 0 || newTodo.status > 3) {
        setTextColor(4);
        printf("잘못된 상태 선택입니다. 기본값(준비)으로 설정합니다.\n");
        newTodo.status = 0; // 기본값: 준비
    }

    todos[(*count)++] = newTodo;
    setTextColor(10);
    printf("새로운 할 일이 추가되었습니다!\n");
    setTextColor(7);
}

// ToDolist 상태 수정 함수
void updateTodoStatus(Todo todos[], int count) {
    if (count == 0) {
        setTextColor(12);
        printf("수정할 할 일이 없습니다.");
        setTextColor(7);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }
    setTextColor(13);
    printf("\n========== 할 일 목록 ==========\n");
    for (int i = 0; i < count; i++) {
        printf("%d. 날짜: %s, 타입: %s, 내용: %s, 상태: %s\n",
            i + 1, todos[i].date, todos[i].type, todos[i].task, STATUS_TEXT[todos[i].status]);
    }
    printf("\n================================\n");
    setTextColor(14);
    printf("상태를 수정할 할 일 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
    setTextColor(7);
    int index;
    scanf("%d", &index);
    getchar(); // 버퍼 비우기
    if (index <= 0 || index > count) {
        setTextColor(12);
        printf("잘못된 선택이거나 취소를 선택하셨습니다.\n");
        setTextColor(7);
        return;
    }
    index--; // 배열 인덱스 보정
    setTextColor(10);
    printf("새로운 상태를 입력해주세요 (0: 준비, 1: 진행, 2: 완료, 3: 보관): ");
    setTextColor(7);
    int newStatus;
    scanf("%d", &newStatus);
    getchar(); // 버퍼 비우기
    if (newStatus < 0 || newStatus > 3) {
        setTextColor(4);
        printf("잘못된 상태 선택입니다.\n");
        setTextColor(7);
        return;
    }
    todos[index].status = newStatus;
    setTextColor(14);
    printf("상태가 '%s'로 수정되었습니다.\n", STATUS_TEXT[newStatus]);
    setTextColor(7);
}

// ToDolist 항목 수정/삭제 함수
void editTodo(Todo todos[], int* count) {
    if (*count == 0) {
        setTextColor(12);
        printf("수정할 할 일이 없습니다.");
        setTextColor(7);
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
        return;
    }
    setTextColor(13);
    printf("\n========== 할 일 목록 ==========\n");
    for (int i = 0; i < count; i++) {
        printf("%d. 날짜: %s, 타입: %s, 내용: %s, 상태: %s\n",
            i + 1, todos[i].date, todos[i].type, todos[i].task, STATUS_TEXT[todos[i].status]);
    }
    printf("\n================================\n");
    setTextColor(14);
    printf("수정 또는 삭제할 할 일 번호를 입력하세요 (0을 입력하면 취소됩니다): ");
    setTextColor(7);
    int index;
    scanf("%d", &index);
    getchar(); // 버퍼 비우기
    if (index <= 0 || index > *count) {
        setTextColor(4);
        printf("잘못된 선택이거나 취소를 선택하셨습니다.\n");
        setTextColor(7);
        return;
    }
    index--; // 배열 인덱스 보정
    setTextColor(14);
    printf("\n1. 수정");
    setTextColor(12);
    printf("\n2. 삭제");
    setTextColor(7);
    printf("선택: ");
    int action;
    scanf("%d", &action);
    getchar();
    if (action == 1) {
        // 수정 기능
        setTextColor(1);
        printf("새 날짜를 입력해주세요 (예시: 20250101): ");
        fgets(todos[index].date, sizeof(todos[index].date), stdin);
        todos[index].date[strcspn(todos[index].date, "\n")] = '\0';
        setTextColor(13);
        printf("새 타입을 입력해주세요: ");
        fgets(todos[index].type, sizeof(todos[index].type), stdin);
        todos[index].type[strcspn(todos[index].type, "\n")] = '\0';
        setTextColor(15);
        printf("새 내용을 입력해주세요: ");
        fgets(todos[index].task, sizeof(todos[index].task), stdin);
        todos[index].task[strcspn(todos[index].task, "\n")] = '\0';
        setTextColor(6);
        printf("새 상태를 입력해주세요 (0: 준비, 1: 진행, 2: 완료, 3: 보관): ");
        setTextColor(7);
        int newStatus;
        scanf("%d", &newStatus);
        getchar();
        if (newStatus < 0 || newStatus > 3) {
            setTextColor(4);
            printf("잘못된 상태 선택입니다.\n");
            setTextColor(7);
        }
        else {
            todos[index].status = newStatus;
        }
        setTextColor(14);
        printf("할 일이 수정되었습니다.\n");
        setTextColor(7);
    }
    else if (action == 2) {
        // 삭제 기능
        for (int i = index; i < *count - 1; i++) {
            todos[i] = todos[i + 1];
        }
        (*count)--;
        setTextColor(12);
        printf("할 일이 삭제되었습니다.\n");
        setTextColor(7);
    }
    else {
        setTextColor(4);
        printf("잘못된 선택입니다.\n");
        setTextColor(7);
    }
}

// ToDolist 모듈 실행 함수
void runTodolistModule() {
    Todo todos[MAX_TODOS];
    int todoCount = 0;

    // 데이터 삭제 알림 추가
    char choice;
    setTextColor(4);
    printf("ToDolist 모듈 실행 시 현재 저장 파일이 초기화될 수 있습니다. 진행하시겠습니까? (Y/N): ");
    setTextColor(7);
    scanf(" %c", &choice);
    getchar(); // 버퍼 비우기

    if (choice == 'Y' || choice == 'y') {
        // 파일 초기화
        FILE* file = fopen("database.txt", "w");
        if (file == NULL) {
            fprintf(stderr, "파일 초기화 실패\n");
            return;
        }
        fprintf(file, "0\n"); // 초기화된 파일은 데이터 개수 0으로 기록
        fclose(file); // 초기화 후 파일 닫기
        printf("데이터베이스가 초기화되었습니다.\n");
    }
    else {
        printf("ToDolist 모듈 실행이 취소되었습니다.\n");
        return;
    }

    // 초기화 후 데이터 로드
    loadTodoListFromFile("database.txt", todos, &todoCount);

    int choiceMenu;
    while (1) {
        clearScreen();
        setTextColor(11);
        printf("\n========== ToDolist 입력 모듈 ==========\n"
            "1. 할 일 입력\n"
            "2. 할 일 목록\n"
            "3. 상태 수정\n"
            "4. 할 일 정정(수정 및 삭제)\n"
            "5. 종료\n"
            "========================================\n");
        setTextColor(7);
        printf("선택: ");
        scanf("%d", &choiceMenu);
        getchar(); // 버퍼 비우기

        switch (choiceMenu) {
        case 1:
            addTodo(todos, &todoCount);
            saveTodoListToFile("database.txt", todos, todoCount); // 변경 사항 저장
            break;
        case 2:
            printTodoList(todos, todoCount);
            break;
        case 3:
            updateTodoStatus(todos, todoCount);
            saveTodoListToFile("database.txt", todos, todoCount); // 변경 사항 저장
            break;
        case 4:
            editTodo(todos, &todoCount);
            saveTodoListToFile("database.txt", todos, todoCount); // 변경 사항 저장
            break;
        case 5:
            saveTodoListToFile("database.txt", todos, todoCount); // 종료 전에 저장
            printf("ToDolist 모듈을 종료합니다. 모든 변경 사항이 저장되었습니다.\n");
            exit(0);
        default:
            setTextColor(12);
            printf("잘못된 선택입니다. 다시 시도해주세요.\n");
            setTextColor(7);
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
        setTextColor(13);
        printf("\n모듈을 선택하세요 (1, 2, 3, q=종료): ");
        setTextColor(7);
        char moduleChoice;
        scanf(" %c", &moduleChoice);
        clearInputBuffer();

        if (moduleChoice == '1') {
            runAccountingModule(rootList);
            break;
        }
        else if (moduleChoice == '2') {
            runTodolistModule();
            break;
        }
        else if (moduleChoice == '3') {
            runKcalInputModule(); // 칼로리 입력 모듈 실행
            break;
        }
        else if (moduleChoice == 'q' || moduleChoice == 'Q') {
            printf("모듈 선택을 종료합니다.\n");
            break;
        }
        else {
            setTextColor(4); // 빨간색
            printf("잘못된 선택입니다. 다시 시도해주세요.\n");
            setTextColor(7); // 기본 색상
        }
    }
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
    setTextColor(4);
    printf("\n==== 저장된 데이터 ====\n");
    setTextColor(12);
    KcalRecord* temp = head;
    while (temp) {
        printf("%s - %d kcal - %s\n", temp->foodName, temp->calories, temp->date);
        temp = temp->next;
    }
    setTextColor(4);
    printf("========================\n");
    setTextColor(7);
    printf("\n아무 키나 누르면 계속합니다...\n");
    getchar(); // 대기
}

void handleInvalidInput() {
    setTextColor(4);
    fprintf(stderr, "잘못된 입력입니다. 다시 시도해주세요.\n");
    setTextColor(7);
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

    fprintf(file, "1\kcal\n");

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
    setTextColor(4);
    printf("칼로리 입력 모듈 실행 시 기존 데이터를 초기화할 수 있습니다. 초기화하시겠습니까? (Y/N): ");
    setTextColor(7);
    scanf(" %c", &resetChoice);
    while (getchar() != '\n'); // 입력 버퍼 비우기

    if (resetChoice == 'Y' || resetChoice == 'y') {
        printf("기존 데이터가 초기화되었습니다. 새로운 데이터를 입력하세요.\n");
        freeMemory(); // 기존 연결리스트 초기화
    }
    else if (resetChoice == 'N' || resetChoice == 'n') {
        printf("선택을 취소하고 초기 화면으로 돌아갑니다.\n");
        return;
    }
    else {
        setTextColor(4);
        fprintf(stderr, "잘못된 선택입니다. 다시 시도해주세요.\n");
        setTextColor(7);
        return;
    }

    while (1) {
        clearScreen();
        setTextColor(11);
        printf("\n==== 칼로리 입력 모듈 ====\n");
        printf("1. 데이터 입력\n");
        printf("2. 입력 데이터 출력\n");
        printf("3. 데이터 삭제\n");
        printf("4. 프로그램 종료\n");
        printf("==========================\n");
        setTextColor(7);

        int choice;
        printf("\n선택: ");
        if (scanf("%d", &choice) != 1) {
            setTextColor(4);
            fprintf(stderr, "잘못된 입력입니다. 다시 시도해주세요.\n");
            setTextColor(7);
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }
        while (getchar() != '\n'); // 입력 버퍼 비우기

        if (choice == 4) {
            saveToFile(); // 종료 시 데이터 파일에 저장
            freeMemory(); // 메모리 해제
            printf("프로그램을 종료합니다.\n");
            exit(0); // 프로그램 종료
        }
        else if (choice == 1) {
            char foodName[MAX_NAME_LEN];
            int calories;
            char date[MAX_DATE_LEN];
            setTextColor(6);
            printf("음식 이름: ");
            setTextColor(7);
            fgets(foodName, MAX_NAME_LEN, stdin);
            foodName[strcspn(foodName, "\n")] = '\0'; // 개행 문자 제거
            setTextColor(12);
            printf("칼로리: ");
            setTextColor(7);
            if (scanf("%d", &calories) != 1 || calories < 0) {
                setTextColor(4);
                fprintf(stderr, "잘못된 입력입니다. 다시 시도해주세요.\n");
                setTextColor(7);
                while (getchar() != '\n'); // 입력 버퍼 비우기
                continue;
            }
            while (getchar() != '\n'); // 입력 버퍼 비우기
            setTextColor(15);
            printf("날짜 (YYYYMMDD): ");
            setTextColor(7);
            fgets(date, MAX_DATE_LEN, stdin);
            date[strcspn(date, "\n")] = '\0'; // 개행 문자 제거
            if (!isValidDate(date)) {
                setTextColor(4);
                fprintf(stderr, "잘못된 날짜 형식입니다. 다시 입력해주세요.\n");
                setTextColor(7);
                continue;
            }

            addKcalRecord(foodName, calories, date); // 연결리스트에 추가
            printf("입력된 데이터: %s - %d kcal - %s\n", foodName, calories, date);
        }
        else if (choice == 2) {
            printKcalData(); // 저장된 데이터를 출력
        }
        else if (choice == 3) {
            char foodName[MAX_NAME_LEN];
            char date[MAX_DATE_LEN];
            setTextColor(6);
            printf("(주의: 같은 이름과 날짜를 지닌 음식이 있다면 먼저 작성된 요소가 삭제됩니다.)\n\n");
            setTextColor(12);
            printf("삭제할 데이터의 음식 이름: ");
            setTextColor(7);
            fgets(foodName, MAX_NAME_LEN, stdin);
            foodName[strcspn(foodName, "\n")] = '\0'; // 개행 문자 제거
            setTextColor(12);
            printf("삭제할 데이터의 날짜 (YYYYMMDD): ");
            setTextColor(7);
            fgets(date, MAX_DATE_LEN, stdin);
            date[strcspn(date, "\n")] = '\0'; // 개행 문자 제거

            if (deleteKcalRecord(foodName, date)) {
                printf("데이터가 성공적으로 삭제되었습니다.\n");
            }
            else {
                printf("삭제할 데이터를 찾을 수 없습니다.\n");
            }
        }
        else {\
        setTextColor(4);
            fprintf(stderr, "잘못된 선택입니다. 다시 입력해주세요.\n");
            setTextColor(7);
        }
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
