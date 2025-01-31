#include "game.h"
#include "users.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mmsystem.h>
#include "console_setting.h"
#pragma comment(lib, "winmm.lib")

DWORD WINAPI playDelayedSound(LPVOID lpParam) {
    Sleep(500); // 500 میلی‌ثانیه تاخیر برای صای خورد شدن بلاک
    PlaySound(TEXT(".\\sounds\\brick.wav"), NULL, SND_ASYNC | SND_FILENAME);
    return 0;
}



static void hide_cursor();
static void gotoxy(int x, int y);
static bool can_move_mario(const GameState *game, int new_x, int new_y);
static bool can_move(const GameState *game, int new_x, int new_y);
static bool can_jump(const GameState *game);

void printmap(char map[Maxmap_ROWS][Maxmap_COLS], GameState *game) {
    //تابع نمایش مپ
    //رنگی کردن ایموجی ها برای نمایش بهتر در ترمینال های قدیمی
    //شمارنده برای برخی المان هایی که داخل فایل مپ هستند و به مکان انها نیاز داشتیم
    int counter_s_block=0;
    int counter_flower=0;
    int counter_coin_block=0;
    int counter_mush_Block=0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (map[i][j] == '#') {
                printf("\033[38;5;208m🧱\033[0m");
            } else if (map[i][j + 1] == '!' && map[i + 1][j + 1] == 'p') {
                printf("█████");
            } else if (map[i][j + 1] == 'p') {
                printf(" ███");
            } else if (map[i][j + 1] == 'P') {
                printf(" \x1b[38;5;34;48;5;34m███\x1b[0m");
            } else if (map[i][j + 1] == '@' && map[i - 1][j + 1] == 'P') {
                printf("\x1b[38;5;34;48;5;34m█████\x1b[0m");
            } else if (map[i][j + 1] == '@' && map[i + 1][j + 1] == 'P') {
                printf("\x1b[38;5;34;48;5;34m█████\x1b[0m");
            }else if (map[i][j + 1] == 'j' && game->mapSelect==2) {
                printf(" \x1b[38;5;226;48;5;226m███\x1b[0m");
            } else if (map[i][j + 1] == '&' && map[i - 1][j + 1] == 'j' && game->mapSelect==2 ) {
                printf("\x1b[38;5;226;48;5;226m█████\x1b[0m");
            } else if (map[i][j + 1] == '&' && map[i + 1][j + 1] == 'j' && game->mapSelect==2 ) {
                printf("\x1b[38;5;226;48;5;226m█████\x1b[0m");
            }
             else if (map[i][j] == ' ') {
                printf("  ");
                if ((map[i][j - 1] == '!' || map[i][j - 1] == '@') || ((map[i][j - 1] == '&' )&& game->mapSelect==2)) {
                    //مجبور به استفاده این حالت!
                    printf("\b");
                }
            } else if (map[i][j] == 'f') {
                printf("\033[38;5;46m🚩\033[0m");
            }  else if (map[i][j] == 'g') {

                game->flower[counter_flower]=(Flower){j,i,true};
                

                printf("  ");
                counter_flower++;
                
            } else if (map[i][j] == 's') {
                game->sblock_Logic[counter_s_block]=(Sblock_Logic){j,i,"",};
                counter_s_block++;
                printf("\033[38;5;196m🟥\033[0m");
                
            } else if (map[i][j] == 'c') {
                
                game->coin_block[counter_coin_block]=(COIN_block_Logic) {j,i,"",(rand() % 5 + 1)};
                printf("\033[38;5;226m🟨\033[0m");
            }
            else if (map[i][j] == 'm') {
                
                game->mushroom_block[counter_mush_Block]=(Mushroom_block_Logic){j,i,true,false,j,i+1,false,false,(rand() % 2 == 0 ? 1 : -1),-1};
                printf("\033[38;5;226m🟨\033[0m");
            }
            else if (map[i][j] == 't' && game->mapSelect==2) {
                
                
                printf("\x1b[38;5;226;48;5;226m██\x1b[0m");
            }
            else if (map[i][j] == '$'&& game->mapSelect==2) {
                
                
                printf(" \x1b[38;5;226;48;5;226m█\x1b[0m");
            
            }
            else if (map[i][j] == 'z' && game->mapSelect==3) {
                
                
                printf("██");
            
            } else if (map[i][j] == 'v' && (game->mapSelect==3 ||game->mapSelect==2)) {
                
                
                printf("  ");
            
            }
        }
        printf("\n");
    }
}
void initializeFlowers(GameState *game) {
    //اماده سازی گل ها
    
    if(game->mapSelect==1) {
        for(int i = 0; i < MAX_Flower; i++) {
            game->flower[i].hideTime = GetTickCount();
            game->flower[i].delayStartTime = 0;
            game->flower[i].randomDelay = rand() % 1000;
        }
    }

    if(game->mapSelect==2) {
        //no flowers!//
        
    }

}
void reset_Coin_block(GameState *game) {
    for(int i=0; i<MAX_CoinBlock; i++) {
        game->coin_block[i].x=0;
        game->coin_block[i].y=0;
        game->coin_block[i].C_block_IsActive=false;
        game->coin_block[i].maxCoin=0;
        
    }

}
void reset_s_block(GameState *game) {
    for(int i=0; i<MAX_S_block; i++) {
        game->sblock_Logic[i].x=0;
        game->sblock_Logic[i].y=0;
        game->sblock_Logic[i].blockS_IsActive=false;
       game->sblock_Logic[i].delayStartTime=0;
        
    }

}  
void initialize_Coin_block(GameState *game) {
    

    for (int i = 0; i < MAX_CoinBlock; i++) {
        
        game->coin_block[i].maxCoin = rand() % 5 + 1; // تعداد سکه‌ها (بین ۱ تا ۵)
        game->coin_block[i].hideTime = GetTickCount(); // زمان شروع
        game->coin_block[i].C_block_IsActive = true; // بلوک در ابتدا فعال است

        /* // چاپ مقدار maxCoin برای دیباگ
        printf("Coin Block %d: maxCoin = %d\n", i, game->coin_block[i].maxCoin); */
    }
}

