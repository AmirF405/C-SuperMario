#include <stdio.h>
#include <stdlib.h>

// تعریف ساختار تاریخچه کاربر
typedef struct {
    int coin;
    int score;
    char endtime[50];
} history;

// تعریف ساختار کاربر
typedef struct user {
    char username[50];
    char password[50];
    char email[50];
    history hisUser;
    struct user* right;
} user;


void displayFileContents() {
    FILE* pFile = fopen("data_/users.dat", "rb");
    if (pFile == NULL) {
        printf("Problem opening user files\n");
        return;
    }

    user temp;
    printf("\nContents of users.dat:\n");
    printf("-------------------------------------------------\n");
    printf("Username\tPassword\tEmail\t\t\tHistory\n");
    printf("-------------------------------------------------\n");

    while (fread(&temp, sizeof(user), 1, pFile)) {
        printf("%s\t\t%s\t\t%s\t\tCoin: %d, Score: %d, EndTime: %s\n",
               temp.username,
               temp.password,
               temp.email,
               temp.hisUser.coin,
               temp.hisUser.score,
               temp.hisUser.endtime);
    }

    fclose(pFile);
}

// تابع اصلی
int main() {
    // نمایش محتوای فایل
    displayFileContents();
    return 0;
}