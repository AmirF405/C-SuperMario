#ifndef USERS_H
#define USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MaxHistory 100

typedef struct HISTORY{
    int score;
    int coin;
    char endtime[50]; // موفقیت یا باختن؟
    int level;
    char date[50];
    


}history;

typedef struct USER{
    char username[50];
    char password[50];
    char email[50];
    history hisUser[MaxHistory];
    struct USER * right;
    int historyCount;

}user;
extern user *head;

void checkAndCreateFile();
void saveToFile(user **head);
void loadFromFile(user **head);
void mainMenu(user** head);
user* createUser(char username[50], char password[50], char email[50]);
user* putUser(user **head, char username[50], char password[50], char email[50]);
void delUser(user **head, user *current);
void signIn(user** head);
void signUp(user** head);
void userMenu(user** head,user *current);
int checkAgain(user** head, char value[50], char *field);
void getPassword(user** head,char* pass, void (*mainMenu)(), char* selecttxt);
void clearLinesAbove(int n);
void confirmPassword(user** head,const char* pass1, char* pass2, void (*mainMenu)());
void loadingAnimation(int duration);
void forgotPassword(user* p, user **head);
void checkAndReturnToMenu(user** head, const char* input, const char* target, void (*mainMenu)(user**));
void drawTitleBox(const char* title, const char* emoji);
void changeUserInformation(user** head,user *current);
void gameHistory(user**head, user * current);
void deletUserMenu(user *currentUser, user ** head);
void changeUsername(user *current,user** head);
void changePssword(user *current,user** head);
void changeEmail(user *current,user** head);

#endif