void initialize_history(GameState* game, user* current) {
    if(current->historyCount < MaxHistory) {
        
        game->currentHistoryIndex = current->historyCount;
        game->hstrGame[game->currentHistoryIndex].score = 0;
        game->hstrGame[game->currentHistoryIndex].coin = 0;
        strcpy(game->hstrGame[game->currentHistoryIndex].endtime, "In Progress");
        
    } else {
        /* printf("History storage full! Cannot create.\n"); */
        game->currentHistoryIndex = -1; // نشانه خطا
    }
}
void initialize_static_enemies (GameState* game) {
    for (int i = 0; i < MAX_staticEnemy; i++) {
       
        game->static_enemies[i].x = 0;
        game->static_enemies[i].y = 0;
        
        
    }

    if(game->mapSelect==1) {

        game->static_enemies[0].x=31;
        game->static_enemies[0].y=12;
        game->static_enemies[0].staticENM_isActive=true;
        game->static_enemies[1].x=35;
        game->static_enemies[1].y=12;
        game->static_enemies[1].staticENM_isActive=true;
        game->static_enemies[2].x=40;
        game->static_enemies[2].y=5;
        game->static_enemies[2].staticENM_isActive=true;
        game->static_enemies[3].x=45;
        game->static_enemies[3].y=12;
        game->static_enemies[3].staticENM_isActive=true;
        //اماده سازی دایناسور ها در مپ اول
    }

    if(game->mapSelect==2) {

        game->static_enemies[0].x=42;
        game->static_enemies[0].y=7;
        game->static_enemies[0].staticENM_isActive=true;
        //اماده سازی دایناسور ها در مپ دوم
    }
    if(game->mapSelect==3) {

        game->static_enemies[0].x=37;
        game->static_enemies[0].y=10;
        game->static_enemies[0].staticENM_isActive=true;
        game->static_enemies[1].x=39;
        game->static_enemies[1].y=9;
        game->static_enemies[1].staticENM_isActive=true;
        //اماده سازی دایناسور ها در مپ سوم
    }
}
void first_initialize_coin(GameState * game) {
    //اماده سازی سکه ها، مرود استافده در تابعgo to map-2-3
    for (int i = 0; i < MAX_coin; i++) {
       
        game->coins[i].x = 0;
        game->coins[i].y = 0;
        game->coins[i].coinIsActive = false;
        game->coins[i].hideTime = 0; 
        
    }

    if(game->mapSelect==1) {
        game->coins[0] = (Coin){7, 8, true, 0};
        game->coins[1] = (Coin){8, 8, true, 0};
        game->coins[2] = (Coin){18, 12, true, 0};
        game->coins[3] = (Coin){19, 12, true, 0};
        game->coins[4] = (Coin){27, 4, true, 0};
        game->coins[5] = (Coin){28, 4, true, 0};

    }

    if(game->mapSelect==2) {

        game->coins[0] = (Coin){3, 9, true, 0};
        game->coins[1] = (Coin){17, 12, true, 0};
        game->coins[2] = (Coin){32, 8, true, 0};
        game->coins[3] = (Coin){53, 12, true, 0};
        

    }
    if(game->mapSelect==3) {

        game->coins[0] = (Coin){35, 11, true, 0};
        game->coins[1] = (Coin){43, 8, true, 0};
        game->coins[2] = (Coin){44, 7, true, 0};
        game->coins[3] = (Coin){44, 12, true, 0};
       

    }

}
void initialize_coin(GameState * game) {
    //اماده سازی سکه ها (اصلی)
    for (int i = 0; i < MAX_coin; i++) {
       
        game->coins[i].x = 0;
        game->coins[i].y = 0;
        
    }

    if(game->mapSelect==1) {
        game->coins[0] = (Coin){7, 8};
        game->coins[1] = (Coin){8, 8};
        game->coins[2] = (Coin){18, 12};
        game->coins[3] = (Coin){19, 12};
        game->coins[4] = (Coin){27, 4};
        game->coins[5] = (Coin){28, 4};

    }

    if(game->mapSelect==2) {

        game->coins[0] = (Coin){35, 11};
        game->coins[1] = (Coin){43, 8};
        game->coins[2] = (Coin){44, 7};
        game->coins[3] = (Coin){44, 12};
        

    }
    if(game->mapSelect==3) {

        game->coins[0] = (Coin){35, 11};
        game->coins[1] = (Coin){43, 8};
        game->coins[2] = (Coin){44, 7};
        game->coins[3] = (Coin){44, 12};
        
    }

}
void first_initialize_normal_enemies(GameState* game) {
    //اماده سازی اولیه خرچنگ ها( استفاده شده در go to map2-3)
    for (int i = 0; i < MAX_normalEnemy; i++) {
       
        game->normal_enemies[i].x = 0;
        game->normal_enemies[i].y = 0;
        game->normal_enemies[i].enemyIsActive = false;
        game->normal_enemies[i].velocity = -1; 
    }

    if(game->mapSelect==1) {
        game->normal_enemies[0]=(EnemyNormal){25,12,-1,true};
    
        game->normal_enemies[1]=(EnemyNormal){58,12,-1,true};
    
        game->normal_enemies[2]=(EnemyNormal){59,12,-1,true};
   

    }
    if(game->mapSelect==2) {

        game->normal_enemies[0]=(EnemyNormal){15,12,-1,true};
    
        game->normal_enemies[1]=(EnemyNormal){42,12,-1,true};
    
        game->normal_enemies[2]=(EnemyNormal){45,10,-1,true};

        

    }
    if(game->mapSelect==3) {

        game->normal_enemies[0]=(EnemyNormal){34,12,-1,true};
    }

}
void initialize_normal_enemies(GameState* game) {
    //اماده سازی اصلی خرچنگ ها

    for (int i = 0; i < MAX_normalEnemy; i++) {
       
        game->normal_enemies[i].x = 0;
        game->normal_enemies[i].y = 0;
        game->normal_enemies[i].velocity = -1; 
    }
    if(game->mapSelect==1) {
        game->normal_enemies[0]=(EnemyNormal){25,12,-1};
    
        game->normal_enemies[1]=(EnemyNormal){58,12,-1};
    
        game->normal_enemies[2]=(EnemyNormal){59,12,-1};
   

    }
    if(game->mapSelect==2) {

        game->normal_enemies[0]=(EnemyNormal){15,12,-1};
    
        game->normal_enemies[1]=(EnemyNormal){42,12,-1};
    
        game->normal_enemies[2]=(EnemyNormal){45,10,-1};


    }
    if(game->mapSelect==3) {

        game->normal_enemies[0]=(EnemyNormal){34,12,-1};
    
    }
    
}
void initialize_shield(GameState * game) {
    if(game->mapSelect==2) {

        game->shield=(Shield){60,10,true};

    }
    
}
void initialize_game(GameState *game, user *currentUser) {
    //اماده سازی اصلی گیم(استفاده شده در شروع بازی)
    
    game->health=5; //جان اولیه
    game->lastEnemyKillTime = 0; //برای بررسی کشته شدن دشمن متحرک(خرچنگ) در 5 ثانیه
    game->scoreMultiplier = 1; // ضریب امتیاز دهی برای دشمنان متحرک
    game->player.shild_and_impact=false; //غیرفعال بودن شیلد در ابتدای بازی
    /* printf("%d",game->mapSelect); */ // برای دیباگ و ساخت مپ های بازی

    initialize_history(game, currentUser);

    if (game->mapSelect == 1) {
        game->player = (Mario){1, 12, 0, false, 0, false, false};
        game->isIn_Map_1 = true;
        game->isIn_Map_2 = false;
        game->isIn_Map_3 = false;
    }
    else if (game->mapSelect == 2) {
        game->player = (Mario){1, 12, 0, false, 0, false, false};
        game->isIn_Map_1 = false;
        game->isIn_Map_2 = true;
        game->isIn_Map_3 = false;
        //ممکن است بی کاربرد باشد
    }
    else if (game->mapSelect == 3) {
        game->player = (Mario){28, 1, 0, false, 0, false, false};
        game->isIn_Map_1 = false;
        game->isIn_Map_2 = false;
        game->isIn_Map_3 = true;
        //ممکن است بی کاربرد باشد
    }
    
    
    game->sblock_Logic[0].blockS_IsActive=true;
    game->sblock_Logic[1].blockS_IsActive=true;
    //فعالسازی اولیه بلاک های شانسی سکه
    
    
    initialize_static_enemies(game);
    
    first_initialize_coin(game);
    initializeFlowers(game); //فعالسازی گل ها
    first_initialize_normal_enemies(game);
    initialize_shield( game);

    load_map(game);

    mciSendString("open \"sounds\\groundMusic.mp3\" type mpegvideo alias grd", NULL, 0, NULL); //اماده سازی اهنگ اصلی
    mciSendString("open \"sounds\\ungroundMusic.mp3\" type mpegvideo alias ugrd", NULL, 0, NULL); //اماده سازی اهنگ زیرزمین

    game->lastEnemyMoveTime = GetTickCount();
    hide_cursor();
    game->visited_map1 = true;
    game->isIn_Map_1=true;
    game->isIn_Map_2=false;
    game->isIn_Map_3=false;
    
    
}
bool check_collision(int mario_x, int mario_y, int enemy_x, int enemy_y) {
    return (mario_x == enemy_x && mario_y == enemy_y); //برای بررسی برخورد ها استفاده شده در برنامه
}
bool has_time_passed(DWORD last_time, DWORD interval) {
    DWORD current_time = GetTickCount();
    return (current_time - last_time >= interval); //تابع فرعی برای بررسی زمان طی شده برای برخی شرط ها
}
static bool can_move_mario(const GameState *game, int new_x, int new_y) {
    //   بررسی حرکت برای حالت عادی  ماریو
    bool canMoveNormal = (
        new_x >= 0 && new_x < COLS &&
        new_y >= 0 && new_y < ROWS &&
        game->map[new_y][new_x] != '#' &&
        game->map[new_y][new_x] != 's' &&
        game->map[new_y][new_x] != 'c' &&
        game->map[new_y][new_x + 1] != 'p' &&
        game->map[new_y][new_x + 1] != '!' &&
        game->map[new_y][new_x - 1] != '!' &&
        game->map[new_y][new_x] != '!' &&
        game->map[new_y][new_x] != 'p' && 
        game->map[new_y][new_x+1] != 'P' &&
        game->map[new_y][new_x] != 'P' && 
        game->map[new_y][new_x+1] != '@' && 
        game->map[new_y][new_x-1] != '@' && 
        game->map[new_y][new_x] != '@' &&
        game->map[new_y][new_x] != 'm'&&
        (game->mapSelect != 3 || game->map[new_y][new_x] != 'z') &&
        (game->mapSelect != 2 || game->map[new_y+1][new_x+1] != 'v') &&
        game->map[new_y][new_x+1] != 'j' &&
        game->map[new_y][new_x] != 'j' && 
        game->map[new_y][new_x+1] != '&' && 
        game->map[new_y][new_x-1] != '&' && 
        game->map[new_y][new_x] != '&' 
        
    );

    // اگر ماریو بزرگ است، موقعیت بالایی هم بررسی شود
    if (game->player.isBig) {
        bool canMoveBig = canMoveNormal && (
            new_y - 1 >= 0 &&
            game->map[new_y - 1][new_x] != '#' &&
            game->map[new_y - 1][new_x] != 's' &&
            game->map[new_y - 1][new_x] != 'c' &&
            game->map[new_y - 1][new_x + 1] != 'p' &&
            game->map[new_y - 1][new_x + 1] != '!' &&
            game->map[new_y - 1][new_x - 1] != '!' &&
            game->map[new_y - 1][new_x] != '!' &&
            game->map[new_y - 1][new_x] != 'p' && 
            game->map[new_y - 1][new_x+1] != 'P' &&
            game->map[new_y - 1][new_x] != 'P' && 
            game->map[new_y - 1][new_x+1] != '@' && 
            game->map[new_y - 1][new_x-1] != '@' && 
            game->map[new_y - 1][new_x] != '@' &&
            game->map[new_y - 1][new_x] != 'm' &&
            (game->mapSelect != 3 || game->map[new_y][new_x] != 'z') &&
            game->map[new_y - 1][new_x+1] != 'j' &&
            game->map[new_y - 1][new_x] != 'j' && 
            game->map[new_y - 1][new_x+1] != '&' && 
            game->map[new_y - 1][new_x-1] != '&' && 
            game->map[new_y - 1][new_x] != '&' 
        );
        return canMoveBig;
    }

    return canMoveNormal;
}
void saveHISTORY(GameState* game, user * currentUser) {
    //ذخیره کل پیشینه بازی طی شده
    int bonusScoreTIME = (game->remainingTime/ 10000) * 100;
    int bonusScoreHEALTH= game->health *1000;
        
    if (game->currentHistoryIndex != -1) {
        game->hstrGame[game->currentHistoryIndex].score += bonusScoreTIME+ bonusScoreHEALTH;
        currentUser->hisUser[currentUser->historyCount] = game->hstrGame[game->currentHistoryIndex];
        strcpy(currentUser->hisUser[currentUser->historyCount].endtime, 
               game->isSucces ? "Victory" : "Game Over");

        if((game->mapSelect != 2 || game->isIn_Map_2) && (game->mapSelect != 3 || game->isIn_Map_3)) {
            game->hstrGame[game->currentHistoryIndex].level= 1;

        } else if(game->isIn_Map_2 || game->mapSelect==2) {
            game->hstrGame[game->currentHistoryIndex].level= 2;

        } else if(game->isIn_Map_3 || game->mapSelect==3) {
            game->hstrGame[game->currentHistoryIndex].level= 2;

        }
        
        time_t t;
        struct tm *tm_info;
        time(&t);
        tm_info = localtime(&t);
        strftime(currentUser->hisUser[currentUser->historyCount].date, sizeof(currentUser->hisUser[currentUser->historyCount].date), "%Y-%m-%d %H:%M:%S", tm_info);
        //ذخیره زمان و تاریخ بازی انجام شده
        
        currentUser->historyCount++;
        game->hstrGame[game->currentHistoryIndex]=(history){0,0,"",2,""};
        
        saveToFile(&head);
        //استفاده از تابع ذخیره، از فایلusers.c
    }
    
}

