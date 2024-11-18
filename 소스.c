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
    printf("\033[H\033[J"); // ANSI �̽������� �ڵ带 ����� ȭ���� ����
}

// �����ͺ��̽� ���Ͽ��� �׸��� �ε��ϴ� �Լ� (������ ����)
Record* loadRecordsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "������ �� �� �����ϴ�.\n");
        exit(1);
    }

    int numCategories;
    if (fscanf(file, "%d\n", &numCategories) != 1) {
        fprintf(stderr, "���Ͽ��� ī�װ� ���� ���� �� �����ϴ�.\n");
        exit(1);
    }

    Record* head = NULL;
    Record* current = NULL;

    for (int i = 0; i < numCategories; i++) {
        char categoryType;
        if (fscanf(file, " %c\n", &categoryType) != 1) {
            fprintf(stderr, "���Ͽ��� ī�װ� Ÿ���� �д� �� ������ �߻��߽��ϴ�.\n");
            exit(1);
        }

        int numRecords;
        if (fscanf(file, "%d\n", &numRecords) != 1) {
            fprintf(stderr, "���Ͽ��� ���ڵ� ���� �д� �� ������ �߻��߽��ϴ�.\n");
            exit(1);
        }

        for (int j = 0; j < numRecords; j++) {
            Record* newRecord = (Record*)malloc(sizeof(Record));
            if (newRecord == NULL) {
                fprintf(stderr, "�޸� �Ҵ翡 �����߽��ϴ�.\n");
                exit(1);
            }

            // ���Ͽ��� �� ���ڵ带 �н��ϴ�.
            if (fscanf(file, "%d %99s %99s\n", &newRecord->amount, newRecord->description, newRecord->date) != 3) {
                fprintf(stderr, "���Ͽ��� �����͸� �д� �� ������ �߻��߽��ϴ�.\n");
                free(newRecord);
                continue;
            }

            newRecord->type = categoryType;  // '+' �Ǵ� '-' ����
            newRecord->next = NULL;

            if (head == NULL) {
                head = newRecord;
            }
            else {
                current->next = newRecord;
            }
            current = newRecord;

            // ��� �� ǥ�ø� ����
            int endMarker;
            if (fscanf(file, "%d\n", &endMarker) != 1 || endMarker != 0) {
                fprintf(stderr, "����� �� ǥ�ø� �д� �� ������ �߻��߽��ϴ�.\n");
                exit(1);
            }
        }
    }

    fclose(file);
    return head;
}

// �ܾ� ��� �Լ�
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

// ���� ���� ��� �Լ�
void calculateMonthlyExpenses(Record* head, int monthlyExpenses[12]) {
    Record* current = head;
    while (current != NULL) {
        if (current->type == '-') {
            int month;
            sscanf(current->date, "%*4d%2d", &month); // ��¥���� ���� ����
            if (month >= 1 && month <= 12) {
                monthlyExpenses[month - 1] += current->amount;
            }
        }
        current = current->next;
    }
}

// Ư�� ���� ���� ���� ��� �Լ�
void printMonthlyDetails(Record* head, int month) {
    Record* current = head;
    printf("\n%d���� ���� ����:\n", month);
    while (current != NULL) {
        if (current->type == '-') {
            int recordMonth;
            sscanf(current->date, "%*4d%2d", &recordMonth);
            if (recordMonth == month) {
                printf("��¥: %s, �ݾ�: %d��, ����: %s\n", current->date, current->amount, current->description);
            }
        }
        current = current->next;
    }
}

// �޸� ���� �Լ�
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
        printf("\n�ܾ�: %d\n", balance);

        // ���� ���� ��� �� ���
        int monthlyExpenses[12] = { 0 };
        calculateMonthlyExpenses(records, monthlyExpenses);
        printf("\n===== ���� ��� �ݾ� =====\n");
        for (int i = 0; i < 12; i++) {
            printf("%d��: %d��\n", i + 1, monthlyExpenses[i]);
        }

        // ����ڰ� ���� ���� ������ ���� ���� ��
        char choice;
        printf("\n���� ���� ������ ���ðڽ��ϱ�? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            int month;
            printf("���� ���� ���� �Է��ϼ��� (1-12): ");
            scanf("%d", &month);
            if (month >= 1 && month <= 12) {
                printMonthlyDetails(records, month);
            }
            else {
                printf("�߸��� ���� �Է��ϼ̽��ϴ�.\n");
            }
        }

        // ���α׷� ���� ���� Ȯ��
        printf("\n���α׷��� �����Ͻðڽ��ϱ�? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            break;
        }
        clearScreen();
    }

    freeRecords(records);
    return 0;
}
