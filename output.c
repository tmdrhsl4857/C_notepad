#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // Windows 환경에서 색상 변경에 필요

#define MAX_RECORDS 100
#define NAME_LENGTH 100
#define _CRT_NO_SECURE_WARNINGS_

#define MAX_FOODS 100  // 최대 음식 수
#define MAX_NAME_LEN 50  // 음식 이름의 최대 길이
#define MAX_DATE_LEN 10  // 날짜의 최대 길이


// 구조체 정의
typedef struct {
    char foodName[MAX_NAME_LEN];  // 음식 이름
    int calories;                 // 칼로리
    char date[MAX_DATE_LEN];      // 날짜
} FoodRecord;

// 전역 변수
FoodRecord foodRecords[MAX_FOODS];
int foodCount = 0;
int recommendedCalories = 0;  // 하루 권장 칼로리

typedef struct Record {
    char type; // '+' for income, '-' for expense
    int amount;
    char description[NAME_LENGTH];
    char date[NAME_LENGTH];
    struct Record* next;
} Record;

#define ANSI_RESET "\033[0m"
#define ANSI_BLUE "\033[1;36m"
#define ANSI_RED "\033[31m"
// 설정값
#define MAX_DAYS 7
#define MAX_PERIODS 10
#define MAX_LENGTH 100
#define FIELD_WIDTH 12  // 각 항목의 고정 너비 (조정 가능)

// 요일 배열
const char* days[] = { "월", "화", "수", "목", "금", "토", "일" };

#define MAX_TASKS 100
#define MAX_STRING_LENGTH 100

// Task structure
typedef struct {
    char date[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char title[MAX_STRING_LENGTH];
    char status[MAX_STRING_LENGTH]; // Task status: "Not Started", "In Progress", "Completed", "Archived"
} Task;

// 함수 선언
void loadFromFile();
void sortByDateAndCalories();
void sortByFoodAndDate();
void displayByDate();
void displayByFood();
void clearScreen();
void displayAnalysis();
void freeRecords(Record* head);
Record* loadRecordsFromFile(const char* filename);
int calculateBalance(Record* head);
void printAllExpenses(Record* head);
void printMonthlyDetails(Record* head, int month);
void printMainMenu(int balance);
void printIncomeDetails(Record* head);
void printBarGraph(int data[], int size);
void printAllIncomes(Record* head);
void printMonthlyIncomes(Record* head, int month);
void clear_screen();
void display_menu();
void save_to_file(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH], const char* filename);
void load_from_file(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH], const char* filename);
void reset_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]);
void display_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]);
void modify_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]); void loadTasksFromFile(Task tasks[], int* taskCount, const char* filename);
void saveTasksToFile(const Task tasks[], int taskCount, const char* filename);
void printTasks(const Task tasks[], int taskCount);
void updateTaskStatus(Task tasks[], int taskCount);
void sortTasks(Task tasks[], int taskCount, int sortBy);
int main_kcal();
int main_budget();
int main_todolist();
int main_schedule();
void setTextColor(int color);
void clearInputBuffer();

int main() {
    printf("이 프로그램은 input.c와 호환되는 전용 출력 프로그램입니다.\n\n");
    printf("아직 개발중인 버전으로 안정성이 낮으며 언제든지 변경될 수 있습니다.\n\n");
    printf("현재를 기준으로 지원되는 모듈은 4가지이며 그 외의 기능을 지원하지 않습니다.\n\n");
    printf("database.txt가 출력 모듈에 알맞지 않는 형태일 때 오류가 발생할 수 있으니 사전에 확인바랍니다.\n\n");
    printf("\n아무 키나 누르면 시작합니다...\n");

    char dum = getchar();
    clearScreen();

    setTextColor(11);
    printf("======================");
    printf("\n현재 지원 가능한 모듈:\n");
    printf("1. 가계부 모듈\n");
    printf("2. ToDolist 모듈\n");
    printf("3. Kcal 모듈\n");
    printf("4. 시간표 모듈\n");
    printf("======================\n");
    setTextColor(7);

    int choice_main;

    printf("선택: ");
    scanf("%d", &choice_main);
    clearInputBuffer(); // 입력이 끝난 후에도 버퍼 비우기

    //clearScreen();
    //printf("Debug: User choice = %d\n", choice_main);

    switch (choice_main) {
    case 1:main_budget(); break;
    case 2:main_todolist(); break;
    case 3:main_kcal(); break;
    case 4:main_schedule(); break;
    }

    return 0;
}