void updateScore(GameState* game, int scoreDelta, int coinDelta) {
    //تابع فرعی برای امتیاز دهیی یا سکه دادن!
    if (game->currentHistoryIndex < 0 || game->currentHistoryIndex >= MaxHistory) {
        printf("found a bug in storage!");
    return;
}
    if(game->currentHistoryIndex != -1) {
        
        game->hstrGame[game->currentHistoryIndex].score += scoreDelta;
        
       
        game->hstrGame[game->currentHistoryIndex].coin += coinDelta;
        
        
        if(game->hstrGame[game->currentHistoryIndex].score < 0) {
            game->hstrGame[game->currentHistoryIndex].score = 0;
        }
        if(game->hstrGame[game->currentHistoryIndex].coin < 0) {
            game->hstrGame[game->currentHistoryIndex].coin = 0;
        }
    }
}
void end_or_readyfor_roun2(GameState *game, user *currentUser) {
    // اگر برده‌ایم (success) و در map1 بودیم یا map2:
    // - برای map1 رکورد را ذخیره می‌کنیم.
    // - اگر در map2 بردیم، کل بازی تمام است و می‌توانیم رکورد را ذخیره کنیم.

    //نکته مهم
    //در حالت فعلی فقط اگر برده باشیم ، تاریخچه بازی ذخیره میشود!
    //با حذف 4 خط پایین همه حالات بردن یا باختن نیز ذخیره میشود
    if (game->isSucces) {
        
        if ((game->mapSelect == 1 || game->isIn_Map_1) ||
            (game->mapSelect == 2 || game->isIn_Map_2)) {
            saveHISTORY(game, currentUser);
        }
    }

    int c;
    while (1) {
        system("cls");

        // حالت باخت (game->isRunning == false و game->isSucces == false)
        if (!game->isRunning && !game->isSucces) {
            printf("Game Over!\n\n");
            printf("'1': Try again\n");
            printf("'2': Return to User Menu\n");
            printf("'esc': Exit\n");

            c = _getch();
            switch (c) {
                case 27: // Escape
                    exit(0);

                case '1': // Try again
                    system("cls");
                    // اگر باخت در مرحله اول (map1)
                    if (game->mapSelect == 1) {
                        // از نو مقداردهی کل بازی (شروع از map1)
                        initialize_game(game, currentUser);
                        mciSendString("seek grd to start", NULL, 0, NULL);
                        start_game(currentUser, game);
                    }
                    // اگر باخت در مرحله دوم (map2 یا map3)
                    else if (game->mapSelect == 2 || game->mapSelect == 3) {
                        // ریست کل مرحله دوم
                        resetMap2And3(game);
                        game->hstrGame[game->currentHistoryIndex]=(history){0,0,"",2,""};
                        game->player.shielded=false;
                        

                        // حالا وارد map2 شویم (از ابتدا)
                        goTo_map2(game);

                        // اگر ساختار حلقه اجازه می‌دهد، می‌توانیم همانجا بازی را ادامه دهیم
                        // یا یک return بزنیم که به حلقه بازی برگردد.
                    }

                    // دوباره آماده اجرای حلقه بازی شویم
                    game->isRunning = true;
                    return;

                case '2': // User Menu
                    system("cls");
                    mciSendString("close ugrd", NULL, 0, NULL);
                    mciSendString("close grd", NULL, 0, NULL);
                    userMenu(&head, currentUser);
                    return;

                case 32: // Space - اگر منوی ویژه‌ای در نظر دارید
                    game->isRunning = true;
                    return;

                default:
                    break;
            }
        }
        // حالت برد
        else if (!game->isRunning && game->isSucces) {
            // اگر برد در مرحلهٔ اول است
            if (game->mapSelect == 1 || game->isIn_Map_1) {
                printf("Victory in Level 1! Ready for Level 2?\n\n");
                printf("'1': Next Level\n");
                printf("'2': Try again\n");
                printf("'3': Return to User Menu\n");
                printf("'esc': Exit\n");

                c = _getch();
                switch (c) {
                    case 27:
                        exit(0);
                    case '1':
                        system("cls");
                        // ورود به مرحله دوم (map2)
                        mciSendString("seek grd to start", NULL, 0, NULL);
                        goTo_map2(game);
                        return;
                    case '2':
                        system("cls");
                        // دوباره از اول بازی (map1) شروع کند
                        mciSendString("seek grd to start", NULL, 0, NULL);
                        start_game(currentUser, game);
                        return;
                    case '3':
                        userMenu(&head, currentUser);
                        return;
                    default:
                        break;
                }
            }
            // اگر برد در مرحلهٔ دوم (map2):
            else if (game->mapSelect == 2 || game->isIn_Map_2) {
                // این یعنی کل بازی تمام شده
                printf("Victory in final level! Congratulations!\n\n");
                printf("'1': Play Again\n");
                printf("'2': Return to User Menu\n");
                printf("'esc': Exit\n");

                c = _getch();
                switch (c) {
                    case 27:
                        exit(0);
                    case '1':
                        system("cls");
                        // شروع مجدد از map1
                        start_game(currentUser, game);
                        return;
                    case '2':
                        mciSendString("close ugrd", NULL, 0, NULL);
                        mciSendString("close grd", NULL, 0, NULL);
                        userMenu(&head, currentUser);
                        return;
                    default:
                        break;
                }
            }
        }
    }
}




void safe_gotoxy(int x, int y) {
    // بررسی محدوده مجاز x و y
    if (x > 0 && x < COLS-1 && y > 0 && y < ROWS-1) {
        gotoxy(x, y);
        //تابع فرعی برای برخی مشکلات پیش امده
        //البته استفاده خاصی ندارد و میتوان از ان صرف نظر کرد
    }
}

void updateFlowers(GameState* game) {
    //این تابع وضعیت گل ها در مپ اول را بروزرسانی میکند
    DWORD currentTime = GetTickCount();

    for (int i = 0; i < MAX_Flower; i++) {
        if (game->flower[i].flower_IsActive) {
            
            if (has_time_passed(game->flower[i].hideTime, 2000 + game->flower[i].randomDelay)) {
                game->flower[i].flower_IsActive = false; 
                game->flower[i].delayStartTime = currentTime; 
                game->flower[i].randomDelay = rand() % 1000; 
                //اگر 2 ثانیه گذشته بود، گل غیرفعال شده و در توابع دیگر ، گل نمایش داده نمیشود
            }
        } else {
            
            if (has_time_passed(game->flower[i].delayStartTime, 2000 + game->flower[i].randomDelay)) {
                game->flower[i].flower_IsActive = true; 
                game->flower[i].hideTime = currentTime; 
                game->flower[i].randomDelay = rand() % 1000; 
                // همچنین پس از 2 ثاینه غیرفعال بودن، دوباره فعال شده و نشان داده میشود
            }
        }
    }
}
void showFlowers(GameState* game) {

    if(game->mapSelect==1) {
        //فقط در مپ اول گل داریم
    
        for(int i=0; i<MAX_Flower; i++) {
            if(game->flower[i].flower_IsActive) {
                safe_gotoxy(game->flower[i].x,game->flower[i].y);
                printf("\033[38;5;34m🌱\033[0m\n");
                //اگر گل فعال بود، چاپ شود
            } else {
                safe_gotoxy(game->flower[i].x,game->flower[i].y);
                printf(" ");
                //و اگر نه، به جای آن دو اسپیس چاپ شود

            }
        }
    }
}
void printCoins(GameState* game) {
    //چاپ سکه های مپ
    int selected;
    if (game->mapSelect==1) {
        selected=coinOfmap1;
    }
    if (game->mapSelect==2) {
        selected=coinOfmap2;
    }
    if (game->mapSelect==3) {
        selected=coinOfmap3;
    }

    for(int i=0; i<selected; i++) {
        if(game->coins[i].coinIsActive) {
            gotoxy(game->coins[i].x,game->coins[i].y);
            printf("\x1b[1;93m●\x1b[0m ");
        }
        

    }
}

