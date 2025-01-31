#include "users.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#include <ctype.h>
#include "console_setting.h"

user *head=NULL;

int currentUser=0;
// تابع main
//برای دیباگ این فایل

/* int main() {
    system("cls");
    
    SetConsoleOutputCP(CP_UTF8);
    checkAndCreateFile();
    
    loadFromFile(&head);
    mainMenu(&head);
    saveToFile(&head);

    user *current = head;
    while (current != NULL) {
        user *temp = current;
        current = current->right;
        free(temp);
    }
} */

void checkAndCreateFile() {
    //اگر فالی وجود نداشت بسازد
    FILE* pFile = fopen("data_/users.dat", "rb");
    if (pFile == NULL) {
        pFile = fopen("data_/users.dat", "wb");
        if (pFile == NULL) {
            printf("Problem creating user files\n");
            return;
        }
        fclose(pFile);
        /* printf("User file created successfully.\n"); */
    } else {
        fclose(pFile);
    }
}
void saveToFile(user **head) {
    // ذخیره کل لیست پیوندی
    FILE* pFile = fopen("data_/users.dat","wb");
    if(pFile == NULL) {
        printf("Error accessing user file\n");
        return;
    }

    user *p = *head;
    while(p != NULL) {
        fwrite(p, sizeof(user), 1, pFile);
        p = p->right;
    }
    fclose(pFile);
}
void loadFromFile(user **head) {
    //لود همه لیست پیوندی
    user *current = *head;
    while (current != NULL) {
        user *temp = current;
        current = current->right;
        free(temp);
    }
    *head = NULL;

    FILE* pFile = fopen("data_/users.dat", "rb");
    if(pFile == NULL) {
        printf("Error opening user file\n");
        return;
    }

    user temp;
    while(fread(&temp, sizeof(user), 1, pFile)) {
        user *newUser = putUser(head, temp.username, temp.password, temp.email);
        newUser->historyCount = temp.historyCount;
        memcpy(newUser->hisUser, temp.hisUser, sizeof(history) * temp.historyCount);
    }
    fclose(pFile);
}

void mainMenu(user** head) {
    // منوی اصلی
    
    char slc='0';
    
    
    drawTitleBox("Hyper Mario Game", "🍄");
    puts("");
    puts("  0: Exit");
    puts("  1: SignUp");
    puts("  2: SignIn\n");
    printf("Press a Key ");

    while (1) {
       
        if (_kbhit()) {
            slc = getch(); 

          
            if (slc == '0' || slc == '1' || slc == '2') {
                switch (slc) {
                    case '0':
    system("cls");
    printf("Exiting...");
    Sleep(1000);
    system("cls");
    saveToFile(head); // ابتدا داده‌ها را ذخیره میکینم
    user *current = *head;
    while (current != NULL) {
        user *temp = current;
        current = current->right;
        free(temp); // سپس حافظه را آزاد میکنیم
    }
    *head = NULL;
    exit(0);
    break;

                    case '1':
                        system("cls");
                        signUp(head);
                        break;
                        

                    case '2':
                        system("cls");
                        signIn(head);
                        break;
                        
                }
            } else {
                system("cls");
                printf("\nInvalid input! Please try again.\n\n");
                Sleep(1000);
                system("cls");

               
                drawTitleBox("Hyper Mario game", "🍄");
                puts("");
                puts("  0: Exit");
                puts("  1: SignUp");
                puts("  2: SignIn\n");
                printf("Press a Key ");
            }
        }

        Sleep(100);
    }
}
user* createUser(char username[50], char password[50], char email[50]) {
    //اضفاه کردن یک نود جدید
    user *p = (user*)malloc(sizeof(user));
    if(p == NULL) {
        printf("Memory allocation error!\n");
        return NULL;
    }
    strcpy(p->username, username);
    strcpy(p->password, password);
    strcpy(p->email, email);
    p->historyCount = 0; // مقداردهی اولیه تاریخچه
    p->right = NULL;
    return p;
}
void delUser(user **head, user *current) {
    //حذف یک نود
    
    user* temp = *head;

    
    if (*head == current) {
        *head = current->right; 
        free(current);
        saveToFile(head);
        printf("\nDelet Account was successful!\n");
        puts("");
        loadingAnimation(4);
        mainMenu(head);
        return;
    }

   
    while (temp->right != current) {
        temp = temp->right;
        if (temp == NULL) { 
            printf("Node not found in the list! (there is a bug!)\n");
            mainMenu(head);
            return;
        }
    }

    
    temp->right = current->right;
    free(current);
    saveToFile(head);
    printf("\nDelet Account was successful!\n");
    puts("");
    loadingAnimation(4);
    mainMenu(head);
    return;
    
}
user* putUser(user **head, char username[50], char password[50], char email[50]) {
    //اشافه کردن نود جدید به اخر لیست
    user* newUser = createUser(username, password, email);
    if(*head == NULL) {
        *head = newUser;
        return newUser;
    }
    user *p = *head;
    while(p->right != NULL) {
        p = p->right;
    }
    p->right = newUser;
    return newUser;
}
int checkAgain(user** head, char value[50], char *field) {
    //برای بررسی تکراری نبودن
    if (*head == NULL) {
        return 0;
    }
    user *p = *head;
    while (p != NULL) {
        if (strcmp(field, "username") == 0 && strcmp(p->username, value) == 0) {
            return 1;
        }
        if (strcmp(field, "email") == 0 && strcmp(p->email, value) == 0) {
            return 1;
        }
        p = p->right;
    }
    return 0;
}


