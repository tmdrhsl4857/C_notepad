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
}

// 가계부 모듈 실행 함수
void runAccountingModule() {
    char choice;
    printf("가계부 모듈 실행 시 현재 저장 파일이 훼손될 가능성이 있습니다. 진행하시겠습니까? (Y/N): ");
    scanf(" %c", &choice);
    dummy = getchar(); // 버퍼 비우기
    if (choice == 'Y' || choice == 'y') {
        int moduleChoice;
        while (1) {
            clearScreen();
            printf("\n========== 가계부 모듈 =========="
                "\n1. 수익 입력"
                "\n2. 지출 입력"
                "\n3. 상위 메뉴로 돌아가기"
                "\n==============================\n");
            printf("선택: ");
            scanf("%d", &moduleChoice);
            dummy = getchar(); // 버퍼 비우기

            if (moduleChoice == 3) {
                return; // 상위 메뉴로 돌아감
            }

            Record newRecord;
            newRecord.type = (moduleChoice == 1) ? '+' : '-';

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

            // 입력된 정보 출력 (확인용)
            printf("\n입력된 정보를 확인해 주세요:\n");
            printf("금액: %d\n", newRecord.amount);
            printf("출처: %s\n", newRecord.description);
            printf("날짜: %s\n", newRecord.date);
            printf("\n아무 키나 누르면 계속합니다...");
            dummy = getchar();
        }
    }
    else {
        printf("가계부 모듈 실행이 취소되었습니다.\n");
        printf("\n아무 키나 누르면 계속합니다...");
        dummy = getchar();
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
                    runAccountingModule();
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