void check_update_coin_block(GameState *game) {
    //تابع بلوک سکه
    DWORD currentTime = GetTickCount(); // زمان فعلی

    for (int i = 0; i < MAX_CoinBlock; i++) {
        

        // بررسی برخورد بازیکن با بلوک سکه
        if (game->player.x == game->coin_block[i].x && 
            game->player.y == game->coin_block[i].y + 1 || (game->player.x == game->coin_block[i].x && 
            game->player.y == game->coin_block[i].y + 2 && (game->player.isBig && game->player.isJumping))) {
                
            // اگر بلوک فعال است
            if (game->coin_block[i].C_block_IsActive) {
                
                // اگر سکه‌ها تمام شده‌اند
                if (game->coin_block[i].maxCoin == 0) {
                    
                    // اگر زمان تاخیر گذشته است، بلوک را غیرفعال کن
                    if (has_time_passed(game->coin_block[i].hideTime, 500)) { // تاخیر ۵۰۰ میلی‌ثانیه
                    
                        game->map[game->coin_block[i].y][game->coin_block[i].x] = ' '; // پاک کردن بلوک از نقشه
                        PlaySound(TEXT(".\\sounds\\brick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                        game->coin_block[i].C_block_IsActive = false; // غیرفعال کردن بلوک
                    }
                } 
                // اگر سکه‌ها باقی مانده‌اند و زمان کافی گذشته است
                else if (game->coin_block[i].maxCoin > 0 && 
                         has_time_passed(game->coin_block[i].hideTime, 500)) { // تاخیر ۵۰۰ میلی‌ثانیه
                    safe_gotoxy(game->coin_block[i].x, game->coin_block[i].y + 2); // حرکت به مختصات سکه
                    printf("\x1b[1;93m●\x1b[0m "); 
                    
                    //پخش صدای سکه اگر ماریو انرا دریافت کند
                    PlaySound(TEXT(".\\sounds\\coin.wav"), NULL, SND_ASYNC | SND_FILENAME);
                    updateScore(game,0,1);
                    game->coin_block[i].maxCoin--; // کاهش تعداد سکه‌ها
                    game->coin_block[i].hideTime = currentTime; // به‌روزرسانی زمان آخرین برخورد

                    // اگر سکه‌ها تمام شدند، زمان تاخیر را شروع کن
                    if (game->coin_block[i].maxCoin == 0) {
                        game->coin_block[i].hideTime = currentTime; // زمان شروع تاخیر
                    }
                }
            }
        }
    }
}
void check_Coin_logic( GameState* game) {
    //بررسی شرط های سکه ها
    int selected;
    if (game->mapSelect==1) {
        selected=coinOfmap1;
    }
    if (game->mapSelect==2) {
        selected=coinOfmap2;
    }
    if (game->mapSelect==3) {
        selected=coinOfmap3;
    }

    for(int i=0; i<selected; i++) {
        if(check_collision(game->player.x,game->player.y,game->coins[i].x,game->coins[i].y) && game->coins[i].coinIsActive!=false) {
            PlaySound(TEXT(".\\sounds\\coin.wav"), NULL, SND_ASYNC | SND_FILENAME);
            updateScore(game,0,1);
            game->coins[i].coinIsActive=false;
            
        }

    }
}

void check_Mush_logic( GameState* game) {
    //بررسی شرط های قارچ
    if(game->mapSelect==2 ||game->mapSelect==1) {

        int selected;
        if(game->mapSelect==1) {
            selected=MAX_MushBlock_map1;

        }
        if(game->mapSelect==2) {
            selected=MAX_MushBlock_map2;
            
        }
        
        for (int i = 0; i < selected; i++) {
            if (check_collision(game->player.x, game->player.y - 1, 
                            game->mushroom_block[i].x, game->mushroom_block[i].y) &&
                game->mushroom_block[i].mushBlock_isActive && 
                !game->mushroom_block[i].mushEated) {

                // غیرفعال کردن بلوک و فعال کردن قارچ
                game->mushroom_block[i].mushBlock_isActive = false;
                game->mushroom_block[i].impacted = true;
                game->mushroom_block[i].Show_mush = true;
                

                PlaySound(TEXT(".\\sounds\\mushActive.wav"), NULL, SND_ASYNC | SND_FILENAME);
                
                
                game->mushroom_block[i].xMush = game->mushroom_block[i].x;
                game->mushroom_block[i].yMush = game->mushroom_block[i].y - 1;
                game->mushroom_block[i].velocity_of_Mush = (rand() % 2 == 0) ? 1 : -1;
                
                
                game->mushroom_block[i].lastHorizontalMove = GetTickCount();
                game->mushroom_block[i].lastVerticalMove = GetTickCount();
            }
        }
    }
}

// ------------------------------------------------------
// نمایش و حرکت قارچ
// ------------------------------------------------------
void showMush( GameState* game) {
    int selected;
    int selectedCoins;
    if(game->mapSelect==1 ||game->mapSelect==2) {
        if(game->mapSelect==1) {
            selected=MAX_MushBlock_map1;

           

        }
        if(game->mapSelect==2) {
            selected=MAX_MushBlock_map2;

            

        }
        
        
        DWORD currentTime = GetTickCount();

        for (int i = 0; i < selected; i++) {
            if (!game->mushroom_block[i].Show_mush || game->mushroom_block[i].mushEated) continue;

            // --- حرکت افقی (هر ۱ ثانیه) ---
            if (currentTime - game->mushroom_block[i].lastHorizontalMove >= 400) {
                safe_gotoxy(game->mushroom_block[i].xMush, game->mushroom_block[i].yMush);
                printf(" "); // پاک کردن موقعیت قبلی

                int newX = game->mushroom_block[i].xMush + game->mushroom_block[i].velocity_of_Mush;
                
                // بررسی امکان حرکت و تغییر جهت
                if (can_move(game, newX, game->mushroom_block[i].yMush ) && newX!=game->static_enemies[0].x) {
                    game->mushroom_block[i].xMush = newX;
                } else {
                    game->mushroom_block[i].velocity_of_Mush *= -1; // معکوس کردن جهت
                }

                game->mushroom_block[i].lastHorizontalMove = currentTime;
            }

            
            if (currentTime - game->mushroom_block[i].lastVerticalMove >= 150) {
                safe_gotoxy(game->mushroom_block[i].xMush, game->mushroom_block[i].yMush);
                printf(" "); 

                int newY = game->mushroom_block[i].yMush + 1;
                
                
                if (can_move(game, game->mushroom_block[i].xMush, newY)) {
                    game->mushroom_block[i].yMush = newY;
                }

                game->mushroom_block[i].lastVerticalMove = currentTime;
            }

            // --- نمایش قارچ در موقعیت جدید ---
            safe_gotoxy(game->mushroom_block[i].xMush, game->mushroom_block[i].yMush);
            printf("\033[38;5;180m🍄\033[0m\n");

            
        }
    }

}

void checkEatedMush(GameState* game) {
    //بررسی خوردن قارچ توسط ماریو
    int selected; 
    if(game->mapSelect==2 ||game->mapSelect==1) {

        if(game->mapSelect==1) {
            selected=MAX_MushBlock_map1;
        }
        if(game->mapSelect==2) {
            selected=MAX_MushBlock_map2;
        }

        for (int i = 0; i < selected; i++) {
            if (game->player.x == game->mushroom_block[i].xMush && 
                game->player.y == game->mushroom_block[i].yMush &&
                !game->mushroom_block[i].mushEated) {
                //پخش ساند افکت قارچ
                PlaySound(TEXT(".\\sounds\\mushEated.wav"), NULL, SND_ASYNC | SND_FILENAME);
                
                
                updateScore(game,1000,0);
                game->mushroom_block[i].mushEated = true;
                game->mushroom_block[i].Show_mush = false;
                game->player.isBig=true;
                //اگر ماریو قارچ را خورد
                // پاک کردن از صفحه
                safe_gotoxy(game->mushroom_block[i].xMush, game->mushroom_block[i].yMush);
                printf(" ");
            }
        }
    }
}

void showStaticEnemies(GameState* game) {
    //نشان دادن دشمن های ثابت یا همان دایناسور ها
    int selected;
    if (game->mapSelect==1) {
        selected=StaticEnemiesOfmap1;
    }
    if (game->mapSelect==2) {
        selected=StaticEnemiesOfmap2;
    }
    if (game->mapSelect==3) {
        selected=StaticEnemiesOfmap3;
    }

    for(int i=0; i<selected; i++) {
        if(game->static_enemies[i].staticENM_isActive) {
            //بردن اشاره گر به مکان دایناسور فعلی
            gotoxy(game->static_enemies[i].x,game->static_enemies[i].y);
            printf("\033[32m🦖\033[0m\n");

        }
    }
}

void moveNormalEnemy(GameState* game) {
    //تابع مهم 
    //حرکت دشمنان متحرک یا همان خرچنگ ها
    int selectedEnemies, selectedCoins;
    DWORD currentTime = GetTickCount();

    
    if (game->mapSelect == 1) {
        selectedEnemies = NormalEnemiesOfmap1;
        
    } if (game->mapSelect == 2) {
        selectedEnemies = NormalEnemiesOfmap2;
        
    } if (game->mapSelect == 3) {
        selectedEnemies = NormalEnemiesOfmap3;
        
    }
    //اگر 600 میلی ثاینه گذشته باشد، خرچنگ با سرعت فعلی اش به مکان بعدی حرکت کند
    //نکته مهم
    // عدد 600 میلی ثاینه به دلخواه میتواند تغییر کند
    if (currentTime - game->lastEnemyMoveTime >= 600) {
        
        for (int i = 0; i < selectedEnemies; i++) {
            safe_gotoxy(game->normal_enemies[i].x, game->normal_enemies[i].y);
            printf(" "); 

            int new_x = game->normal_enemies[i].x + game->normal_enemies[i].velocity;
            if (can_move(game, new_x, game->normal_enemies[i].y ) && ((game->mapSelect!=1) || new_x!=game->static_enemies[0].x) && (game->mapSelect!=2 || (game->map[game->normal_enemies[i].y][new_x]!='v' && game->map[game->normal_enemies[i].y+1][new_x+2]!='v'))) {
                game->normal_enemies[i].x = new_x;
            } else {
                game->normal_enemies[i].velocity *= -1;
                //برعکس شدن مسیر حرکت دشمن پس از برخورد با موانع
            }
        }

        game->lastEnemyMoveTime = currentTime;
        //ذخیره زمان برای بررسی بعدی
    }
}

void showNormalEnemies(GameState* game) {
    //نمایش دشمنان متجرک
    
    int selected;
    if (game->mapSelect==1) {
        selected=NormalEnemiesOfmap1;
    }
    if (game->mapSelect==2) {
        selected=NormalEnemiesOfmap2;
    }
    if (game->mapSelect==3) {
        selected=NormalEnemiesOfmap3;
    }
    
    //اگر دشمنان فعال بودند نشان داده شوند!
    for(int i=0; i<selected; i++) {
        if(game->normal_enemies[i].enemyIsActive) {
            gotoxy(game->normal_enemies[i].x,game->normal_enemies[i].y);
            printf("\033[35m🐙\033[0m\n");
            if(game->player.x==game->normal_enemies[i].x && game->player.y+1==game->normal_enemies[i].y) {
                //اگر از بالا به دشمن برخورد کنیم

                    DWORD currentTime = GetTickCount();
                    
                    if (currentTime - game->lastEnemyKillTime <= 5000) {
                        //اگر 5 ثانیه یا کمتر از کشتن دشمن قبلی گذشته باشد ، ضریب امتیاز دهی 2 برابر شود
                        game->scoreMultiplier *= 2;
                    } else {
                        // اگر زمان بیشتری گذشته بود، ضریب 1 بماند
                        game->scoreMultiplier = 1;
                    }
                    
                    updateScore(game, 100 * game->scoreMultiplier, 0);
                    // امتیاز دهی به بازیکن با توجه به ضریب مشخص شده
                    game->lastEnemyKillTime = currentTime;
                    //صدای نابود شدن دشمن
                    PlaySound(TEXT(".\\sounds\\kick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                    //غیرفعال شدن دشمن
                    game->normal_enemies[i].enemyIsActive=false;
                }
                 else if(game->player.x==game->normal_enemies[i].x && game->player.y==game->normal_enemies[i].y) {
                    //اگر از چپ یا راست با دشمن برخورد کنیم
                if(game->mapSelect==1 || game->mapSelect==2) {
                    //اگر در مپ دوم یا اول بودیم
                    if(!game->player.shielded ) {
                        //اگر ماریو شیلد نداشت
                        
                        if(game->player.isBig) {
                            //اگر ماریو بزرگ بود جان اش کم نشود، فقط کوچک شود
                            game->player.isBig=false;
                        } else {
                            game->health--;
                            // اگر ماریو کوچک بود کوچک شود
                        }
                        game->player.x=1;
                        game->player.y=10;
                        //پس از برخورد ماریو، ماریو به اول مپ 1 یا 2 برود!
                        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
                        
                    } else if(game->player.shielded) {
                        //اگر ماریو شیلد داشت 
                        if(!game->player.shild_and_impact) {
                            //بررسی میشود که اولین دشمنی است که نابود میشود؟
                            PlaySound(TEXT(".\\sounds\\kick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->normal_enemies[i].enemyIsActive=false;
                            game->player.shild_and_impact=true;
                            //اگر اینطور بود،از هر طرف به دشمن برخورد کند، دشمن را نابود میکند


                        } else {
                            //اگر شیلد داشت و قبلا دشمنی را کشته بود، دیگر با برخورد به دشمن اورا نکشد، بلکه 3 خانه بالا بپرد
                            // ممکن است دوباره روی خود دشمن فرود بیایید و انرا بکشد!
                            // و پخش صدای یووو توسط ماریو
                            PlaySound(TEXT(".\\sounds\\yoShield.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->player.x=game->normal_enemies[i].x;
                            game->player.y=game->normal_enemies[i].y-3;
                        }
                    }
                    
                    
                }
                if(game->mapSelect==3 ) {
                    // همه شرط های بالا برای مپ سوم یا همان زیر زمین بررسی شود
                    if(!game->player.shielded ) {

                    
                        if(game->player.isBig) {
                            game->player.isBig=false;
                        } else {
                            game->health--;
                        }
                        game->player.x=27;
                        game->player.y=10;
                        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
                    } else if(game->player.shielded) {
                        if(!game->player.shild_and_impact) {
                            PlaySound(TEXT(".\\sounds\\kick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->normal_enemies[i].enemyIsActive=false;
                            game->player.shild_and_impact=true;

                        } else {
                            PlaySound(TEXT(".\\sounds\\yoShield.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->player.x=game->normal_enemies[i].x;
                            game->player.y=game->normal_enemies[i].y-3;
                        }
                    }
                    
                }
                
            } 
            
        }
    }        
    moveNormalEnemy(game);
    //تابع حرکت دشمنان فراخوانی میشود تا اطلاعات دمشنان بروز شود
}


void check_staticEnemy( GameState* game) {
    //مانند دشمنان مترج همه شرط ها بررسی می شود!
    //با این تفاوت که دایناسور ها  ثابت هستند و ماریو نمیتواند انها را نابود کند
    //مگر اینکه شیلد داشته باشد!
    int selected;
    if (game->mapSelect==1) {
        selected=StaticEnemiesOfmap1;
    }
    if (game->mapSelect==2) {
        selected=StaticEnemiesOfmap2;
    }
    if (game->mapSelect==3) {
        selected=StaticEnemiesOfmap3;
    }

    for(int i=0; i<selected; i++) {
        if(check_collision(game->player.x,game->player.y,game->static_enemies[i].x,game->static_enemies[i].y) && game->static_enemies[i].staticENM_isActive) {
            if(game->mapSelect==1 || game->mapSelect==2) {
                if(!game->player.shielded ) {
                    if(game->player.isBig) {
                        game->player.isBig=false;
                            
                    } else {
                        game->health--;
                        }
                        game->player.x=1;
                            game->player.y=12;
                        
                        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
                } else if(game->player.shielded) {
                    if(!game->player.shild_and_impact) {
                        PlaySound(TEXT(".\\sounds\\kick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                        game->static_enemies[i].staticENM_isActive=false;
                        game->player.shild_and_impact=true;

                    } else {
                        PlaySound(TEXT(".\\sounds\\yoShield.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->player.x=game->normal_enemies[i].x;
                            game->player.y=game->normal_enemies[i].y-3;

                    }
                }
                }
                if(game->mapSelect==3 || game->isIn_Map_3 ) {
                    if(!game->player.shielded ) {
                        if(game->player.isBig) {
                            game->player.isBig=false;
                            
                        } else {
                            game->health--;
                        }
                        game->player.x=27;
                        game->player.y=12;
                    } else if(game->player.shielded) {
                        if(!game->player.shild_and_impact) {
                            PlaySound(TEXT(".\\sounds\\kick.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->static_enemies[i].staticENM_isActive=false;
                            game->player.shild_and_impact=true;

                        } else {
                            PlaySound(TEXT(".\\sounds\\yoShield.wav"), NULL, SND_ASYNC | SND_FILENAME);
                            game->player.x=game->normal_enemies[i].x;
                            game->player.y=game->normal_enemies[i].y-3;

                        }
                    }
                    
                    PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
                }

        }

    }
}

void setConsoleSize(int width, int height) {
    // این تابع برای ویندوز ترمینال بیهوده است
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    
    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;
    SetConsoleScreenBufferSize(hOut, bufferSize);

    
    SMALL_RECT rect;
    rect.Left = 0;
    rect.Top = 0;
    rect.Right = width - 1;   
    rect.Bottom = height - 1; 

    SetConsoleWindowInfo(hOut, TRUE, &rect);
}


void load_map(GameState *game) {
    // لود کردن مپ ها از فایل ها
    FILE *fp;
    int rows;
    //قبل از لود مپ
    // پاکسازی آرایه map
    for (int i = 0; i < ROWS; i++) {
        memset(game->map[i], '\0', COLS + 1); // خالی کردن هر سطر از آرایه
    }

    if (game->mapSelect == 1) {
        fp = fopen("maps/map.txt", "r");
        rows = ROWS;
        if (!fp) {
            printf("Error opening map file!\n");
            return;
        }
    }

    if (game->mapSelect == 2) {
        fp = fopen("maps/map2.txt", "r");
        rows = ROWS;
        if (!fp) {
            printf("Error opening map2 file!\n");
            return;
        }
    }

    if (game->mapSelect == 3) {
        fp = fopen("maps/map3.txt", "r");
        rows = ROWS;
        if (!fp) {
            printf("Error opening map3 file!\n");
            return;
        }
    }

    for (int i = 0; i < rows; i++) {
        if (!fgets(game->map[i], COLS + 1, fp)) {
            printf("Error reading map at row %d\n", i);
            break;
        }
        game->map[i][strcspn(game->map[i], "\n")] = '\0';
    }
    fclose(fp);
}

void handle_input(GameState *game, user *currentUser) {
    //مدیریت ورودی های کاربر
    //مدیریت پرش ماریو
    // پخش صدای پرش ماریو
    if (_kbhit()) {
        int ch = _getch();
        int new_x = game->player.x;

        switch (ch) {
            case 'a': case 'A': case 75:new_x--; break;
            case 'd': case 'D': case 77: new_x++; break;
            case 'w': case 'W': case 32: case 72:
            if(!game->player.isBig) {
                PlaySound(TEXT(".\\sounds\\jumpS.wav"), NULL, SND_ASYNC | SND_FILENAME);
                
            }
            if(game->player.isBig) {
                PlaySound(TEXT(".\\sounds\\jumpB.wav"), NULL, SND_ASYNC | SND_FILENAME);

            }
                if (game->player.isSitting) {
                    //اگر ماریو بزگ باشد و نشسته باشد، با زدن دکمه بالا یا اسپیس یا دبلیو میتواند بلند شود و ادامه دهد
                    game->player.isSitting = false;
                    game->player.isBig = true;
                } else if (can_jump(game)) {
                    // اگر شرایط پریدن را داشت
                    game->player.remainingJump = JUMP_HEIGHT;
                    // به مقدار 4 کارکتر بپرد
                    game->player.isJumping = true;
                    game->player.isAtPeak = false;
                    game->player.lastJumpTime = GetTickCount();
                    // شروع تایمر برای بررسی زمان پرش!
                }
                break;
            case 's': case 'S': case 80:
            //این حالت فقط برای حالتی است که ماریو بزرگ باشد
            // میتواند بنشیند، اما دیگر حرکت نمیتواند بکند
            //و به ظاهر کوچک میشود
                if (game->player.isBig) {
                    game->player.isSitting = true;
                    game->player.isBig = false;
                }
                break;

            case 27: game->isRunning = false;
            //با زدنesc بازی گیم اور میشود! و 
            game->isSucces=false;
            break;
        }

        if (!game->player.isSitting && can_move_mario(game, new_x, game->player.y)) {
            //بررسی اینکه ماریو میتواند جرکت کند یا نه!
            game->player.x = new_x;
        }
        
        
    }
}


void update_physics(GameState *game) {
    //این تابع بیشتر پرش را مدیریت کرده 
    Mario *m = &game->player;
    DWORD currentTime = GetTickCount();

    if (m->isJumping) {
        if (m->isAtPeak) {
            //ا140 ثانیه ماریو را در اوج پرش نگه دارد تا مکانیک بازی طبیعی تر و راحت تر شود
            // این عدد قابل تغییر است
            if (currentTime - m->peakTime >= 140) {
                m->isAtPeak = false;
                m->isJumping = false;
            }
        } else {
            if (currentTime - m->lastJumpTime >= 90 && m->remainingJump > 0) {
                //هر 90 میلی ثانیه موقعی ماریو را بالاتر ببر تا حس پرش را القا کند
                int new_y = m->y - 1; 
                if (can_move_mario(game, m->x, new_y)) {
                    m->y = new_y;
                    m->remainingJump--;
                    m->lastJumpTime = currentTime; 
                    if (m->remainingJump == 0) {
                        m->isAtPeak = true;
                        m->peakTime = currentTime; 
                    }
                } else {
                    if (!m->isAtPeak) {
                        m->isAtPeak = true;
                        m->peakTime = currentTime; 
                    } else if (currentTime - m->peakTime >= 140) {
                        // اگر به طور مثال به مانعی رسید
                        // و پرش اش متوقف شد
                        // مثل حالت اوج پرش
                        // پرش را متوقف کن
                        // و 140 ثانیه در ان حالت نگه دار
                        // برای طبیعی تر شدن و راحت تر شدن مکانیک
                        m->isJumping = false; 
                        m->remainingJump = 0;
                    }
                }
            }
        }
    } else {
        if (currentTime - m->lastFallTime >= 70) {
            //هر 70 میلی ثانیه
            // ماریو را به پاین میبرد تا به زمین یا یک مانع برسد و پرش متوقف شود
            int new_y = m->y + 1;
            if (can_move_mario(game, m->x, new_y)) {
                m->y = new_y;
                m->lastFallTime = currentTime; 
            }
        }
    }
}

void chance_s_BlockCoin(GameState *game, int blockIndex) {
    //این تابع به احتمال 20 درصد به بلوک سکه، سکه میدهد
    //عدد 20 قابل تغییر است
    
    if (rand() % 100 < 20) { 
        
        gotoxy(game->sblock_Logic[blockIndex].x, game->sblock_Logic[blockIndex].y+1);
        printf("\x1b[1;93m●\x1b[0m "); 
        //پس از برخورد به بلوک شانسی سکه، و ازاد شدن سکه، صدای سکه پخش شود و به سکه های ماریو اضافه شود
        PlaySound(TEXT(".\\sounds\\coin.wav"), NULL, SND_ASYNC | SND_FILENAME);
        updateScore(game,0,1);
    }
}

void check_SBlocks(GameState *game) {
    //بلوک شانسی سکه و قوانین ان
    for (int i = 0; i < MAX_S_block; i++) {
        // بررسی برخورد بازیکن با بلوک از پایین
        //این شرط کمی پیچیده شده
        // و ممکن هست بخش هایی از ان اضافه باشد
        if ((game->player.x == game->sblock_Logic[i].x && 
             game->player.y == (game->player.isBig ? game->sblock_Logic[i].y + 2:game->sblock_Logic[i].y +1)  &&  // از بالا فرود آمدن
             game->sblock_Logic[i].blockS_IsActive && game->player.isBig)) {
                

            // زمان غیرفعال شدن بلوک را ذخیره کنید
            game->sblock_Logic[i].hideTime = GetTickCount();

            // پاک کردن بلوک از صفحه
            safe_gotoxy(game->sblock_Logic[i].x, game->sblock_Logic[i].y);
            printf(" "); // پاک کردن بلوک با چاپ یک فاصله
            //پخش صدای تخریب
            PlaySound(TEXT(".\\sounds\\brick.wav"), NULL, SND_ASYNC | SND_FILENAME);

            // بررسی شانس وجود سکه در بلوک
            chance_s_BlockCoin(game, i);

            // غیرفعال کردن بلوک
            game->sblock_Logic[i].blockS_IsActive = false;
        }


        // بررسی پریدن بازیکن از بالای بلوک
        if (
    // شرایطی که **همیشه** باید چک شوند
    game->sblock_Logic[i].blockS_IsActive 
    && game->player.isBig 
    && game->player.x == game->sblock_Logic[i].x
    
    && (
        // حالت اول: موقعیت Y خاص + اوج پرش
        (game->player.y == game->sblock_Logic[i].y - 5 && game->player.isAtPeak)
        
        || 
        
        // حالت دوم: فقط در مپ 2 با شرایط پرش
        (
            game->mapSelect == 2 
            && game->player.isJumping 
            && (game->player.y == game->sblock_Logic[i].y - 4 || game->player.y == game->sblock_Logic[i].y - 2)
        )
    )
) {
                //پخش صدا با ترد، برای تاخیر چند میلی ثانیه ای
                //برای اینکه ماریو تا زمانی که پرش اش تمام شود و به بلوک برسد تقریبا 500 میلی ثاینه یا کمی کمتر میگذرد
                HANDLE hThread = CreateThread(NULL, 0, playDelayedSound, NULL, 0, NULL);
                if (hThread) {
                    CloseHandle(hThread); 
                }
                
            // زمان شروع تاخیر را ذخیره کنید
            game->sblock_Logic[i].delayStartTime = GetTickCount();

            // غیرفعال کردن بلوک
            game->sblock_Logic[i].blockS_IsActive = false;
        }
        
        // بررسی گذشت زمان برای حذف بلوک از نقشه برای زمانی که از بالا به بلوک ضربه بزنیم)
        if (!game->sblock_Logic[i].blockS_IsActive && 
            has_time_passed(game->sblock_Logic[i].hideTime, 250)) {
            
            // حذف بلوک از نقشه (اگر در محدوده مجاز باشد)
            if (game->sblock_Logic[i].x >= 0 && game->sblock_Logic[i].x < COLS &&
                game->sblock_Logic[i].y >= 0 && game->sblock_Logic[i].y < ROWS) {
                    
                game->map[game->sblock_Logic[i].y][game->sblock_Logic[i].x] = ' ';
            }
        }
        
        // بررسی گذشت زمان برای حذف بلوک از نقشه از بالا)
        if (!game->sblock_Logic[i].blockS_IsActive && 
            game->sblock_Logic[i].delayStartTime != 0 && // اگر تاخیر شروع شده باشد
            has_time_passed(game->sblock_Logic[i].delayStartTime, 500)) { // 0.5 ثانیه تاخیر
            
            // پاک کردن بلوک از صفحه
            safe_gotoxy(game->sblock_Logic[i].x, game->sblock_Logic[i].y);
            printf(" "); // پاک کردن بلوک با چاپ یک فاصله

            // بررسی شانس وجود سکه در بلوک
            chance_s_BlockCoin(game, i);

            // حذف بلوک از نقشه (اگر در محدوده مجاز باشد)
            if (game->sblock_Logic[i].x >= 0 && game->sblock_Logic[i].x < COLS &&
                game->sblock_Logic[i].y >= 0 && game->sblock_Logic[i].y < ROWS) {
                game->map[game->sblock_Logic[i].y][game->sblock_Logic[i].x] = ' ';
            }

            // ریست زمان شروع تاخیر
            game->sblock_Logic[i].delayStartTime = 0;
        }
    }
}

void check_Flowers(GameState * game){
    //بررسی برخورد ماریو به گل ها
    for(int i=0; i<MAX_Flower; i++) {
        if(game->player.x==game->flower[i].x && game->player.y==game->flower[i].y) {

            if(game->flower[i].flower_IsActive) {
                if(game->player.isBig) {
                        game->player.isBig=false;
                    } else {
                        game->health--;
                    }
                updateScore(game,-500,0);
                //کاهش 500 امتیاز از ماریو
                
                game->player.x-=2;
                //ماریو را 2 خانه به عقب پرت میکند 
                //در مپ اول ممکن است در اثر این تغییر مکان ماریو، ماریو به دایناسور برخورد کند و باز هم اسیب ببنید!
                PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
            
        }
    }
}

void resetMap2And3(GameState *game) {
    
   //رسیت کردن صدای مپ ها
    mciSendString("seek grd to start", NULL, 0, NULL);
    mciSendString("seek ugrd to start", NULL, 0, NULL);
    // mapSelect = 2 می‌گذاریم تا مراحل بعدی بداند در ابتدای map2 هستیم:
    game->mapSelect = 2;

    // چون قراره از ابتدا وارد map2 شویم، visited_map3 را false می‌کنیم
    // تا وقتی بعداً وارد map3 شدیم، دوباره مقداردهی اولیه یا ذخیره/بازیابی درست انجام شود.
    game->visited_map3 = false;
    
    // می‌توانیم isIn_Map_2 و isIn_Map_3 را در اینجا false کنیم
    // تا وقتی وارد تابع goTo_map2 می‌شویم، مقداردهی لازم را انجام دهد.
    game->isIn_Map_2 = false;
    game->isIn_Map_3 = false;

    // هرچه مربوط به map2 و map3 است ریست می‌کنیم
    // مثلاً:
    reset_Coin_block(game);
    reset_s_block(game);
    game->player.shielded=false;
    game->player.shild_and_impact=false;


    // درنهایت map2 در ابتدای کار قرار خواهد گرفت و map3 هم خالی و آمادهٔ مقداردهی است.
}

void goTo_map2(GameState *game) {
    game->mapSelect = 2;

    
    if (game->isIn_Map_1 || game->isIn_Map_2) {
        // چون داریم از map1 (یا ریست کامل) می‌آییم:
        game->isIn_Map_1 = false;
        game->isIn_Map_2 = true;
        game->isMapChanged = true;

        // ریست و مقداردهی اولیه‌ی map2
        first_initialize_coin(game);
        first_initialize_normal_enemies(game);
        initialize_static_enemies(game);
        reset_Coin_block(game);
        reset_s_block(game);
        initialize_shield(game);
        

        // فعال کردن Sblockها طبق نیاز
        game->sblock_Logic[0].blockS_IsActive = true;
        game->sblock_Logic[1].blockS_IsActive = true;

        initialize_Coin_block(game);

        // تنظیم وضعیت پلیر
        game->player.isBig = false;
        game->health = 5;
        game->player.x = 1;
        game->player.y = 12;

        // موسیقی پس‌زمینه
        mciSendString("play grd repeat", NULL, 0, NULL);
    }

    // 2) اگر در map3 بودیم و داریم برمی‌گردیم به map2
    else if (game->isIn_Map_3) {
        // ابتدا وضعیت map3 را در آرایه موقت ذخیره می‌کنیم
        for (int i = 0; i < coinOfmap3; i++) {
            game->coinsTempformap3[i].x = game->coins[i].x;
            game->coinsTempformap3[i].y = game->coins[i].y;
            game->coinsTempformap3[i].coinIsActive = game->coins[i].coinIsActive;
        }
        for (int i = 0; i < NormalEnemiesOfmap3; i++) {
            game->normal_enemiesTempformap3[i].x = game->normal_enemies[i].x;
            game->normal_enemiesTempformap3[i].y = game->normal_enemies[i].y;
            game->normal_enemiesTempformap3[i].enemyIsActive = game->normal_enemies[i].enemyIsActive;
            game->normal_enemiesTempformap3[i].velocity = game->normal_enemies[i].velocity;
        }
        for (int i = 0; i < StaticEnemiesOfmap3; i++) {
            game->static_enemiesTempformap3[i].x=game->static_enemies[i].x;
            game->static_enemiesTempformap3[i].y=game->static_enemies[i].y;
            game->static_enemiesTempformap3[i].staticENM_isActive=game->static_enemies[i].staticENM_isActive;
        }
        for (int i = 0; i <MAX_CoinBlock; i++) {
            game->coin_blockTempformap3[i].x=game->coin_block[i].x;
            game->coin_blockTempformap3[i].y=game->coin_block[i].y;
            game->coin_blockTempformap3[i].C_block_IsActive=game->coin_block[i].C_block_IsActive;
            game->coin_blockTempformap3[i].maxCoin=game->coin_block[i].maxCoin;
            
        }
        for(int i = 0; i <MAX_S_block; i++) {
            game->sblock_LogicTempformap3[i].x=game->sblock_Logic[i].x;
            game->sblock_LogicTempformap3[i].y=game->sblock_Logic[i].y;
            game->sblock_LogicTempformap3[i].blockS_IsActive=game->sblock_Logic[i].blockS_IsActive;
            
        }

        // سپس وضعیت قبلی map2 را از آرایه temp مربوط به map2 برگردانیم
        for (int i = 0; i < coinOfmap2; i++) {
            game->coins[i].x = game->coinsTempformap2[i].x;
            game->coins[i].y = game->coinsTempformap2[i].y;
            game->coins[i].coinIsActive = game->coinsTempformap2[i].coinIsActive;
        }
        for (int i = 0; i < NormalEnemiesOfmap2; i++) {
            game->normal_enemies[i].x = game->normal_enemiesTempformap2[i].x;
            game->normal_enemies[i].y = game->normal_enemiesTempformap2[i].y;
            game->normal_enemies[i].enemyIsActive = game->normal_enemiesTempformap2[i].enemyIsActive;
            game->normal_enemies[i].velocity = game->normal_enemiesTempformap2[i].velocity;
        }
        for(int i=0; i<StaticEnemiesOfmap2; i++) {
            game->static_enemies[i].x= game->static_enemiesTempformap2[i].x;
            game->static_enemies[i].y= game->static_enemiesTempformap2[i].y;
            game->static_enemies[i].staticENM_isActive= game->static_enemiesTempformap2[i].staticENM_isActive;
            
        }
        for(int i = 0; i <MAX_CoinBlock; i++) {
            game->coin_block[i].x= game->coin_blockTempformap2[i].x;
            game->coin_block[i].y= game->coin_blockTempformap2[i].y;
            game->coin_block[i].C_block_IsActive=game->coin_blockTempformap2[i].C_block_IsActive;
            game->coin_block[i].maxCoin=game->coin_blockTempformap2[i].maxCoin;
        }
        for(int i = 0; i <MAX_S_block; i++) {
            game->sblock_Logic[i].x=game->sblock_LogicTempformap2[i].x;
            game->sblock_Logic[i].y=game->sblock_LogicTempformap2[i].y;
            game->sblock_Logic[i].blockS_IsActive=game->sblock_LogicTempformap2[i].blockS_IsActive;

        }

        //این ذخیره ها انجام میشود تا اگر کاربر از مپ 2 ب3 رفت و دوباره به مپ 2 بازگشت، همه چیز مثل قبل باقی بماند
        
        
        

        // حالا از map3 خارج شده‌ایم:
        game->isIn_Map_3 = false;
        game->isIn_Map_2 = true;
        game->isMapChanged = true;

        // جای ماریو را جایی بگذارید که از map3 برگشته است
        game->player.x = 44;
        game->player.y = 12;
    }

    // در انتها:
    game->isRunning = true;
    load_map(game);
    Sleep(300);
}


void goTo_map3(GameState *game) {
    game->mapSelect = 3;

    // شرایط 1) وقتی اولین بار از map2 وارد map3 می‌شویم (visited_map3 == false)
    //         => مقداردهی اولیه و ذخیره‌ی وضعیت map2
    if (game->isIn_Map_2 && !game->visited_map3) {
        // ابتدا وضعیت فعلی map2 را در متغیرهای temp ذخیره می‌کنیم
        for (int i = 0; i < coinOfmap2; i++) {
            game->coinsTempformap2[i].x = game->coins[i].x;
            game->coinsTempformap2[i].y = game->coins[i].y;
            game->coinsTempformap2[i].coinIsActive = game->coins[i].coinIsActive;
        }
        for (int i = 0; i < NormalEnemiesOfmap2; i++) {
            game->normal_enemiesTempformap2[i].x = game->normal_enemies[i].x;
            game->normal_enemiesTempformap2[i].y = game->normal_enemies[i].y;
            game->normal_enemiesTempformap2[i].enemyIsActive = game->normal_enemies[i].enemyIsActive;
            game->normal_enemiesTempformap2[i].velocity = game->normal_enemies[i].velocity;
        }
        for (int i = 0; i < StaticEnemiesOfmap2; i++) {
            game->static_enemiesTempformap2[i].x=game->static_enemies[i].x;
            game->static_enemiesTempformap2[i].y=game->static_enemies[i].y;
            game->static_enemiesTempformap2[i].staticENM_isActive=game->static_enemies[i].staticENM_isActive;
        }
        for (int i = 0; i <MAX_CoinBlock; i++) {
            game->coin_blockTempformap2[i].x=game->coin_block[i].x;
            game->coin_blockTempformap2[i].y=game->coin_block[i].y;
            game->coin_blockTempformap2[i].C_block_IsActive=game->coin_block[i].C_block_IsActive;
            game->coin_blockTempformap2[i].maxCoin=game->coin_block[i].maxCoin;
            
        }
        for(int i = 0; i <MAX_S_block; i++) {
            game->sblock_LogicTempformap2[i].x=game->sblock_Logic[i].x;
            game->sblock_LogicTempformap2[i].y=game->sblock_Logic[i].y;
            game->sblock_LogicTempformap2[i].blockS_IsActive=game->sblock_Logic[i].blockS_IsActive;
            
        }

        // حالا map3 را از صفر مقداردهی می‌کنیم
        first_initialize_normal_enemies(game);
        first_initialize_coin(game);
        initialize_static_enemies(game);
        reset_Coin_block(game);
        reset_s_block(game);
        initialize_Coin_block(game);

        // چون برای اولین بار است، حالا visited_map3 را true می‌کنیم
        game->visited_map3 = true;

        // تغییر پرچم‌های وضعیت
        game->isIn_Map_2 = false;
        game->isIn_Map_3 = true;
        game->isMapChanged = true;
    }

    // شرایط 2) اگر قبلاً به map3 رفته‌ایم (visited_map3 == true)
    //         دوباره از map2 -> map3 می‌آییم => وضعیت map2 را ذخیره و map3 را بازیابی
    else if (game->isIn_Map_2 && game->visited_map3) {
        // وضعیت map2 را در آرایهٔ temp مربوطه ذخیره می‌کنیم
        for (int i = 0; i < coinOfmap2; i++) {
            game->coinsTempformap2[i].x = game->coins[i].x;
            game->coinsTempformap2[i].y = game->coins[i].y;
            game->coinsTempformap2[i].coinIsActive = game->coins[i].coinIsActive;
        }
        for (int i = 0; i < NormalEnemiesOfmap2; i++) {
            game->normal_enemiesTempformap2[i].x = game->normal_enemies[i].x;
            game->normal_enemiesTempformap2[i].y = game->normal_enemies[i].y;
            game->normal_enemiesTempformap2[i].enemyIsActive = game->normal_enemies[i].enemyIsActive;
            game->normal_enemiesTempformap2[i].velocity = game->normal_enemies[i].velocity;
        }
        for (int i = 0; i < StaticEnemiesOfmap2; i++) {
            game->static_enemiesTempformap2[i].x=game->static_enemies[i].x;
            game->static_enemiesTempformap2[i].y=game->static_enemies[i].y;
            game->static_enemiesTempformap2[i].staticENM_isActive=game->static_enemies[i].staticENM_isActive;
        }
        for (int i = 0; i <MAX_CoinBlock; i++) {
            game->coin_blockTempformap2[i].x=game->coin_block[i].x;
            game->coin_blockTempformap2[i].y=game->coin_block[i].y;
            game->coin_blockTempformap2[i].C_block_IsActive=game->coin_block[i].C_block_IsActive;
            game->coin_blockTempformap2[i].maxCoin=game->coin_block[i].maxCoin;
            
        }
        for(int i = 0; i <MAX_S_block; i++) {
            game->sblock_LogicTempformap2[i].x=game->sblock_Logic[i].x;
            game->sblock_LogicTempformap2[i].y=game->sblock_Logic[i].y;
            game->sblock_LogicTempformap2[i].blockS_IsActive=game->sblock_Logic[i].blockS_IsActive;
            
        }

        // سپس map3 را از temp خودش (اگر قبلاً ذخیره کرده‌ایم) برگردانیم
        for (int i = 0; i < coinOfmap3; i++) {
            game->coins[i].x = game->coinsTempformap3[i].x;
            game->coins[i].y = game->coinsTempformap3[i].y;
            game->coins[i].coinIsActive = game->coinsTempformap3[i].coinIsActive;
        }
        for (int i = 0; i < NormalEnemiesOfmap3; i++) {
            game->normal_enemies[i].x = game->normal_enemiesTempformap3[i].x;
            game->normal_enemies[i].y = game->normal_enemiesTempformap3[i].y;
            game->normal_enemies[i].enemyIsActive = game->normal_enemiesTempformap3[i].enemyIsActive;
            game->normal_enemies[i].velocity = game->normal_enemiesTempformap3[i].velocity;
        }
        for (int i = 0; i < StaticEnemiesOfmap3; i++) {
            game->static_enemies[i].x= game->static_enemiesTempformap3[i].x;
            game->static_enemies[i].y= game->static_enemiesTempformap3[i].y;
            game->static_enemies[i].staticENM_isActive= game->static_enemiesTempformap3[i].staticENM_isActive;
        }
        for (int i = 0; i <MAX_CoinBlock; i++) {
            game->coin_block[i].x=game->coin_blockTempformap3[i].x;
            game->coin_block[i].y= game->coin_blockTempformap3[i].y;
            game->coin_block[i].C_block_IsActive= game->coin_blockTempformap3[i].C_block_IsActive;
            game->coin_block[i].maxCoin= game->coin_blockTempformap3[i].maxCoin;
        }
        for (int i = 0; i <MAX_S_block; i++) {
            game->sblock_Logic[i].x= game->sblock_LogicTempformap3[i].x;
            game->sblock_Logic[i].y= game->sblock_LogicTempformap3[i].y;
            game->sblock_Logic[i].blockS_IsActive=game->sblock_LogicTempformap3[i].blockS_IsActive;
        }
        

        //این ذخیره ها انجام میشود تا اگر کاربر از مپ 2 ب3 رفت و دوباره به مپ 2 بازگشت، همه چیز مثل قبل باقی بماند
        

        // تغییر شرایط بازی
        game->isIn_Map_2 = false;
        game->isIn_Map_3 = true;
        game->isMapChanged = true;
    }

    
    load_map(game);

    // جای اولیهٔ ماریو در map3
    game->player.x = 28;
    game->player.y = 2;

    Sleep(300);
}


void check_gameTIMER(GameState * game) {
    //بررسی زمان بازی
    // اگر زمان تمام شود بازی گیم اور میشود
    DWORD currentTime = GetTickCount();
    if (currentTime - game->startTime >= 1000) {  
        game->remainingTime -= 1000;
        game->startTime = currentTime;  

        
        if (game->remainingTime <= 0) {
            game->isRunning = false;
            game->isSucces = false;
            if(game->isIn_Map_2 ||game->isIn_Map_3|| game->mapSelect==2) {
                //این شرط برای توابعgoto map2-3 
                //مهم بود
                // تا شرایط را به حالتی برگردانیم که مپ 2-3 کاملا ریست شود و انگار از مپ 1 وارد مپ 2 شده ایم!
                game->isIn_Map_1 = true;
                game->visited_map3 = false;
            }
            mciSendString("stop grd", NULL, 0, NULL);
            PlaySound(TEXT(".\\sounds\\gameOver.wav"), NULL, SND_ASYNC | SND_FILENAME);
        }
    }
}
void displayTimer(GameState *game) {
    DWORD elapsedTime = GetTickCount() - game->startTime;
    DWORD timeLeft = game->remainingTime - elapsedTime;

    if (timeLeft < 0) timeLeft = 0;  // اطمینان از اینکه تایمر منفی نشود

   
    

    // نمایش ثانیه و میلی‌ثانیه*
    printf("Time: %d.%01d sec", timeLeft / 1000, timeLeft % 10);
    
}

void show_shield_(GameState* game) {
    //نمایش شیلد در مپ
    if(game->mapSelect==2)
        if(game->shield.shieldActive ) {
            gotoxy(game->shield.x,game->shield.y) ;
            printf("\033[36m🛡️\033[0m\n");
        } else {
            //اگر شیلد غیرفعال شد یا به عبارتی توسط ماریو خورده شده
            // حذف شود از روی مپ
            gotoxy(game->shield.x,game->shield.y) ;
            printf("  ");

        }
    
}
void check_shield_eated(GameState* game) {
    // بررسی وضعیت فعال بودن شیلد
    if(game->mapSelect==2 && 
        game->player.x==game->shield.x &&
        game->player.y==game->shield.y 
    ) {
        
        if(game->shield.shieldActive) {
            
            PlaySound(TEXT(".\\sounds\\shield.wav"), NULL, SND_ASYNC | SND_FILENAME);
            game->shield.shieldActive=false;

        }

        game->player.shielded=true;
    }
    
}
void updateLoginc(GameState*game) {
    //تابع مهم قوانین بازی
    check_Coin_logic(game);
    check_SBlocks(game);

    check_Flowers(game);
    check_update_coin_block(game);

    if(!game->isRunning) {
        //اگر بازی استپ شد به هر دلیل، موزیک قطع شود
        mciSendString("stop grd", NULL, 0, NULL);
        mciSendString("stop ugrd", NULL, 0, NULL);
    }

    if(game->map[game->player.y+1][game->player.x]=='@' && game->mapSelect==1) {
        //پخش صدای حرکت از طریق لوله
        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
        //اگر در مپ اول بودیم و با این موقعیت @ رسیدیم
        // در واقع باید از طریق لوله، ماریو به لوله بعدی جا به جا شود
        game->player.x=25;
        game->player.y= game->player.isBig ? 5 : 4 ;
        
    }

    if(game->map[game->player.y+1][game->player.x]=='@' && game->mapSelect==2) {
        mciSendString("stop grd", NULL, 0, NULL);
        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
        mciSendString("play ugrd repeat", NULL, 0, NULL);
        //اگر در مپ دوم  بودیم و به این موقعیت @ رسیدیم
        //باید به زیر زمین یا مپ سوم برویم

        goTo_map3(game);
        
    }

    check_staticEnemy(game); 
    check_Mush_logic(game);
    checkEatedMush(game);
    check_shield_eated(game);

    if(game->map[game->player.y][game->player.x]=='f' &&( game->isIn_Map_1 ||game->mapSelect==1)) {
        //اگر در مپ اول به پرچم رسیدیم، برده ایم
        //اماده اسزی شرایط ورود به مپ دوم
        mciSendString("stop grd", NULL, 0, NULL);
        mciSendString("seek grd to start", NULL, 0, NULL);
        Sleep(300);
        PlaySound(TEXT(".\\sounds\\flag.wav"), NULL, SND_ASYNC | SND_FILENAME);
        Sleep(1000);
        PlaySound(TEXT(".\\sounds\\win.wav"), NULL, SND_ASYNC | SND_FILENAME);
        Sleep(1000);
        game->isSucces=true;
        game->isRunning=false;
        
        
    }
    if(game->map[game->player.y][game->player.x]=='f' && ( game->isIn_Map_2 ||game->mapSelect==2)) {
        //اگر در مپ دوم به پرچم رسیدیم باز هم مرحله با موفقیت تمام میشود
        game->player.y-=1;
        mciSendString("stop grd", NULL, 0, NULL);
        Sleep(300);
        PlaySound(TEXT(".\\sounds\\flag.wav"), NULL, SND_ASYNC | SND_FILENAME);
        Sleep(1000);
        PlaySound(TEXT(".\\sounds\\win.wav"), NULL, SND_ASYNC | SND_FILENAME);
        Sleep(1000);
        game->isSucces=true;
        game->isRunning=false;
        
        
    }
    if(game->map[game->player.y][game->player.x+1]=='z' && game->map[game->player.y+1][game->player.x+1]=='#' &&
    game->map[game->player.y][game->player.x+2]=='z' && game->mapSelect==3) {

        //اگر به لوله z رسیدیم در مپ سوم
        //به مپ دوم باز میگردیم
        Sleep(400);
        gotoxy(game->player.x,game->player.y);
        printf(" ");
        Sleep(300);
        mciSendString("stop ugrd", NULL, 0, NULL);
        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
        //تغییر اهنگ مپ3 به مپ 2
        mciSendString("play grd repeat", NULL, 0, NULL);
        goTo_map2(game);
    }


    if(game->mapSelect==3 && game->map[game->player.y][game->player.x]=='v') {
        //اگر در مپ سوم به پاله یا همانv افتادیم
        // ماریو به اول مپ 3 برگردد و جان اش کم شود!
        // یا اگر بزرگ بود، کوچ شود
        Sleep(300);
        gotoxy(game->player.x,game->player.y);
        if(game->player.isBig) {
                        game->player.isBig=false;
                    } else {
                        game->health--;
                    }
        
        printf(" ");
        //حدف ماریو
        // پخش صدا
        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
        Sleep(500);
        
        game->player.x=27;
        game->player.y=12;
    }


    if(game->health==0) {
        //بررسی جان باقی مانده
        game->isRunning=false;
        game->isSucces=false;
        //اگر جان صفر شد
        // بازی استپ میشود
        PlaySound(TEXT(".\\sounds\\death.wav"), NULL, SND_ASYNC | SND_FILENAME);
        if(game->isIn_Map_1) {
            mciSendString("close grd", NULL, 0, NULL);
        }
        if(game->isIn_Map_2) {
            mciSendString("stop grd", NULL, 0, NULL);
            game->isIn_Map_1 = true;
            game->visited_map3 = false;
        }
        if(game->isIn_Map_3) {
            mciSendString("stop ugrd", NULL, 0, NULL);
            game->isIn_Map_1 = true;
            game->visited_map3 = false;
        }
        Sleep(2000);
    }

    
    if(game->map[game->player.y+1][game->player.x]=='&' && game->mapSelect==2) {
        //اگ در مپ دوم بودیم به لوله زرد رسیدیم
        // به سمت پرچم منتطقل شویم
        PlaySound(TEXT(".\\sounds\\pipeTravel.wav"), NULL, SND_ASYNC | SND_FILENAME);
        game->player.x=61;
        game->player.y= 2 ;
        
        
    }

    check_gameTIMER(game);
    //بررسی تایم باقی مانده
}
void showHealth(GameState* game) {
    //نشان دادن جان باقی مانده
    // بصورت 5 ایموجی قلب
    
    for(int i=0; i<game->health; i++) {
        printf("❤️");
         
    }
    for (int i = game->health; i < 5; i++) { 
        printf("  "); 
    }

    printf("\t");
}
void displayState(GameState *game) {
    // نشان دادن وضعیت سکه و اسکور و زمان باقی مانده و جان ماریو
    gotoxy(19,16);
    displayTimer(game);
    printf("\t");
    showHealth(game);
    
    printf("💰 = %d\t✨ = %d\t",game->hstrGame[game->currentHistoryIndex].coin,game->hstrGame[game->currentHistoryIndex].score);
    printf("\t");
    
    

    if(game->player.shielded) {
        printf("\033[36m🛡️\033[0m Active!\t");
    }

}

void render(GameState *game) {
    //نشان دادن ماریو
    // نشان دادن وضعیت ماریو

    if(!game->player.isBig) {
        gotoxy(game->player.x, game->player.y);
        printf("🧑");
    } else {
        gotoxy(game->player.x, game->player.y-1);
        printf("🧑");
        gotoxy(game->player.x, game->player.y);
        printf("🧑");
    }

    displayState(game);
    
}

static bool can_move(const GameState *game, int new_x, int new_y) {
    //بررسی امکان حرکت برای دشمنان متحرک
    return (
        new_x >= 0 && new_x < COLS &&
        new_y >= 0 && new_y < ROWS &&
        game->map[new_y][new_x] != '#' &&
        
        game->map[new_y][new_x] != 's' &&
        game->map[new_y][new_x] != 'c' &&
        game->map[new_y][new_x + 1] != 'p' &&
        game->map[new_y][new_x + 1] != '!' &&
        game->map[new_y][new_x - 1] != '!' &&
        game->map[new_y][new_x] != '!' &&
        game->map[new_y][new_x] != 'p' && 
        game->map[new_y][new_x+1] != 'P' &&
        game->map[new_y][new_x] != 'P' && 
        game->map[new_y][new_x+1] != '@' && 
        game->map[new_y][new_x-1] != '@' && 
        game->map[new_y][new_x] != '@' &&
        game->map[new_y][new_x] != 'm' /* &&
        (game->mapSelect != 3 || game->map[new_y][new_x] != 'z') */
        

        

    );
}


static bool can_jump(const GameState *game) {
    //بررسی اینکه ایا ماریو روی جسمی که ایستاده توان پرش دارد یا خیر!
    const Mario *m = &game->player;
    return (
        m->y == ROWS - 2 ||
        game->map[m->y + 1][m->x] == '#' ||
        
        game->map[m->y + 1][m->x] == 's' ||
        game->map[m->y + 1][m->x] == 'c' ||
        game->map[m->y + 1][m->x] == '@'||
        game->map[m->y + 1][m->x-1] == '@'||
        game->map[m->y + 1][m->x+1] == '@'||
        game->map[m->y + 1][m->x] == '!' ||
        game->map[m->y + 1][m->x-1] == '!' ||
        game->map[m->y + 1][m->x+1] == '!' ||
        game->map[m->y + 1][m->x] == 'm' ||
        (game->map[m->y + 1][m->x] == 'z' && game->mapSelect==3)) ;
    
}

static void hide_cursor() {
    CONSOLE_CURSOR_INFO info = {100, FALSE};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

static void gotoxy(int x, int y) {
    //تابع پر استفاده برای مشخص کردن مکان نما
    COORD coord = {x*2 , y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void start_game(user* current, GameState *game) {
    //تابع اصلی اجرای گیم
    //اگر بازی استپ شود به تایعend_or_map2
    // میرویم و در انجا تصمیم به چگونگی ادامه بازی میگیریم
    // و دویاره با فعالش دن بازی، با این حلقه باز میگردیم
    mciSendString("seek grd to start", NULL, 0, NULL);
    system("mode con cols=140 lines=30");
    game->mapSelect=1;
    
    game->isSucces= false;
    game->isMapChanged=false;
    game->visited_map1=false;
    game->visited_map2=false;
    game->visited_map3=false;
    game->isRunning = true;
    initialize_game(game, current);
    mciSendString("play grd repeat", NULL, 0, NULL);
    printmap(game->map, game);
    

    game->startTime = GetTickCount();  
    game->remainingTime = 50000;  
    

    // اضافه کردن یک حلقۀ do/while
    do {
        // حلقۀ اصلی بازی
        while (game->isRunning) {
            
            if (game->isMapChanged) {
                system("cls");
                
                printmap(game->map, game);
                
                game->isMapChanged = false;
                if(!game->isIn_Map_2 ||!game->isIn_Map_3) {
                    game->startTime = GetTickCount();  
                    game->remainingTime = 50000; 
                }
                 
                
            }

            if (!game->player.isBig) {
                gotoxy(game->player.x, game->player.y);
                printf(" \b\n");
            } else {
                gotoxy(game->player.x, game->player.y - 1);
                printf(" \b\n");
                gotoxy(game->player.x, game->player.y);
                printf(" \b\n");
            }
            updateLoginc(game);
            
            updateFlowers(game);
            handle_input(game, current);
            update_physics(game);
            printCoins(game);
            show_shield_(game);
            showMush(game);
            showFlowers(game);
            showNormalEnemies(game);
            showStaticEnemies(game);
            render(game);
            

            Sleep(17);
        }

        // وقتی از حلقۀ بازی بیرون می‌آییم (برد یا باخت)، این تابع منو را نشان می‌دهد
        end_or_readyfor_roun2(game, current);

        // اگر داخل end_or_readyfor_roun2 کاربر «ادامه» یا «مرحله بعد» را بزند
        // ممکن است دوباره game->isRunning=true شود و در نتیجۀ do/while
        // یک بار دیگر حلقه را از بالا ادامه می‌دهیم.
    } while (game->isRunning);
}



//برای دیباگ و جک فایلgame.c
/* int main() {
    srand(time(NULL));
    GameState game;
    
    
    system("cls");
    system("chcp 65001 > nul");
    
    user h;
    SetConsoleOutputCP(CP_UTF8);
    start_game(&h,&game);
    return 0;
}
 */