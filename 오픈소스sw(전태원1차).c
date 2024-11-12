#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 100
char dummy;  // getchar ��� ���ֱ� �뵵

void clearScreen() {
    printf("\033[H\033[J"); // ANSI �̽������� �ڵ带 ����� ȭ���� ����
}

// �׸� ����ü ����
typedef struct Item {
    char name[NAME_LENGTH];
    struct ItemList* children; // �ڽ� �׸� ����Ʈ
} Item;

// �׸� ����Ʈ ����ü ����
typedef struct ItemList {
    Item** items;
    int size;
    int capacity;
} ItemList;

// ����Ʈ �ʱ�ȭ �Լ�
void initItemList(ItemList* list) {
    list->size = 0;
    list->capacity = 2;  // �ʱ� �뷮
    list->items = (Item**)malloc(list->capacity * sizeof(Item*));
    if (list->items == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        exit(1);
    }
}

// ����Ʈ Ȯ�� �Լ�
void resizeItemList(ItemList* list) {
    list->capacity *= 2;
    list->items = (Item**)realloc(list->items, list->capacity * sizeof(Item*));
    if (list->items == NULL) {
        fprintf(stderr, "�޸� ���Ҵ� ����\n");
        exit(1);
    }
}

// ���ο� �׸� �߰� �Լ�
void addItem(ItemList* list, const char* itemName) {
    if (list->size >= list->capacity) {
        resizeItemList(list);
    }
    Item* newItem = (Item*)malloc(sizeof(Item));
    if (newItem == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        exit(1);
    }
    strcpy(newItem->name, itemName);
    newItem->children = (ItemList*)malloc(sizeof(ItemList));
    if (newItem->children == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        exit(1);
    }
    initItemList(newItem->children);
    list->items[list->size++] = newItem;
    printf("�׸� '%s' �߰� �Ϸ�!\n", itemName);
}

// �׸� ��� ��� �Լ�
void printItemList(const ItemList* list) {
    printf("\n�׸� ���:\n");
    for (int i = 0; i < list->size; i++) {
        printf("%d. %s\n", i + 1, list->items[i]->name);
    }
}

// �׸� �޸� ���� �Լ�
void freeItem(Item* item) {
    for (int i = 0; i < item->children->size; i++) {
        freeItem(item->children->items[i]);
    }
    free(item->children->items);
    free(item->children);
    free(item);
}

// ����Ʈ �޸� ���� �Լ�
void freeItemList(ItemList* list) {
    for (int i = 0; i < list->size; i++) {
        freeItem(list->items[i]);
    }
    free(list->items);
}

// �׸� ���� �Լ�
void deleteItem(ItemList* list, int index) {
    if (index < 0 || index >= list->size) {
        printf("�߸��� �ε����Դϴ�.\n");
        return;
    }
    freeItem(list->items[index]);
    for (int i = index; i < list->size - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->size--;
    printf("�׸� ���� �Ϸ�!\n");
}

// �׸� �̸� ���� �Լ�
void editItemName(Item* item, const char* newName) {
    strcpy(item->name, newName);
    printf("�׸� �̸��� '%s'�� ����Ǿ����ϴ�.\n", newName);
}

// �����ͺ��̽��� ���Ͽ� �����ϴ� �Լ�
void saveItemListToFile(const ItemList* list, FILE* file) {
    fprintf(file, "%d\n", list->size);  // ���� ����Ʈ ũ�� ����
    for (int i = 0; i < list->size; i++) {
        fprintf(file, "%s\n", list->items[i]->name);  // �׸� �̸� ����
        saveItemListToFile(list->items[i]->children, file);
    }
}

void saveDatabaseToFile(const char* filename, const ItemList* rootList) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "���� ���� ����\n");
        return;
    }
    saveItemListToFile(rootList, file);
    fclose(file);
    printf("�����ͺ��̽��� ���Ͽ� ����Ǿ����ϴ�.\n");
}

// ���Ͽ��� �����ͺ��̽��� �ҷ����� �Լ�
void loadItemListFromFile(ItemList* list, FILE* file) {
    int size;
    if (fscanf(file, "%d\n", &size) != 1) {  // �׸� ���� �о��
        return;
    }
    for (int i = 0; i < size; i++) {
        char itemName[NAME_LENGTH];
        if (fgets(itemName, sizeof(itemName), file) == NULL) {
            return;
        }
        itemName[strcspn(itemName, "\n")] = '\0';  // ���� ���� ����
        addItem(list, itemName);
        loadItemListFromFile(list->items[list->size - 1]->children, file);
    }
}

void loadDatabaseFromFile(const char* filename, ItemList* rootList) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("����� �����ͺ��̽� ������ �����ϴ�.\n");
        return;
    }
    loadItemListFromFile(rootList, file);
    fclose(file);
    printf("�����ͺ��̽��� ���Ͽ��� �ε�Ǿ����ϴ�.\n");
}

