#define _CRT_NO_SECURE_WARNINGS_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// 함수 선언
void loadFromFile();
void sortByDateAndCalories();
void sortByFoodAndDate();
void displayByDate();
void displayByFood();
void clearScreen();
void displayAnalysis();

int main() {
    int choice;

    // 데이터 로드
    loadFromFile();

    // 메뉴 출력
    while (1) {
        printf("칼로리 메모장\n");
        printf("\n==== 메뉴 ====\n");
        printf("1. 날짜별 음식 섭취 출력\n");
        printf("2. 음식별 섭취 출력\n");
        printf("3. 분석 결과 출력\n");
        printf("4. 종료\n");
        printf("선택 : ");
        if (scanf("%d", &choice) != 1) {
            printf("잘못된 입력입니다. 다시 시도하세요.\n");
            while (getchar() != '\n'); // 잘못된 입력 처리
            clearScreen(); // 잘못된 입력 후 화면 지우기
            continue;
        }

        while (getchar() != '\n');  // 버퍼 비우기

        switch (choice) {
        case 1:
            sortByDateAndCalories();
            displayByDate();
            clearScreen();
            break;
        case 2:
            sortByFoodAndDate();
            displayByFood();
            clearScreen();
            break;
        case 3:
            displayAnalysis();  // 분석 결과 출력
            clearScreen();
            break;
        case 4:
            printf("\n프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 입력입니다. 다시 선택하세요.\n");
            clearScreen(); // 잘못된 입력 후 화면 지우기
        }
    }

    return 0;
}

// 파일에서 데이터 읽기
void loadFromFile() {
    FILE* file = fopen("database.txt", "r");  // 파일 이름 고정
    if (!file) {
        printf("파일을 열 수 없습니다: database.txt\n");
        exit(1);
    }

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
        if (strncmp(line, "kcal", 2) != 0  ) {
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

void displayByDate() {
    printf("\n==== 날짜별 음식 섭취 ====\n");
    if (foodCount == 0) {
        printf("기록이 없습니다.\n");
        return;
    }

    char currentDate[MAX_DATE_LEN] = "";
    int firstDateFlag = 1; // 날짜 구분을 위해 플래그 사용

    // 테두리 출력
    printf("+------------+------------------+--------------+\n");
    printf("|    날짜    |     음식 이름    |    칼로리    |\n");
    printf("+------------+------------------+--------------+\n");

    for (int i = 0; i < foodCount; i++) {
        // 새로운 날짜가 나오면 날짜를 출력
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            strcpy(currentDate, foodRecords[i].date);
            if (!firstDateFlag) {
                printf("+------------+------------------+--------------+\n");
            }
            firstDateFlag = 0;
            printf("| %-10s | %-16s | %-12d |\n", currentDate, foodRecords[i].foodName, foodRecords[i].calories);
        }
        else {
            // 같은 날짜의 음식 섭취 출력
            printf("| %-10s | %-16s | %-12d |\n", "", foodRecords[i].foodName, foodRecords[i].calories);
        }
    }

    // 테두리 끝
    printf("+------------+------------------+--------------+\n");
}



void displayByFood() {
    printf("\n==== 음식별 섭취 ====\n");
    if (foodCount == 0) {
        printf("기록이 없습니다.\n");
        return;
    }

    char currentFood[MAX_NAME_LEN] = "";
    int firstFoodFlag = 1; // 음식 구분을 위해 플래그 사용

    // 테두리 출력
    printf("+------------------+------------+--------------+\n");
    printf("|    음식 이름     |    날짜    |    칼로리    |\n");
    printf("+------------------+------------+--------------+\n");

    for (int i = 0; i < foodCount; i++) {
        // 새로운 음식이 나오면 음식 이름을 출력
        if (strcmp(currentFood, foodRecords[i].foodName) != 0) {
            strcpy(currentFood, foodRecords[i].foodName);
            if (!firstFoodFlag) {
                printf("+------------------+------------+--------------+\n");
            }
            firstFoodFlag = 0;
            printf("| %-16s | %-10s | %-12d |\n", currentFood, foodRecords[i].date, foodRecords[i].calories);
        }
        else {
            // 같은 음식의 날짜별 섭취만 출력
            printf("| %-16s | %-10s | %-12d |\n", "", foodRecords[i].date, foodRecords[i].calories);
        }
    }

    // 테두리 끝
    printf("+------------------+------------+--------------+\n");
}


// 분석 결과 출력
void displayAnalysis() {
    if (foodCount == 0) {
        printf("분석할 데이터가 없습니다.\n");
        return;
    }

    // 날짜별로 섭취 음3식 정렬
    sortByDateAndCalories();

    printf("\n==== 섭취 음식 분석 ====\n");

    // 성별 입력
    while (1) {
        printf("성별을 입력해주세요 (남자/man 또는 여자/woman) : ");
        char se[6];  // 문자열 배열로 선언
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
    char frequentFoods[MAX_FOODS][MAX_NAME_LEN];  // 자주 먹은 음식을 저장할 배열
    int foodCounts[MAX_FOODS] = { 0 };  // 각 음식의 등장 횟수를 저장할 배열
    int frequentFoodCount = 0;  // 자주 먹은 음식 개수

    for (int i = 0; i < foodCount; i++) {
        int count = 0;
        for (int j = 0; j < foodCount; j++) {
            if (strcmp(foodRecords[i].foodName, foodRecords[j].foodName) == 0) {
                count++;
            }
        }
        // 자주 먹은 음식 횟수 갱신
        if (count > maxCount) {
            maxCount = count;
            frequentFoodCount = 0;  // 새로운 최대 횟수이면 기존 목록을 초기화
            strcpy(frequentFoods[frequentFoodCount], foodRecords[i].foodName);  // 새로운 음식 추가
            foodCounts[frequentFoodCount] = count;  // 그 음식의 횟수도 기록
            frequentFoodCount++;
        }
        else if (count == maxCount) {
            // 동일 횟수일 경우 그 음식도 추가
            int alreadyExists = 0;
            for (int k = 0; k < frequentFoodCount; k++) {
                if (strcmp(frequentFoods[k], foodRecords[i].foodName) == 0) {
                    alreadyExists = 1;
                    break;
                }
            }
            if (!alreadyExists) {
                strcpy(frequentFoods[frequentFoodCount], foodRecords[i].foodName);
                foodCounts[frequentFoodCount] = count;
                frequentFoodCount++;
            }
        }
    }

    printf("2. 자주 먹은 음식:\n");
    for (int i = 0; i < frequentFoodCount; i++) {
        printf("%s - %d번\n", frequentFoods[i], foodCounts[i]);
    }



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

    // 출력 유지 후 화면 지우기
    getchar();  // 추가 입력 대기 (성별 입력 후 남아 있는 '\n' 제거)
    
}



// 화면 지우기
void clearScreen() {
    printf("\n출력을 확인하려면 아무 키나 누르세요...\n");
    getchar();  // 입력 대기
    system("clear || cls");  // Linux/Mac에서는 'clear', Windows에서는 'cls'
}