int main_kcal() {
    int choice;

    // 데이터 로드
    loadFromFile();

    // 메뉴 출력
    while (1) {
        clearScreen();
        printf("\n==== 메뉴 ====\n");
        printf("1. 날짜별 음식 섭취 출력\n");
        printf("2. 음식별 섭취 출력\n");
        printf("3. 분석 결과 출력\n");
        printf("4. 종료\n");
        printf("선택: ");

        if (scanf("%d", &choice) != 1) {
            printf("잘못된 입력입니다. 숫자를 입력하세요.\n");
            clearInputBuffer(); // 입력 버퍼 비우기
            continue;
        }
        clearInputBuffer(); // 정상 입력 후에도 버퍼 비우기
        //printf("Debug: User choice = %d\n", choice); // 디버깅 출력

        switch (choice) {
        case 1:
            sortByDateAndCalories();
            displayByDate();
            break;
        case 2:
            sortByFoodAndDate();
            displayByFood();
            break;
        case 3:
            displayAnalysis();
            break;
        case 4:
            printf("프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 선택입니다. 다시 시도하세요.\n");
        }
    }


    return 0;
}

// 파일에서 데이터 읽기
void loadFromFile() {
    FILE* file = fopen("database.txt", "r");
    if (!file) {
        printf("파일을 열 수 없습니다: database.txt\n");
        exit(1);
    }
    printf("Debug: File opened successfully.\n");

    char line[256];

    // 첫 번째 줄에 '1'이 있는지 확인
    if (fgets(line, sizeof(line), file)) {
        if (line[0] != '1') {  // 첫 번째 줄이 '1'이 아니면 잘못된 파일 형식
            printf("잘못된 파일 형식: 첫 번째 줄이 '1'이어야 합니다.\n");
            fclose(file);
            exit(1);
        }
    }

    // 두 번째 줄에 '남자', 'man', '여자', 'woman'이 있는지 확인
    if (fgets(line, sizeof(line), file)) {
        // kcal가 아닌 경우 종료
        if (strncmp(line, "kcal", 2) != 0) {
            printf("잘못된 파일 형식: 두 번째 줄은 'kcal'이어야 합니다.\n");
            fclose(file);
            exit(1);
        }

    }

    // PBU 뒤에 올 숫자 (읽어들일 데이터의 개수) 읽기
    int numData = 0;
    if (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d", &numData) != 1 || numData < 0) {
            printf("잘못된 파일 형식: 데이터 갯수가 잘못되었습니다.\n");
            fclose(file);
            exit(1);
        }
    }

    // 파일에서 나머지 데이터 읽기
    while (fgets(line, sizeof(line), file) && foodCount < numData) {
        if (line[0] == '0') continue;  // '0'이면 데이터를 끝낸 것으로 간주

        if (foodCount >= MAX_FOODS) {  // 배열 초과 방지
            printf("데이터가 너무 많습니다. %d개의 데이터만 로드합니다.\n", MAX_FOODS);
            break;
        }

        FoodRecord food = { "", 0, "" };  // 구조체 초기화
        if (sscanf(line, "%s %d %s", food.foodName, &food.calories, food.date) == 3) {
            foodRecords[foodCount++] = food;
        }
        else {
            printf("잘못된 데이터 형식: %s", line);
        }
    }
    char line_[256];
    while (fgets(line_, sizeof(line_), file)) {
        printf("Debug: Line read: %s", line_); // 읽은 데이터 출력
    }
    fclose(file);
}

