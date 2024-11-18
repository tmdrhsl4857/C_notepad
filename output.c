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

void clearScreen() {
    printf("\033[H\033[J"); // ANSI 이스케이프 코드를 사용해 화면을 지움
}

// 데이터베이스 파일에서 항목을 로드하는 함수 (수정된 버전)
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

            // 파일에서 각 레코드를 읽습니다.
            if (fscanf(file, "%d %99s %99s\n", &newRecord->amount, newRecord->description, newRecord->date) != 3) {
                fprintf(stderr, "파일에서 데이터를 읽는 중 오류가 발생했습니다.\n");
                free(newRecord);
                continue;
            }

            newRecord->type = categoryType;  // '+' 또는 '-' 설정
            newRecord->next = NULL;

            if (head == NULL) {
                head = newRecord;
            }
            else {
                current->next = newRecord;
            }
            current = newRecord;

            // 요소 끝 표시를 읽음
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

// 월별 지출 계산 함수
void calculateMonthlyExpenses(Record* head, int monthlyExpenses[12]) {
    Record* current = head;
    while (current != NULL) {
        if (current->type == '-') {
            int month;
            sscanf(current->date, "%*4d%2d", &month); // 날짜에서 월을 추출
            if (month >= 1 && month <= 12) {
                monthlyExpenses[month - 1] += current->amount;
            }
        }
        current = current->next;
    }
}

// 특정 월의 지출 내역 출력 함수
void printMonthlyDetails(Record* head, int month) {
    Record* current = head;
    printf("\n%d월의 지출 내역:\n", month);
    while (current != NULL) {
        if (current->type == '-') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
            }
        }
        current = current->next;
    }
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

int main() {
    const char* filename = "database.txt";
    Record* records = loadRecordsFromFile(filename);

    while (1) {
        int balance = calculateBalance(records);
        printf("\n잔액: %d\n", balance);

        // 월별 지출 계산 및 출력
        int monthlyExpenses[12] = { 0 };
        calculateMonthlyExpenses(records, monthlyExpenses);
        printf("\n===== 월별 사용 금액 =====\n");
        for (int i = 0; i < 12; i++) {
            printf("%d월: %d원\n", i + 1, monthlyExpenses[i]);
        }

        // 사용자가 월별 지출 내역을 보고 싶을 때
        char choice;
        printf("\n월별 지출 내역을 보시겠습니까? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            int month;
            printf("보고 싶은 월을 입력하세요 (1-12): ");
            scanf("%d", &month);
            if (month >= 1 && month <= 12) {
                printMonthlyDetails(records, month);
            }
            else {
                printf("잘못된 월을 입력하셨습니다.\n");
            }
        }

        // 프로그램 종료 여부 확인
        printf("\n프로그램을 종료하시겠습니까? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            break;
        }
        clearScreen();
    }

    freeRecords(records);
    return 0;
}
