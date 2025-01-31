#include "users.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#include <ctype.h>
#include <time.h>
#include "console_setting.h"

int main() {
    setConsoleSizeAndLockLOGIN();

    system("chcp 65001 > nul");
    srand(time(NULL));
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
}