// 날짜와 칼로리 기준으로 정렬
void sortByDateAndCalories() {
    for (int i = 0; i < foodCount - 1; i++) {
        for (int j = i + 1; j < foodCount; j++) {
            // 날짜 우선 정렬, 동일 날짜일 경우 칼로리 내림차순 정렬
            if (strcmp(foodRecords[i].date, foodRecords[j].date) > 0 ||
                (strcmp(foodRecords[i].date, foodRecords[j].date) == 0 &&
                    foodRecords[i].calories < foodRecords[j].calories)) {
                FoodRecord temp = foodRecords[i];
                foodRecords[i] = foodRecords[j];
                foodRecords[j] = temp;
            }
        }
    }
}

// 음식 이름과 날짜 기준으로 정렬
void sortByFoodAndDate() {
    for (int i = 0; i < foodCount - 1; i++) {
        for (int j = i + 1; j < foodCount; j++) {
            // 음식 이름 우선 정렬, 동일 음식일 경우 날짜 오름차순 정렬
            if (strcmp(foodRecords[i].foodName, foodRecords[j].foodName) > 0 ||
                (strcmp(foodRecords[i].foodName, foodRecords[j].foodName) == 0 &&
                    strcmp(foodRecords[i].date, foodRecords[j].date) > 0)) {
                FoodRecord temp = foodRecords[i];
                foodRecords[i] = foodRecords[j];
                foodRecords[j] = temp;
            }
        }
    }
}

// 날짜별 음식 섭취 출력
void displayByDate() {
    printf("\n==== 날짜별 음식 섭취 ====\n");
    if (foodCount == 0) {
        printf("기록이 없습니다.\n");
        return;
    }

    char currentDate[MAX_DATE_LEN] = "";

    for (int i = 0; i < foodCount; i++) {
        // 새로운 날짜가 나오면 날짜를 출력
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            strcpy(currentDate, foodRecords[i].date);
            printf("\n%s\n", currentDate);
        }
        // 같은 날짜의 음식 섭취 출력
        printf("%s - %d kcal\n", foodRecords[i].foodName, foodRecords[i].calories);
    }
    printf("\n\n출력을 확인하려면 아무 키나 누르세요...\n");
    getchar();
}

// 음식별 섭취 출력
void displayByFood() {
    printf("\n==== 음식별 섭취 ====\n");
    if (foodCount == 0) {
        printf("기록이 없습니다.\n");
        return;
    }

    char currentFood[MAX_NAME_LEN] = "";

    for (int i = 0; i < foodCount; i++) {
        // 새로운 음식이 나오면 음식 이름을 출력
        if (strcmp(currentFood, foodRecords[i].foodName) != 0) {
            strcpy(currentFood, foodRecords[i].foodName);
            printf("\n%s\n", currentFood);  // 음식 이름 출력
        }
        // 같은 음식의 날짜별 섭취만 출력
        printf("  Date : %s, Kcal : %d\n", foodRecords[i].date, foodRecords[i].calories);
    }
    printf("\n\n출력을 확인하려면 아무 키나 누르세요...\n");
    getchar();
}