void signUp(user** head) {
    //ساخت اکانت 
    char username[50];
    char pass[50];
    char pass2[50];
    char email[50];
    history hisUser={0,0,"",0,""};
    
    drawTitleBox("SIGN UP", "📝");
    printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");

    // دریافت نام کاربری
    printf("Enter a username: ");
    scanf("%s", username);
    checkAndReturnToMenu(head, username, "0", mainMenu);

    // بررسی تکراری نبودن نام کاربری
    if (checkAgain(head, username, "username")) {
        puts("\nThis username already exists\n");
        Sleep(1000);
        system("cls");
        signUp(head); // بازگشت به منوی ثبت نام
        return;
    }

    // دریافت رمز عبور
    getPassword(head, pass, mainMenu, " ");

    // تأیید رمز عبور
    confirmPassword(head, pass, pass2, mainMenu);

    // دریافت ایمیل
    printf("Enter your Email: ");
    scanf("%s", email);
    checkAndReturnToMenu(head, email, "0", mainMenu);

    // ایجاد کاربر جدید و ذخیره در لیست
    putUser(head,username,pass,email);


    // ذخیره اطلاعات در فایل
    saveToFile(head);

    system("cls");
    printf("User registered successfully!\n\n");
    loadingAnimation(4);
    system("cls");
    mainMenu(head); // بازگشت به منوی اصلی
    return;
}
void checkAndReturnToMenu(user** head, const char* input, const char* target, void (*mainMenu)(user**)) {
    //تابع برای مدیریت ورودی صفر توسط کاربر
    //برای خروج از منوی فعلی
    if (strcmp(input, target) == 0) {
        
        system("cls");
        while (getchar() != '\n');
        mainMenu(head);
        return;
    }
}

void clearLinesAbove(int n) {
    //پاک کردن خط های کنسول
    //با کد های انسی
    for (int i = 0; i < n; i++) {
        printf("\033[1A"); 
        printf("\033[2K"); 
    }
    printf("\r"); 
    fflush(stdout); 
}

