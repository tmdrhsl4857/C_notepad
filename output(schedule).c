#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int main() {
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