// 분석 결과 출력
void displayAnalysis() {
    if (foodCount == 0) {
        printf("분석할 데이터가 없습니다.\n");
        return;
    }

    // 날짜별로 섭취 음식 정렬
    sortByDateAndCalories();

    printf("\n==== 섭취 음식 분석 ====\n");

    // 성별 입력
    while (1) {
        printf("성별을 입력해주세요 (남자/man 또는 여자/woman): ");
        char se[10];  // 문자열 배열로 선언
        scanf("%s", se);  // 문자열 입력받기

        if (strcmp(se, "남자") == 0 || strcmp(se, "man") == 0) {
            recommendedCalories = 2500;
            break;
        }
        else if (strcmp(se, "여자") == 0 || strcmp(se, "woman") == 0) {
            recommendedCalories = 2000;
            break;
        }
        else {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    }

    // 1. 가장 높은 칼로리를 섭취한 음식 찾기
    int maxCalories = 0;
    printf("\n1. 가장 높은 칼로리를 섭취한 음식 : ");
    for (int i = 0; i < foodCount; i++) {
        if (foodRecords[i].calories > maxCalories) {
            maxCalories = foodRecords[i].calories;
        }
    }

    // 가장 높은 칼로리를 섭취한 모든 음식 출력
    for (int i = 0; i < foodCount; i++) {
        if (foodRecords[i].calories == maxCalories) {
            printf("%s - %d kcal\n", foodRecords[i].foodName, foodRecords[i].calories);
        }
    }

    printf("\n");

    // 2. 자주 먹은 음식 찾기
    int maxCount = 0;
    char frequentFood[MAX_NAME_LEN] = "";
    for (int i = 0; i < foodCount; i++) {
        int count = 0;
        for (int j = 0; j < foodCount; j++) {
            if (strcmp(foodRecords[i].foodName, foodRecords[j].foodName) == 0) {
                count++;
            }
        }
        if (count > maxCount) {
            maxCount = count;
            strcpy(frequentFood, foodRecords[i].foodName);
        }
    }
    printf("2. 자주 먹은 음식: %s - %d번\n", frequentFood, maxCount);

    // 3. 입력된 날짜에 대한 하루 권장 칼로리 초과 여부 확인
    printf("\n3. 하루 권장 칼로리 초과 여부 (%dkcal):\n", recommendedCalories);

    // 날짜별로 칼로리 합산 후 초과 여부 출력
    char currentDate[MAX_DATE_LEN] = ""; // 날짜 초기화
    int dailyCalories = 0;  // 날짜별 칼로리 합산 초기화

    // 칼로리 초과 여부를 날짜별로 처리
    for (int i = 0; i < foodCount; i++) {
        // 날짜가 변경되었을 때 (새로운 날짜가 오면)
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            // 이전 날짜의 칼로리 합산 후 결과 출력
            if (strlen(currentDate) > 0) {  // 날짜가 초기값이 아니면 출력
                if (dailyCalories > recommendedCalories) {
                    printf("  날짜: %s, 권장 칼로리 초과 (%d kcal 섭취)\n", currentDate, dailyCalories);
                }
                else {
                    printf("  날짜: %s, 권장 칼로리 이하 (%d kcal 섭취)\n", currentDate, dailyCalories);
                }
            }
            // 날짜 갱신
            strcpy(currentDate, foodRecords[i].date);
            dailyCalories = foodRecords[i].calories;  // 새로운 날짜에서 칼로리 초기화
        }
        else {
            dailyCalories += foodRecords[i].calories;  // 같은 날짜일 경우 칼로리 더하기
        }
    }

    // 마지막 날짜에 대한 결과 출력
    if (dailyCalories > recommendedCalories) {
        printf("  날짜: %s, 권장 칼로리 초과 (%d kcal 섭취)\n", currentDate, dailyCalories);
    }
    else {
        printf("  날짜: %s, 권장 칼로리 이하 (%d kcal 섭취)\n", currentDate, dailyCalories);
    }

    getchar();
    printf("\n\n출력을 확인하려면 아무 키나 누르세요...\n");
    getchar();

}