void getPassword(user** head,char* pass, void (*mainMenu)(), char* selecttxt) {
    //تابع گرفتن پسورد 
    while (1) {
        printf("Enter a%spassword (Minimum 8 characters): ", selecttxt);
        scanf("%s", pass);

        checkAndReturnToMenu(head,pass, "0", mainMenu);

        if (strlen(pass) >= 8) {
            
            break;
        }

        
        printf("\nMinimum 8 characters! Try again.\n");
        Sleep(1200);

        clearLinesAbove(3);
        
        
        
    }
}
void confirmPassword(user** head,const char* pass1, char* pass2, void (*mainMenu)()) {
    //تابع وارد کردن دوباره پسورد
    while (1) {
        printf("Enter your password again: ");
        scanf("%s", pass2);
        checkAndReturnToMenu(head,pass2, "0", mainMenu);

        if (strcmp(pass1, pass2) != 0) {
            printf("\nPassword does not match! Try again.\n");
            Sleep(1200);
            clearLinesAbove(3);
        } else {
            break;
        }
    }
}

void loadingAnimation(int duration) {
    //صرفا برای زیباسازی بازی
    printf("Loading");
    for (int i = 0; i < duration; i++) {
        printf(".");
        fflush(stdout); 
        Sleep(500); 
       
    }
    printf("\n");
}
void signIn(user** head){
    //تابع ورود کاربر
    
    char pass[50];
    
    char username[50]="";
    char usernameUPPER[50]="";
    char email[50]="";
    int slc;
    char slcForPass;
    int found=0;
    user *p;
    int i;
    char ch;
    

    drawTitleBox("SIGN IN","🔑");
    
    while(1) {
        
        printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");
        printf("Enter your username: ");
        scanf("%s", username);
        checkAndReturnToMenu(head,username,"0",mainMenu);

        p=*head;
        if(*head==NULL) {
            system("cls");
            printf("\nPlease register first!\n");
            
            Sleep(1200);
            system("cls");
            mainMenu(head);
            return;
        }
        while(p!=NULL) {
            if(strcmp(p->username,username)==0) {
                found=1;
                break;
            }
            p=p->right;
            
        }
        if(!found) {
            printf("\nUser not found!\n");
            Sleep(1000);
            clearLinesAbove(6);
            
            continue;
            
        }
        clearLinesAbove(3);
        
        printf("1: Enter password\n");
        printf("2: Forgot password\n\n");
        printf("Select an option: ");
        if(slcForPass=getch()) {

            switch (slcForPass)
            //بررسی خواسته کاربر برای وارد کردن رمز یا بازیابی رمز عبور
            {
            case '1':
                system("cls");
                strcpy(usernameUPPER,username);
                drawTitleBox("SIGN IN","🔑");
                
                printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");
                 printf("Enter your username: %s\n",username);
                while(1) {
                    i=0;
                    printf("Enter your password: ");
                    while (1) {
                        
                        ch = getch(); 

                        if (ch == '\r' || ch == '\n') { 
                            pass[i] = '\0'; 
                            break;
                        } else if (ch == '\b' && i > 0) {
                            i--;
                            printf("\b \b"); 
                        } else if (ch != '\b' && i < 50 - 1) { 
                            pass[i] = ch;
                            i++;
                            printf("*"); 
                        }
                    }
                    checkAndReturnToMenu(head,pass,"0",mainMenu);

                    if(strcmp(p->password,pass)==0) {
                        system("cls");
                        
                        printf("\nLogin successful! Welcome, %s!\n\n",strupr (usernameUPPER));
                        loadingAnimation(4);
                        
                        userMenu(head,p);
                        return;

                } else {
                    printf("\n\nIncorrect password! Try again.\n");
                    Sleep(1000);
                    clearLinesAbove(3);
                    
                }
                }
                break;

            case '2':
                system("cls");
                drawTitleBox("SIGN IN","🔑");
                forgotPassword(p,head);
                break;
            
            default:
                system("cls");
                printf("\nInvalid choice! Try again.\n");
                Sleep(1000);
                system("cls");
                drawTitleBox("SIGN IN","🔑");
                
                break;
            }

            
    
        }
    }    
}

