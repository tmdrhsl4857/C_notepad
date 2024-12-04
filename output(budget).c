#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100
#define NAME_LENGTH 100

typedef struct Record {
    char type; // '+' for income, '-' for expense
    int amount;
    char description[NAME_LENGTH];
    char date[NAME_LENGTH];
    struct Record* next;
} Record;

//함수 원형 선언 모음
void clearScreen();
void freeRecords(Record* head);
Record* loadRecordsFromFile(const char* filename);
int calculateBalance(Record* head);
void printAllExpenses(Record* head);
void printMonthlyDetails(Record* head, int month);
void printMainMenu(int balance);
void printIncomeDetails(Record* head);

int main() {
    const char* filename = "database.txt";
    Record* records = loadRecordsFromFile(filename);

    while (1) {
        int balance = calculateBalance(records); // 잔액 계산
        printMainMenu(balance); // 잔액과 함께 메뉴 출력

        int choice;
        scanf("%d", &choice);
        getchar(); // 버퍼 비우기

        switch (choice) {
        case 1:
            printAllExpenses(records);
            break;
        case 2:
            printIncomeDetails(records);
            printf("\n아무 키나 누르면 상위 메뉴로 돌아갑니다...");
            getchar(); // 키 입력 대기
            break;
        case 3:
            printf("\n프로그램을 종료합니다.\n");
            freeRecords(records);
            exit(0);
        default:
            printf("\n잘못된 선택입니다. 다시 시도해주세요.\n");
            printf("\n아무 키나 누르면 계속합니다...");
            getchar(); // 키 입력 대기
        }
    }

    return 0;
}

//화면 초기화
void clearScreen() {
    printf("\033[H\033[J"); // ANSI 이스케이프 코드를 사용해 화면을 지움
}

// 메모리 해제 함수
void freeRecords(Record* head) {
    Record* current = head;
    while (current != NULL) {
        Record* temp = current;
        current = current->next;
        free(temp);
    }
}

// 데이터베이스 파일에서 항목을 로드하는 함수
Record* loadRecordsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "파일을 열 수 없습니다.\n");
        exit(1);
    }

    int numCategories;
    if (fscanf(file, "%d\n", &numCategories) != 1) {
        fprintf(stderr, "파일에서 카테고리 수를 읽을 수 없습니다.\n");
        exit(1);
    }

    Record* head = NULL;
    Record* current = NULL;

    for (int i = 0; i < numCategories; i++) {
        char categoryType;
        if (fscanf(file, " %c\n", &categoryType) != 1) {
            fprintf(stderr, "파일에서 카테고리 타입을 읽는 중 오류가 발생했습니다.\n");
            exit(1);
        }

        int numRecords;
        if (fscanf(file, "%d\n", &numRecords) != 1) {
            fprintf(stderr, "파일에서 레코드 수를 읽는 중 오류가 발생했습니다.\n");
            exit(1);
        }

        for (int j = 0; j < numRecords; j++) {
            Record* newRecord = (Record*)malloc(sizeof(Record));
            if (newRecord == NULL) {
                fprintf(stderr, "메모리 할당에 실패했습니다.\n");
                exit(1);
            }

            if (fscanf(file, "%d %99s %99s\n", &newRecord->amount, newRecord->description, newRecord->date) != 3) {
                fprintf(stderr, "파일에서 데이터를 읽는 중 오류가 발생했습니다.\n");
                free(newRecord);
                continue;
            }

            newRecord->type = categoryType;
            newRecord->next = NULL;

            if (head == NULL) {
                head = newRecord;
            }
            else {
                current->next = newRecord;
            }
            current = newRecord;

            int endMarker;
            if (fscanf(file, "%d\n", &endMarker) != 1 || endMarker != 0) {
                fprintf(stderr, "요소의 끝 표시를 읽는 중 오류가 발생했습니다.\n");
                exit(1);
            }
        }
    }

    fclose(file);
    return head;
}

// 잔액 계산 함수
int calculateBalance(Record* head) {
    int balance = 0;
    Record* current = head;
    while (current != NULL) {
        if (current->type == '+') {
            balance += current->amount;
        }
        else if (current->type == '-') {
            balance -= current->amount;
        }
        current = current->next;
    }
    return balance;
}


// 전체 지출 내역 출력 함수
void printAllExpenses(Record* head) {
    clearScreen();
    Record* current = head;
    int hasExpenses = 0;

    printf("\n전체 지출 내역:\n");
    while (current != NULL) {
        if (current->type == '-') {
            hasExpenses = 1;
            printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
        }
        current = current->next;
    }
    if (!hasExpenses) {
        printf("지출 내역이 없습니다.\n");
    }
    else {
        int month;
        printf("\n특정 월의 지출 내역을 보시겠습니까? (1-12, 0: 취소): ");
        scanf("%d", &month);
        getchar(); // 버퍼 비우기
        if (month >= 1 && month <= 12) {
            printMonthlyDetails(head, month); // 월별 내역 출력
            return; // 특정 월 지출 내역 화면에서 전체 지출 내역 화면으로 복귀
        }
    }

    // 상위 메뉴 복귀
    printf("\n아무 키나 누르면 상위 메뉴로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
}

// 월별 지출 내역 출력 함수
void printMonthlyDetails(Record* head, int month) {
    clearScreen();
    Record* current = head;
    int hasExpenses = 0;

    printf("\n%d월의 지출 내역:\n", month);
    while (current != NULL) {
        if (current->type == '-') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                hasExpenses = 1;
                printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
            }
        }
        current = current->next;
    }

    if (!hasExpenses) {
        printf("%d월의 지출 내역이 없습니다.\n", month);
    }

    // 상위 메뉴로 복귀
    printf("\n아무 키나 누르면 전체 지출 내역 화면으로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
    printAllExpenses(head); // 전체 지출 내역 화면으로 복귀
}


void printMainMenu(int balance) {
    clearScreen();
    printf("===== 메뉴 =====\n");
    printf("현재 잔액: %d원\n\n", balance);
    printf("1. 전체 지출 내역 보기\n");
    printf("2. 수익 내역 확인\n");
    printf("3. 프로그램 종료\n");
    printf("선택: ");
}

// 수익 내역 출력 함수
void printIncomeDetails(Record* head) {
    clearScreen();
    Record* current = head;
    int hasIncome = 0;

    printf("\n수익 내역:\n");
    while (current != NULL) {
        if (current->type == '+') {
            hasIncome = 1;
            printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
        }
        current = current->next;
    }
    if (!hasIncome) {
        printf("수익 내역이 없습니다.\n");
    }
}