int main_budget() {
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

    printf("\n전체 지출 내역:\n");
    current = head;
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
            printMonthlyDetails(head, month);
            return; // 월별 내역 출력 후 상위 메뉴로 돌아감
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
    printf("1. 지출 내역 확인\n");
    printf("2. 수익 내역 확인\n");
    printf("3. 프로그램 종료\n");
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
            printf("#");
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
            printf("#");
        }
        printf(" (%d원)\n", monthlyIncomes[i]);
    }

    printf("\n전체 수익 내역:\n");
    current = head;
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
    else {
        int month;
        printf("\n특정 월의 수익 내역을 보시겠습니까? (1-12, 0: 취소): ");
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

    printf("\n%d월의 수익 내역:\n", month);
    while (current != NULL) {
        if (current->type == '+') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                hasIncome = 1;
                printf("날짜: %s, 금액: %d원, 이유: %s\n", current->date, current->amount, current->description);
            }
        }
        current = current->next;
    }

    if (!hasIncome) {
        printf("%d월의 수익 내역이 없습니다.\n", month);
    }

    // 상위 메뉴 복귀
    printf("\n아무 키나 누르면 전체 수익 내역 화면으로 돌아갑니다...");
    getchar(); // 키 입력 대기
    clearScreen(); // 화면 초기화
    printAllIncomes(head); // 전체 수익 내역 화면으로 복귀
}

// 화면 지우기 함수
void clear_screen() {
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // Unix/Linux/MacOS
#endif
}

// 메뉴 출력
void display_menu() {
    printf("\n--- 시간표 모듈 ---\n");
    printf("1. 시간표 추가 및 수정\n");
    printf("2. 시간표 보기\n");
    printf("3. 시간표 초기화\n");
    printf("4. 프로그램 종료\n");
    printf("선택: ");
}

// 파일에 시간표 저장
void save_to_file(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH], const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("파일 저장 오류");
        exit(1);
    }

    int count = 0; // 저장된 수업의 개수 세기
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                count++;
            }
        }
    }

    fprintf(file, "%d\n", count); // 수업의 개수를 파일의 첫 번째 줄에 저장

    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                fprintf(file, "%s %d교시: %s\n", days[i], j + 1, schedule[i][j]);
                fprintf(file, "0\n"); // 수업 뒤에 0 추가
            }
        }
    }

    fclose(file);
}

// 파일에서 시간표 불러오기
void load_from_file(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH], const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return; // 파일이 없으면 초기 상태 유지
    }

    int count;
    fscanf(file, "%d", &count); // 수업의 개수를 읽어들임

    char day[MAX_LENGTH];
    int period;
    char subject[MAX_LENGTH];
    while (fscanf(file, "%s %d교시: %[^\n]", day, &period, subject) == 3) {
        // 요일 매칭
        int day_index = -1;
        for (int i = 0; i < MAX_DAYS; i++) {
            if (strcmp(day, days[i]) == 0) {
                day_index = i;
                break;
            }
        }

        if (day_index != -1 && period >= 1 && period <= MAX_PERIODS) {
            strcpy(schedule[day_index][period - 1], subject);
        }

        // '0'을 읽어드림 (다음 수업을 구분하는 0)
        int temp;
        fscanf(file, "%d", &temp);
    }

    fclose(file);
}

// 시간표 초기화
void reset_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]) {
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int j = 0; j < MAX_PERIODS; j++) {
            schedule[i][j][0] = '\0'; // 배열 초기화
        }
    }
    printf("시간표가 초기화되었습니다.\n");
}