void forgotPassword(user* p, user** head) {
    //تابع فراموشی رمز عبور و بازیابی با ایمیل
    char email[50];
    char newPass[50];
        printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");
    while (1) {
        
        printf("Enter your Email to recover your password: ");
        scanf("%s", email);
        checkAndReturnToMenu(head,email, "0", signIn);

        if (strcmp(p->email, email) == 0) {
            getPassword(head,newPass, signIn, " New ");
            strcpy(p->password, newPass);
            saveToFile(head);
            printf("\nPassword recovery successful!\n");
            Sleep(1000);
            system("cls");
            
            signIn(head);
            return;
        } else {
            printf("\nEmail does not match! Please try again.\n");
        }
    }
}
void drawTitleBox(const char* title, const char* emoji) {
    //صرفا برای زیبا سازی تایتل های منو ها
    int length = strlen(title);
    int emojiWidth = 2; 
    int boxWidth = length + 4 + 2 * emojiWidth; 

    printf("╔");
    for (int i = 0; i < boxWidth; i++) {
        printf("═");
    }
    printf("╗\n");

    printf("%s  %*s%s%*s  %s\n", emoji, (boxWidth - length - 2 * emojiWidth) / 2, "", title, (boxWidth - length - 2 * emojiWidth + 1) / 2, "", emoji);

    printf("╚");
    for (int i = 0; i < boxWidth; i++) {
        printf("═");
    }
    printf("╝\n");
}

void userMenu(user** head,user *current) {
    //منوی اصلی کاربر
    char slc;
    char slcSure;
    system("cls");
    drawTitleBox("USER MENU", "");
    puts("");
    while (1) {
        GameState game;
        printf("\033[1;32m  Press 'W' to Start Game!\033[0m\n\n");
        puts("  0: LogOut");
        puts("  1: Game History");
        puts("  2: Change User Information\n");

        printf("Press a Key ");

        slc = getch(); 
        //اجرای بازی با فشردن w
        switch (slc) {
            case 'W':
            case 'w':
            
                system("cls");
                printf("Starting Game");
                puts("");
                loadingAnimation(6);
                
                system("cls");
                game.mapSelect=2;
                setConsoleSizeAndLockGAME();
                start_game(current,&game);
                return;

            case '0':
            system("cls");
                puts("\033[31mReally quit?\033[0m");
                puts("\n 0: No");
                puts(" 1: Yes\n");
                printf("Press a Key ");
                slcSure=getch();
                if(slcSure=='1') {
                    
                    printf("\nLogging Out...");
                    Sleep(800);
                    system("cls");
                    mainMenu(head);
                    return ;
                    
                break;
                } else {
                    userMenu(head,current);
                    
                }
                break;
                

            case '1':
                
                
                gameHistory(head, current);
                return;

            case '2':
                system("cls");
            
                changeUserInformation(head,current);
                
                return;

            default:
                system("cls");
                printf("\nInvalid input! Please try again.\n\n");
                Sleep(1000);
                system("cls");
                break;
        }
    }
}

