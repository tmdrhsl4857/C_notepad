#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define MAX_RECORDS 100
#define NAME_LENGTH 100

typedef struct Record {
    char type; // '+' for income, '-' for expense
    int amount;
    char description[NAME_LENGTH];
    char date[NAME_LENGTH];
    struct Record* next;
} Record;

//글자 색, 배경 색 추가 함수
void setTextColor(int textColor, int backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = (backgroundColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}

//함수 원형 선언 모음
void clearScreen();
void freeRecords(Record* head);
Record* loadRecordsFromFile(const char* filename);
int calculateBalance(Record* head);
void printAllExpenses(Record* head);
void printMonthlyDetails(Record* head, int month);
void printMainMenu(int balance);
void printIncomeDetails(Record* head);void printBarGraph(int data[], int size);
void printAllIncomes(Record* head);
void printMonthlyIncomes(Record* head, int month);

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
            setTextColor(14,0);
            printf("\n아무 키나 누르면 상위 메뉴로 돌아갑니다...");
            setTextColor(7,0);
            getchar(); // 키 입력 대기
            break;
        case 3:
            printf("\n프로그램을 종료합니다.\n");
            freeRecords(records);
            exit(0);
        default:
        setTextColor(12,0);
            printf("\n잘못된 선택입니다. 다시 시도해주세요.\n");
            setTextColor(7,0);
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

    // 월별 지출 데이터 초기화
    int monthlyExpenses[12] = { 0 };
    // 월별 지출 데이터를 계산
    while (current != NULL) {
        if (current->type == '-') {
            int month;
            sscanf(current->date, "%*4d%2d", &month);
            if (month >= 1 && month <= 12) {
                monthlyExpenses[month - 1] += current->amount;
            }
        }
        current = current->next;
    }
    // 텍스트 기반 그래프 출력
    printBarGraph(monthlyExpenses, 12);
    setTextColor(4,0);
    printf("\n========전체 지출 내역========\n");
    setTextColor(7,0);
    current = head;
    while (current != NULL) {
        if (current->type == '-') {
            hasExpenses = 1;
            setTextColor(12,0);
            printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
            setTextColor(7,0);
        }
        current = current->next;
    }
    if (!hasExpenses) {
        setTextColor(12,0);
        printf("지출 내역이 없습니다.\n");
        setTextColor(7,0);
    }
     else {
        int month;
        setTextColor(14,0);
        printf("\n특정 월의 지출 내역을 보시겠습니까? (1-12, 0: 취소): ");
        setTextColor(7,0);
        scanf("%d", &month);
        getchar(); // 버퍼 비우기
        if (month >= 1 && month <= 12) {
            printMonthlyDetails(head, month);
            return;  // 월별 내역 출력 후 상위 메뉴로 돌아감
        }
    }
    // 상위 메뉴 복귀
    printf("\n아무 키나 누르면 상위 메뉴로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
}

// 특정 월의 지출 내역 출력 함수
void printMonthlyDetails(Record* head, int month) {
    clearScreen();
    Record* current = head;
    int hasExpenses = 0;

    setTextColor(4,0);
    printf("\n========%d월의 지출 내역========\n", month);
    setTextColor(7,0);
    while (current != NULL) {
        if (current->type == '-') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                hasExpenses = 1;
                setTextColor(12,0);
                printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
                setTextColor(7,0);
            }
        }
        current = current->next;
    }
    if (!hasExpenses) {
        setTextColor(12,0);
        printf("%d월의 지출 내역이 없습니다.\n", month);
        setTextColor(7,0);
    }
    // 상위 메뉴로 복귀
    printf("\n아무 키나 누르면 전체 지출 내역 화면으로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
    printAllExpenses(head); // 전체 지출 내역 화면으로 복귀
}
void printMainMenu(int balance) {
    clearScreen();
    setTextColor(11,0);
    printf("===== 메뉴 =====\n");
    printf("현재 잔액: %d원\n\n", balance);
    printf("1. 지출 내역 확인\n");
    printf("2. 수익 내역 확인\n");
    printf("3. 프로그램 종료\n");
    printf("================\n");
    setTextColor(7,0);
    printf("선택: ");
}

// 수익 내역 출력 함수
void printIncomeDetails(Record* head) {
     printAllIncomes(head);
}
void printBarGraph(int data[], int size) {
    printf("\n=== 월별 지출 그래프 ===\n");
    printf("그래프 단위: 10000원\n\n");
    for (int i = 0; i < size; i++) {
        printf("%2d월: ", i + 1);
        for (int j = 0; j < data[i] / 10000; j++) { // 만원 단위로 스케일링
            printf("▒▒");
        }
        printf(" (%d원)\n", data[i]);
    }
}
void printAllIncomes(Record* head) {
    clearScreen();
    Record* current = head;
    int hasIncome = 0;

// 월별 수익 데이터 초기화
    int monthlyIncomes[12] = { 0 };
    // 월별 수익 데이터를 계산
    while (current != NULL) {
        if (current->type == '+') {
            int month;
            sscanf(current->date, "%*4d%2d", &month);
            if (month >= 1 && month <= 12) {
                monthlyIncomes[month - 1] += current->amount;
            }
        }
        current = current->next;
    }
    // 텍스트 기반 그래프 출력
    printf("\n=== 월별 수익 그래프 ===\n");
    printf("그래프 단위: 10000원\n\n");
    for (int i = 0; i < 12; i++) {
        printf("%2d월: ", i + 1);
        for (int j = 0; j < monthlyIncomes[i] / 10000; j++) { // 만원 단위로 스케일링
            printf("▒▒");
        }
        printf(" (%d원)\n", monthlyIncomes[i]);
    }
    setTextColor(10,0);
    printf("\n========전체 수익 내역========\n");
    current = head;

    while (current != NULL) {
        if (current->type == '+') {
            hasIncome = 1;
            setTextColor(2,0);
            printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
            setTextColor(7,0);
        }
        current = current->next;
    }
    if (!hasIncome) {
        printf("수익 내역이 없습니다.\n");
    }
    else {
        int month;
        setTextColor(14,0);
        printf("\n특정 월의 수익 내역을 보시겠습니까? (1-12, 0: 취소): ");
        setTextColor(7,0);
        scanf("%d", &month);
        getchar(); // 버퍼 비우기
        if (month == 0) {
            // 취소 시 상위 메뉴로 바로 복귀
            return;
        }
        if (month >= 1 && month <= 12) {
            printMonthlyIncomes(head, month);
            return; // 월별 내역 출력 후 상위 메뉴로 돌아감
        }
    }
    // 상위 메뉴 복귀
    printf("\n아무 키나 누르면 상위 메뉴로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
}
void printMonthlyIncomes(Record* head, int month) {
    clearScreen();
    Record* current = head;
    int hasIncome = 0;
    setTextColor(10,0);
    printf("\n========%d월의 수익 내역========\n", month);
    setTextColor(7,0);
    while (current != NULL) {
        if (current->type == '+') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                hasIncome = 1;
                setTextColor(2,0);
                printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
                setTextColor(7,0);
            }
        }
        current = current->next;
    }
    if (!hasIncome) {
        setTextColor(2,0);
        printf("%d월의 수익 내역이 없습니다.\n", month);
        setTextColor(7,0);
    }
    // 상위 메뉴 복귀
    printf("\n아무 키나 누르면 전체 수익 내역 화면으로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
    printAllIncomes(head); // 전체 수익 내역 화면으로 복귀
}