// 시간표 표시
void display_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]) {
    int max_period = 1; // 최소 1교시 출력 보장
    int col_width[MAX_DAYS]; // 각 열의 최대 너비를 저장하는 배열

    // 최대 교시 계산
    for (int i = 0; i < MAX_DAYS; i++) {
        col_width[i] = strlen(days[i]); // 초기값: 요일 길이
        for (int j = 0; j < MAX_PERIODS; j++) {
            if (strlen(schedule[i][j]) > 0) {
                // 현재 열의 최대 너비 업데이트
                if (strlen(schedule[i][j]) > col_width[i]) {
                    col_width[i] = strlen(schedule[i][j]);
                }
                // 최대 교시 갱신
                if (j + 1 > max_period) {
                    max_period = j + 1;
                }
            }
        }
    }

    // 각 열 너비의 합과 테두리 크기를 고려해 교시 열 너비 설정
    int first_col_width = 8; // "1교시" 같은 교시 번호 열의 고정 너비

    // 전체 표 출력 시작
    printf("\n시간표:\n");

    // 상단 구분선
    printf("┌───────────");
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int k = 0; k < col_width[i] + 2; k++) printf("─");
        printf("┬");
    }
    printf("\b┐\n"); // 마지막 "┬" 대신 "┐"

    // 요일 출력 (토요일: 파랑색, 일요일: 빨강색)
    printf("│ %-*s ", first_col_width, " ");
    for (int i = 0; i < MAX_DAYS; i++) {
        if (i == 5) { // 토요일
            printf("│ %s%-*s%s ", ANSI_BLUE, col_width[i], days[i], ANSI_RESET);
        }
        else if (i == 6) { // 일요일
            printf("│ %s%-*s%s ", ANSI_RED, col_width[i], days[i], ANSI_RESET);
        }
        else {
            printf("│ %-*s ", col_width[i], days[i]);
        }
    }
    printf("│\n");

    // 중간 구분선 (요일 아래)
    printf("├───────────");
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int k = 0; k < col_width[i] + 2; k++) printf("─");
        printf("┼");
    }
    printf("\b┤\n");

    // 교시와 수업 출력
    for (int j = 0; j < max_period; j++) {
        // "교시"를 포함한 문자열 출력
        char period_label[16]; // 교시 문자열 생성용
        snprintf(period_label, sizeof(period_label), "%d교시", j + 1); // "1교시", "2교시" 등 생성
        printf("│ %-*s ", first_col_width, period_label); // 교시 번호 출력

        for (int i = 0; i < MAX_DAYS; i++) {
            if (strlen(schedule[i][j]) > 0) {
                printf("│ %-*s ", col_width[i], schedule[i][j]);
            }
            else {
                printf("│ %-*s ", col_width[i], " "); // 빈 칸 출력
            }
        }
        printf("│\n");

        // 교시 구분선 (마지막 행 제외)
        if (j < max_period - 1) {
            printf("├───────────");
            for (int i = 0; i < MAX_DAYS; i++) {
                for (int k = 0; k < col_width[i] + 2; k++) printf("─");
                printf("┼");
            }
            printf("\b┤\n");
        }
    }

    // 하단 구분선
    printf("└───────────");
    for (int i = 0; i < MAX_DAYS; i++) {
        for (int k = 0; k < col_width[i] + 2; k++) printf("─");
        printf("┴");
    }
    printf("\b┘\n"); // 마지막 "┴" 대신 "┘"
}



// 시간표 추가 및 수정
void modify_schedule(char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH]) {
    char input[MAX_LENGTH];
    char day[MAX_LENGTH];
    int period;
    char subject[MAX_LENGTH];

    printf("수정할 시간표를 입력하세요 (예: 월 1 수학): ");
    scanf(" %[^\n]", input);

    // 입력 파싱
    if (sscanf(input, "%s %d %[^\n]", day, &period, subject) != 3) {
        printf("입력 형식이 잘못되었습니다. (예: 월 1 수학)\n");
        return;
    }

    // 요일 매칭
    int day_index = -1;
    for (int i = 0; i < MAX_DAYS; i++) {
        if (strcmp(day, days[i]) == 0) {
            day_index = i;
            break;
        }
    }

    if (day_index == -1) {
        printf("잘못된 요청입니다. (월~일 중 하나를 입력하세요)\n");
        return;
    }

    // 교시 유효성 검사
    if (period < 1 || period > MAX_PERIODS) {
        printf("잘못된 교시입니다. (1~%d교시만 입력 가능)\n", MAX_PERIODS);
        return;
    }

    // 시간표 수정
    strcpy(schedule[day_index][period - 1], subject);
    printf("%s %d교시가 %s로 수정되었습니다.\n", days[day_index], period, subject);
}

