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

    while (1) {
        printf("성별을 입력해주세요 (남자/man 또는 여자/woman) : ");
        char se[6];
        scanf("%s", se);

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

    clearScreen(); // 잘못된 입력 후 화면 지우기

    // 메뉴 출력
    while (1) {
        printf("칼로리 메모장\n");
        printf("\n==== 메뉴 ====\n");
        printf("1. 날짜별 음식 섭취 출력\n");
        printf("2. 음식별 섭취 출력\n");
        printf("3. 종료\n");
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

    // 날짜별로 칼로리 합계를 저장할 변수
    char currentDate[MAX_DATE_LEN] = "";
    int dailyCalories = 0;  // 해당 날짜의 칼로리 합계

    // 테두리 출력 - 음식 정보 테이블
    printf("+------------+------------------+--------------+\n");
    printf("|    날짜    |     음식 이름    |    칼로리    |\n");
    printf("+------------+------------------+--------------+\n");

    // 날짜별 음식 섭취 출력
    for (int i = 0; i < foodCount; i++) {
        // 새로운 날짜가 나오면 날짜를 출력
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            // 이전 날짜의 칼로리 합계를 출력
            if (strlen(currentDate) > 0) {
                // 해당 날짜의 음식 정보는 출력 후, 총합 출력
            }

            // 새로운 날짜로 변경
            strcpy(currentDate, foodRecords[i].date);
            dailyCalories = 0; // 새로운 날짜의 칼로리 합계 초기화
        }

        // 같은 날짜에 여러 음식이 있으면 각각 출력
        printf("| %-10s | %-16s | %-12d |\n", foodRecords[i].date, foodRecords[i].foodName, foodRecords[i].calories);

        // 해당 날짜의 칼로리 합산
        dailyCalories += foodRecords[i].calories;
    }

    // 테두리 끝 - 음식 정보 테이블
    printf("+------------+------------------+--------------+\n");

    // 종합 정보 테이블 출력
    printf("\n==== 종합 정보 ====\n");
    printf("+------------+------------------+--------------+\n");
    printf("|    날짜    |    칼로리        | 권장 칼로리  |\n");
    printf("+------------+------------------+--------------+\n");

    // 날짜별 총합 정보를 출력하고 중복을 방지
    strcpy(currentDate, "");
    dailyCalories = 0;
    for (int i = 0; i < foodCount; i++) {
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            // 이전 날짜의 칼로리 합계를 출력
            if (strlen(currentDate) > 0) {
                // 해당 날짜의 음식 총합을 권장 칼로리와 비교하여 출력
                if (dailyCalories > recommendedCalories) {
                    printf("| %-10s | %-16d | %-12s |\n", currentDate, dailyCalories, "권장 초과");
                }
                else {
                    printf("| %-10s | %-16d | %-12s |\n", currentDate, dailyCalories, "권장 이하");
                }
            }

            // 새로운 날짜로 변경
            strcpy(currentDate, foodRecords[i].date);
            dailyCalories = 0; // 새로운 날짜의 칼로리 합계 초기화
        }

        // 해당 날짜의 칼로리 합산
        dailyCalories += foodRecords[i].calories;
    }

    // 마지막 날짜의 종합 정보 출력
    if (dailyCalories > recommendedCalories) {
        printf("| %-10s | %-16d | %-12s |\n", currentDate, dailyCalories, "권장 초과");
    }
    else {
        printf("| %-10s | %-16d | %-12s |\n", currentDate, dailyCalories, "권장 이하");
    }

    // 테두리 끝 - 종합 정보 테이블
    printf("+------------+------------------+--------------+\n");

    // 가장 많이 섭취한 날과 가장 적게 섭취한 날 출력
    int maxCalories = 0, minCalories = INT_MAX;
    char maxDate[MAX_DATE_LEN], minDate[MAX_DATE_LEN];
    dailyCalories = 0;

    strcpy(maxDate, "");
    strcpy(minDate, "");

    // 다시 한 번 날짜별로 칼로리를 합산하여 가장 많이 섭취한 날과 가장 적게 섭취한 날을 찾기
    for (int i = 0; i < foodCount; i++) {
        // 새로운 날짜가 나오면
        if (strcmp(currentDate, foodRecords[i].date) != 0) {
            // 이전 날짜의 칼로리 합계를 계산
            if (strlen(currentDate) > 0) {
                // 가장 많이 섭취한 날과 가장 적게 섭취한 날을 계산
                if (dailyCalories > maxCalories) {
                    maxCalories = dailyCalories;
                    strcpy(maxDate, currentDate);
                }
                if (dailyCalories > 0 && dailyCalories < minCalories) {  // 0 kcal 제외
                    minCalories = dailyCalories;
                    strcpy(minDate, currentDate);
                }
            }

            // 새로운 날짜로 변경
            strcpy(currentDate, foodRecords[i].date);
            dailyCalories = 0; // 새로운 날짜의 칼로리 합계 초기화
        }

        // 해당 날짜의 칼로리 합산
        dailyCalories += foodRecords[i].calories;
    }

    // 마지막 날짜의 칼로리 합계 계산
    if (dailyCalories > maxCalories) {
        maxCalories = dailyCalories;
        strcpy(maxDate, currentDate);
    }
    if (dailyCalories > 0 && dailyCalories < minCalories) {
        minCalories = dailyCalories;
        strcpy(minDate, currentDate);
    }

    // 가장 많이 섭취한 날과 가장 적게 섭취한 날 출력
    printf("\n==== 분석 결과 ====\n");
    printf("가장 많이 섭취한 날: %s - %d kcal\n", maxDate, maxCalories);
    printf("가장 적게 섭취한 날: %s - %d kcal\n", minDate, minCalories);
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

    printf("\n==== 분석 결과 ====\n");
    // 1. 가장 높은 칼로리 음식 찾기
    int maxCaloriesFood = 0;
    printf("\n가장 높은 칼로리를 섭취한 음식 : ");
    for (int i = 0; i < foodCount; i++) {
        if (foodRecords[i].calories > maxCaloriesFood) {
            maxCaloriesFood = foodRecords[i].calories;
        }
    }

    // 가장 높은 칼로리를 섭취한 모든 음식 출력
    for (int i = 0; i < foodCount; i++) {
        if (foodRecords[i].calories == maxCaloriesFood) {
            printf("%s - %d kcal\n", foodRecords[i].foodName, foodRecords[i].calories);
        }
    }

    printf("\n");

    // 2. 자주 먹은 음식 찾기
    int maxCount = 0;
    char frequentFoods[MAX_FOODS][MAX_NAME_LEN];
    int foodCounts[MAX_FOODS] = { 0 };
    int frequentFoodCount = 0;

    for (int i = 0; i < foodCount; i++) {
        int count = 0;
        for (int j = 0; j < foodCount; j++) {
            if (strcmp(foodRecords[i].foodName, foodRecords[j].foodName) == 0) {
                count++;
            }
        }
        if (count > maxCount) {
            maxCount = count;
            frequentFoodCount = 0;
            strcpy(frequentFoods[frequentFoodCount], foodRecords[i].foodName);
            foodCounts[frequentFoodCount] = count;
            frequentFoodCount++;
        }
        else if (count == maxCount) {
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

    printf("자주 먹은 음식:\n");
    for (int i = 0; i < frequentFoodCount; i++) {
        printf("%s - %d번\n", frequentFoods[i], foodCounts[i]);
    }
}


void clearScreen() {
    printf("\n출력을 확인하려면 아무 키나 누르세요...\n");
    getchar();  // 입력 대기
    system("clear || cls");  // Linux/Mac에서는 'clear', Windows에서는 'cls'
}