void navigateItem(Item* item) {
    int choice;
    char itemName[NAME_LENGTH];

    while (1) {
        clearScreen();
        printf("\n���� ��ġ: %s\n", item->name);
        printf("1. �� �׸� �߰�\n");
        printf("2. �׸� ��� ���\n");
        printf("3. �׸� ����\n");
        printf("4. ���� �޴��� ���ư���\n");
        printf("����: ");
        scanf("%d", &choice);
        dummy = getchar(); // ���� ����

        switch (choice) {
        case 1:
            printf("\n�߰��� �׸� �̸��� �Է��ϼ���: ");
            fgets(itemName, sizeof(itemName), stdin);
            itemName[strcspn(itemName, "\n")] = '\0';  // ���� ���� ����
            addItem(item->children, itemName);
            break;
        case 2:
            printItemList(item->children);
            printf("\n\n�ƹ� Ű�� ������ ����մϴ�...");
            dummy = getchar();
            break;
        case 3:
            if (item->children->size == 0) {
                printf("������ �׸��� �����ϴ�. �ƹ� Ű�� ������ �޴��� ���ư��ϴ�...");
                dummy = getchar();
                break;
            }
            printItemList(item->children);
            printf("\n������ �׸� ��ȣ�� �Է��ϼ��� (0�� �Է��ϸ� ��ҵ˴ϴ�): ");
            int deleteIndex;
            scanf("%d", &deleteIndex);
            dummy = getchar();  // ���� ����
            if (deleteIndex == 0) {
                printf("������ ����߽��ϴ�.\n");
                dummy = getchar();
                break;
            }
            if (deleteIndex > 0 && deleteIndex <= item->children->size) {
                deleteItem(item->children, deleteIndex - 1);
            }
            else {
                printf("�߸��� �����Դϴ�. �ƹ� Ű�� ������ ����մϴ�...");
                dummy = getchar();
            }
            break;
        case 4:
            return;
        default:
            printf("�߸��� �����Դϴ�. �ٽ� �õ����ּ���.\n");
            printf("\n�ƹ� Ű�� ������ ����մϴ�...");
            dummy = getchar();
        }
    }
}

int main() {
    ItemList rootList;
    initItemList(&rootList);

    loadDatabaseFromFile("database.txt", &rootList);  // ���α׷� ���� �� ������ �ε�

    char itemName[NAME_LENGTH];
    int choice;

    while (1) {
        clearScreen();
        printf("\n========== �޴� ==========");
        printf("\n1. �� �׸� �߰�\n");
        printf("2. �׸� ����\n");
        printf("3. �׸� ����\n");
        printf("4. �׸� ����\n");
        printf("5. ����\n");
        printf("==========================\n");
        printf("����: ");
        scanf("%d", &choice);
        dummy = getchar();  // ���� ����

        switch (choice) {
        case 1:
            printf("\n�߰��� �׸� �̸��� �Է��ϼ���: ");
            fgets(itemName, sizeof(itemName), stdin);
            itemName[strcspn(itemName, "\n")] = '\0';  // ���� ���� ����
            addItem(&rootList, itemName);
            break;
        case 2:
            if (rootList.size == 0) {
                printf("�׸��� �����ϴ�. �ƹ� Ű�� ������ �޴��� ���ư��ϴ�...");
                dummy = getchar();
                break;
            }
            printItemList(&rootList);
            printf("\n������ �׸� ��ȣ�� �Է��ϼ��� (0�� �Է��ϸ� ��ҵ˴ϴ�): ");
            int deleteIndex;
            scanf("%d", &deleteIndex);
            dummy = getchar();  // ���� ����
            if (deleteIndex == 0) {
                printf("������ ����߽��ϴ�.\n");
                dummy = getchar();
                break;
            }
            deleteItem(&rootList, deleteIndex - 1);
            break;
        case 3:
            if (rootList.size == 0) {
                printf("�׸��� �����ϴ�. �ƹ� Ű�� ������ �޴��� ���ư��ϴ�...");
                dummy = getchar();
                break;
            }
            printItemList(&rootList);
            printf("\n������ �׸� ��ȣ�� �Է��ϼ��� (0�� �Է��ϸ� ��ҵ˴ϴ�): ");
            int editIndex;
            scanf("%d", &editIndex);
            dummy = getchar();  // ���� ����
            if (editIndex == 0) {
                printf("������ ����߽��ϴ�.\n");
                dummy = getchar();
                break;
            }
            if (editIndex > 0 && editIndex <= rootList.size) {
                printf("���ο� �׸� �̸��� �Է��ϼ���: ");
                fgets(itemName, sizeof(itemName), stdin);
                itemName[strcspn(itemName, "\n")] = '\0';  // ���� ���� ����
                editItemName(rootList.items[editIndex - 1], itemName);
            }
            else {
                printf("�߸��� �����Դϴ�. �ƹ� Ű�� ������ ����մϴ�...");
                dummy = getchar();
            }
            break;
        case 4:
            if (rootList.size == 0) {
                printf("�׸��� �����ϴ�. �ƹ� Ű�� ������ �޴��� ���ư��ϴ�...");
                dummy = getchar();
                break;
            }
            printItemList(&rootList);
            printf("\n�� ��° �׸��� �����Ͻðڽ��ϱ�? (��ȣ �Է�): ");
            int index;
            scanf("%d", &index);
            dummy = getchar();  // ���� ����

            if (index > 0 && index <= rootList.size) {
                navigateItem(rootList.items[index - 1]);
            }
            else {
                printf("�߸��� �����Դϴ�. �ƹ� Ű�� ������ �޴��� ���ư��ϴ�...");
                dummy = getchar();
            }
            break;
        case 5:
            saveDatabaseToFile("database.txt", &rootList);  // ���α׷� ���� �� ������ ����
            printf("���α׷��� �����մϴ�.\n");
            freeItemList(&rootList);
            return 0;
        default:
            printf("�߸��� �����Դϴ�. �ٽ� �õ����ּ���.\n");
            printf("\n�ƹ� Ű�� ������ ����մϴ�...");
            dummy = getchar();
        }
    }

    return 0;
}