// 메인 함수
int main_schedule() {
    char schedule[MAX_DAYS][MAX_PERIODS][MAX_LENGTH] = { 0 };
    int choice;

    // 초기화
    load_from_file(schedule, "database_schedule.txt");

    do {
        clear_screen(); // 이전 화면 지우기
        display_menu();
        scanf("%d", &choice);

        clear_screen(); // 선택 후 화면 지우기

        switch (choice) {
        case 1: // 시간표 추가 및 수정
            modify_schedule(schedule);
            break;
        case 2: // 시간표 보기
            display_schedule(schedule);
            break;
        case 3: // 시간표 초기화
            reset_schedule(schedule);
            break;
        case 4: // 프로그램 종료
            save_to_file(schedule, "database_schedule.txt");
            printf("프로그램을 종료합니다.\n");
            break;
        default:
            printf("잘못된 선택입니다. 다시 입력하세요.\n");
        }

        // 결과를 확인할 시간 제공
        if (choice != 4) {
            printf("\nEnter 키를 눌러 계속하세요...");
            getchar(); // 버퍼 비우기
            getchar(); // 사용자 입력 대기
        }

    } while (choice != 4);

    return 0;
}

int main_todolist() {
    Task tasks[MAX_TASKS];
    int taskCount = 0;
    int choice;

    // Load tasks from file
    loadTasksFromFile(tasks, &taskCount, "database_todolist.txt");

    while (1) {
        printf("\nTo-Do List\n");
        printf("1. 목록 보여주기\n");
        printf("2. 상태 업데이트\n");
        printf("3. 나가기\n");
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
            updateTaskStatus(tasks, taskCount);
            break;
        case 3:
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
        if (strcmp(tasks[i].status, "0") == 0 || (strcmp(tasks[i].status, "준비") != 0 && strcmp(tasks[i].status, "진행") != 0 && strcmp(tasks[i].status, "완료") != 0 && strcmp(tasks[i].status, "보관") != 0)) {
            strcpy(tasks[i].status, "준비");
        }
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

    printf("\n%-15s %-15s %-30s %-15s\n", "날짜", "유형", "내용", "상태");
    printf("-------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < taskCount; i++) {
        printf("%-15s %-15s %-30s %-15s\n", tasks[i].date, tasks[i].type, tasks[i].title, tasks[i].status);
    }
}

void updateTaskStatus(Task tasks[], int taskCount) {
    system("cls"); // Clear the screen (use "cls" for Windows)
    if (taskCount == 0) {
        printf("업데이트 할 목록이 없습니다..\n");
        return;
    }

    printTasks(tasks, taskCount);
    printf("\n상태를 업데이트 하고 싶은 목록의 번호를 입력하세요. (1 to %d): ", taskCount);
    int index;
    scanf("%d", &index);
    getchar(); // Consume the newline character

    if (index < 1 || index > taskCount) {
        printf("잘못된 번호 입니다.\n");
        return;
    }

    printf("\n새로운 상태를 입력하세요 (1: 준비, 2: 진행, 3: 완료, 4: 보관): ");
    int statusChoice;
    scanf("%d", &statusChoice);
    getchar(); // Consume the newline character

    switch (statusChoice) {
    case 1:
        strcpy(tasks[index - 1].status, "준비");
        break;
    case 2:
        strcpy(tasks[index - 1].status, "진행");
        break;
    case 3:
        strcpy(tasks[index - 1].status, "완료");
        break;
    case 4:
        strcpy(tasks[index - 1].status, "보관");
        break;
    default:
        printf("잘못된 선택입니다.\n");
        return;
    }

    printf("상태 업데이트가 성공적으로 되었습니다.\n");
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

void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