void changeUserInformation(user** head,user *current) {
    //تابع تغییر اطلاعات کاربری
    char slc;
    system("cls");
    drawTitleBox("MANAGE ACCOUNT", "");
    puts("");
    while (1) {
        puts("  0: Previous Menu");
        puts("  1: Change Username");
        puts("  2: Change Password");
        puts("  3: Change Email");
        puts("  4: Delet Account\n");

        printf("Press a Key ");

        slc = getch(); 
        
        switch (slc) {
            case '0':
            
                system("cls");
                
                userMenu(head,current);
                return;

            case '1':
                changeUsername(current,head);;
                return;

            case '2':
                changePssword(current,head);
                
                
                return;

            case '3':
                changeEmail(current,head);
                
                return;
            case '4':
                
                deletUserMenu(current,head);
                
                
                return;

            default:
                system("cls");
                printf("\nInvalid input! Please try again.\n\n");
                Sleep(1000);
                system("cls");
                break;
        }
    }
}
void gameHistory(user **head, user *current) {
    //نشان دادن تاریخچه بازی های کاربر
    //البته با ساختار فعلی game.c فقط برد ها نشان داده میوشد
    //که قابل تغییر است
    char slc;
    system("cls");
    drawTitleBox("YOUR HISTORY", " ");
    printf("\n\033[31mPress '0' to return to previous menu\033[0m\n\n");

    for(int i = 0; i < current->historyCount; i++) {
        printf("   🕹️ Game %d:\n", i+1);
        printf("   💰 Coin= %d\n   ✨ Score= %d\n   🎗️ State= %s\n   💡 Level= %d\n   🕓 Date= %s\n\n", 
              current->hisUser[i].coin,
               current->hisUser[i].score,
              current->hisUser[i].endtime,
              current->hisUser[i].level,
              current->hisUser[i].date);
    }

    if(current->historyCount == 0) {
        printf("No history available.\n");
    }

    slc = getch();
    if(slc == '0') {
        userMenu(head, current);
        return;
    }
    return;
}
void deletUserMenu(user *currentUser, user ** head) {
    //تابع حذف یک کاربر
    system("cls");
    char slcSure;
    drawTitleBox("Delet Account","");
    puts("");
    puts("\033[37mThis will \033[31mdelete everything\033[37m permanently. Are you sure?\033[0m");
                puts("\n 0: No");
                puts(" 1: Yes\n");
                printf("Press a Key ");
                slcSure=getch();
                if(slcSure=='1') {
                    system("cls");
                    delUser(head,currentUser);
                    return ;
                    
                
                } else {
                    changeUserInformation(head,currentUser);
                    
                }
    
    
    
}

void changePssword(user *current,user** head){
    system("cls");
    char newPass[50];
    drawTitleBox("Change Password","");
    printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");

     getPassword(head,newPass, changeUserInformation, " New ");
            strcpy(current->password, newPass);
            saveToFile(head);
            printf("\nChange Password was successful!\n");
            Sleep(1000);
            system("cls");
        
            changeUserInformation(head,current);
            return;
    
}
void changeUsername(user *current, user **head) {
    //تغییرنام کاربری
    system("cls");
    drawTitleBox("Change Username", "");
    printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");
    char newUsername[50];
    while (1) {
        printf("Enter a New Username: ");
        scanf("%s", newUsername);
        checkAndReturnToMenu(head,newUsername,"0",(void (*)(user**))changeUserInformation);

        if (checkAgain(head, newUsername, "username")) {
            puts("\nThis username already exists\n");
            Sleep(1000);
            clearLinesAbove(4);
            continue;
        }

        strcpy(current->username, newUsername);
        saveToFile(head);
        printf("\nChange Username was successful!\n");
        Sleep(1000);
        system("cls");
        changeUserInformation(head,current);
        break;
    }
}

void changeEmail(user *current, user **head) {
    //تغییرایمل
    system("cls");
    drawTitleBox("Change Email", "");
    printf("\n\033[31mEnter '0' to return to previous menu\033[0m\n\n");
    char newEmail[100]; 
    while (1) {
        printf("Enter a New Email: ");
        scanf("%s", newEmail);
        checkAndReturnToMenu(head,newEmail,"0",(void (*)(user**))changeUserInformation);

        
        if (checkAgain(head, newEmail, "email")) {
            puts("\nThis email already exists\n");
            Sleep(1000);
            clearLinesAbove(4);
            continue;
        }

        
        strcpy(current->email, newEmail);
        saveToFile(head); 
        printf("\nChange Email was successful!\n");
        Sleep(1000);
        system("cls");
        changeUserInformation(head, current); 
        break;
    }
}
