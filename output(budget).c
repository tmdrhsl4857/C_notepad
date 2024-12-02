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

// 메모리 해제 함수
void freeRecords(Record* head) {
    Record* current = head;
    while (current != NULL) {
        Record* temp = current;
        current = current->next;
        free(temp);
    }
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

// 특정 월의 지출 내역을 날짜순으로 정렬하는 함수
void sortRecordsByDate(Record** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    Record* sorted = NULL;
    Record* current = *head;
    while (current != NULL) {
        Record* next = current->next;
        if (sorted == NULL || strcmp(current->date, sorted->date) < 0) {
            current->next = sorted;
            sorted = current;
        }
        else {
            Record* temp = sorted;
            while (temp->next != NULL && strcmp(temp->next->date, current->date) < 0) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    *head = sorted;
}

// 특정 월의 지출 내역 출력 함수
void printMonthlyDetails(Record* head, int month) {
    Record* current = head;
    Record* monthlyHead = NULL;
    Record* monthlyCurrent = NULL;

    while (current != NULL) {
        if (current->type == '-') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                Record* newRecord = (Record*)malloc(sizeof(Record));
                if (newRecord == NULL) {
                    fprintf(stderr, "메모리 할당에 실패했습니다.\n");
                    exit(1);
                }
                *newRecord = *current;
                newRecord->next = NULL;

                if (monthlyHead == NULL) {
                    monthlyHead = newRecord;
                }
                else {
                    monthlyCurrent->next = newRecord;
                }
                monthlyCurrent = newRecord;
            }
        }
        current = current->next;
    }

    sortRecordsByDate(&monthlyHead);

    printf("\n%d월의 지출 내역:\n", month);
    monthlyCurrent = monthlyHead;
    while (monthlyCurrent != NULL) {
        printf("날짜: %s, 금액: %d원, 이유: %s\n", monthlyCurrent->date, monthlyCurrent->amount, monthlyCurrent->description);
        monthlyCurrent = monthlyCurrent->next;
    }

    // 정렬된 월별 기록 메모리 해제
    freeRecords(monthlyHead);
}

void printIncomeDetails(Record* head) {
    Record* current = head;

    printf("\n수익 내역:\n");
    while (current != NULL) {
        if (current->type == '+') {
            printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
        }
        current = current->next;
    }
    printf("\n아무 키나 누르면 계속합니다...");
    dummy = getchar();
}

int main() {
    const char* filename = "database.txt";
    Record* records = loadRecordsFromFile(filename);

    while (1) {
        int balance = calculateBalance(records);
        printf("잔액: %d\n", balance);

        // 월별 지출 계산 및 출력
        int monthlyExpenses[12] = { 0 };
        calculateMonthlyExpenses(records, monthlyExpenses);
        printf("\n===== 월별 사용 금액 =====\n");
        for (int i = 0; i < 12; i++) {
            printf("%d월: %d원\n", i + 1, monthlyExpenses[i]);
        }

        // 사용자 선택지
        char choice;
        printf("\n===== 메뉴 =====\n");
        printf("1. 월별 지출 내역 확인\n");
        printf("2. 수익 내역 확인\n");
        printf("3. 프로그램 종료\n");
        printf("선택: ");
        scanf(" %c", &choice);

        if (choice == '1') {
            int month;
            printf("보고 싶은 월을 입력하세요 (1-12): ");
            scanf("%d", &month);
            if (month >= 1 && month <= 12) {
                printMonthlyDetails(records, month);
            } else {
                printf("잘못된 월을 입력하셨습니다.\n");
            }
        } else if (choice == '2') {
            printIncomeDetails(records);
        } else if (choice == '3') {
            break;
        } else {
            printf("잘못된 선택입니다.\n");
        }

        clearScreen();
    }

    freeRecords(records);
    return 0;
}