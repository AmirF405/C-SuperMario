#ifndef GAME_H
#define GAME_H
#include "users.h"
#include <windows.h>
#include <conio.h>
#include <stdbool.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define Maxmap_ROWS 100
#define Maxmap_COLS 100
#define ROWS 14
#define ROWS2 14
#define COLS 66
#define GRAVITY 1
#define JUMP_HEIGHT 4
#define StaticEnemiesOfmap1 4
#define StaticEnemiesOfmap2 1
#define StaticEnemiesOfmap3 2 //3//
#define NormalEnemiesOfmap1 3
#define NormalEnemiesOfmap2 3
#define NormalEnemiesOfmap3 1
#define coinOfmap1 6
#define coinOfmap2 4
#define coinOfmap3 4 
#define MAX_S_block 2

#define MAX_Flower 2
#define MAX_CoinBlock 3
#define MAX_MushBlock_map1 1
#define MAX_MushBlock_map2 1

#define MAX_coin  6
#define MAX_staticEnemy  4
#define MAX_normalEnemy 3





typedef struct {
    
    int x;
    int y;
    int velocity;
    bool isJumping;
    int remainingJump;
    bool isBig;
    bool isSitting ;
    DWORD lastJumpTime;
    DWORD lastFallTime;
    DWORD peakTime;
    bool isAtPeak;
    bool shielded;
    bool shild_and_impact;
    
} Mario;

typedef struct {
    
    int x;
    int y;
    bool staticENM_isActive;
   
} EnemyStatic;

typedef struct {
    int x;
    int y;
    int velocity;
    bool enemyIsActive;
    
    
} EnemyNormal;

typedef struct {
    int x;
    int y;
    bool coinIsActive;
    
    DWORD hideTime;
    
} Coin;

typedef struct {
    int x;
    int y;
    bool blockS_IsActive;
    
    DWORD delayStartTime;
    DWORD hideTime;
    
} Sblock_Logic;

typedef struct {
    int x;
    int y;
    bool C_block_IsActive;
    int maxCoin;
    DWORD hideTime;
    
} COIN_block_Logic;

typedef struct {
    int x;
    int y;
    bool mushBlock_isActive;
    bool impacted;
    int xMush;
    int yMush;
    bool Show_mush;
    bool mushEated;
    int velocity_of_Mush;
    DWORD lastHorizontalMove;
    DWORD lastVerticalMove;
    
    DWORD firstImpact;
    
} Mushroom_block_Logic;

typedef struct {
    int x;
    int y;
    
    bool flower_IsActive;
    DWORD hideTime;
    DWORD delayStartTime;
    DWORD randomDelay;
    
} Flower;

typedef struct {
    int x;
    int y;
    bool shieldActive;
    

} Shield;



//ساختار اصلی بازی
typedef struct {
    char map[Maxmap_ROWS][Maxmap_COLS];
    
    Mario player;
    EnemyStatic static_enemiesTempformap2[StaticEnemiesOfmap1];
    EnemyStatic static_enemiesTempformap3[StaticEnemiesOfmap1];
    EnemyStatic static_enemies[StaticEnemiesOfmap1];
    EnemyNormal normal_enemies[NormalEnemiesOfmap1];
    EnemyNormal normal_enemiesTempformap2[NormalEnemiesOfmap1];
    EnemyNormal normal_enemiesTempformap3[NormalEnemiesOfmap1];
    history hstrGame[MaxHistory];
    bool isRunning;
    DWORD lastEnemyMoveTime;
    Coin coins[10];
    Coin coinsTempformap2[10];
    Coin coinsTempformap3[10];
    Sblock_Logic sblock_Logic[MAX_S_block];
    Sblock_Logic sblock_LogicTempformap2[MAX_S_block];
    Sblock_Logic sblock_LogicTempformap3[MAX_S_block];
    int camX; //برای مپ اسکرولینگ
    int camY;//بی استفاده
    
    Flower flower[MAX_Flower];
    COIN_block_Logic coin_block [MAX_CoinBlock];
    COIN_block_Logic coin_blockTempformap2 [MAX_CoinBlock];
    COIN_block_Logic coin_blockTempformap3 [MAX_CoinBlock];
    Mushroom_block_Logic mushroom_block[MAX_MushBlock_map1];
    bool isSucces;
    int currentHistoryIndex;
    int mapSelect;
    bool isIn_Map_1;
    bool isIn_Map_2;
    bool isIn_Map_3;
    bool isMapChanged;
    bool visited_map1;
    bool visited_map2;
    bool visited_map3;
    int health;
    DWORD startTime;  // زمان شروع مرحله
    DWORD remainingTime; // زمان باقی مانده!
    DWORD lastEnemyKillTime;
    int scoreMultiplier;
    Shield shield;
    
} GameState;
//پروتوتایپ توابعgame.c

void initializeFlowers(GameState *game);
void initialize_Coin_block(GameState *game);
void initialize_normal_enemies(GameState* game);
void first_initialize_normal_enemies(GameState* game);
void first_initialize_coin(GameState * game);
void initialize_coin(GameState * game);
void initialize_game(GameState *game, user *currentUser);
bool check_collision(int mario_x, int mario_y, int enemy_x, int enemy_y);
bool has_time_passed(DWORD last_time, DWORD interval);

void updateFlowers(GameState* game);
void showFlowers(GameState* game);
void printCoins(GameState* game);
void safe_gotoxy(int x, int y);
void check_update_coin_block(GameState *game);
void check_Coin_logic( GameState* game);
void check_Mush_logic( GameState* game);
void showMush( GameState* game);
void moveNormalEnemy(GameState* game);
void showNormalEnemies(GameState* game);
void check_staticEnemy( GameState* game);
void checkEatedMush(GameState* game);
void showStaticEnemies(GameState* game);
void chance_s_BlockCoin(GameState *game, int blockIndex);
void check_SBlocks(GameState *game);
void check_Flowers(GameState * game);
void updateLoginc(GameState*game);
void load_map(GameState *game);
void handle_input(GameState *game, user *currentUser);
void update_physics(GameState *game);
void goTo_map3(GameState *game);
void goTo_map2(GameState *game);
void render(GameState *game);
void start_game(user* current, GameState *game);
void setConsoleSize(int width, int height);
void end_or_readyfor_roun2(GameState *game, user *currentUser);
void printmap(char map[Maxmap_ROWS][Maxmap_COLS], GameState *game);
void updateScore(GameState* game, int scoreDelta, int coinDelta);
void initialize_history(GameState* game, user* current);
void reset_Coin_block(GameState *game);
DWORD WINAPI playDelayedSound(LPVOID lpParam);
void reset_s_block(GameState *game);
void displayTimer(GameState *game);
void check_gameTIMER(GameState * game);
void show_shield_(GameState* game);
void check_shield_eated(GameState* game);
void initialize_shield(GameState * game);
void saveHISTORY(GameState* game, user * currentUser);
void resetMap2And3(GameState *game);
void showHealth(GameState* game);
void displayState(GameState *game);



#endif