/*
 * 围棋游戏系统 - Part 1: 核心头文件和数据结构
 * 负责人: 251880102 胡哲
 * 包含: 全局定义、数据结构、核心游戏逻辑函数声明
 */

#ifndef PART1_CORE_H
#define PART1_CORE_H

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <io.h>

 // 棋盘基础配置
#define BOARD_SIZE 19
#define CELL_SIZE 30
#define STONE_RADIUS 13
#define BOARD_MARGIN 50
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 750

// 棋子状态
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// 系统配置
#define MAX_HISTORY 500
#define MAX_NAME_LENGTH 50

// 游戏状态结构
typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int currentPlayer;
    int blackCaptures;
    int whiteCaptures;
    int moveCount;
    int blackTime;  // 黑方剩余时间(秒)
    int whiteTime;  // 白方剩余时间(秒)
    int lastCaptureCount; // 上一手提子数
} GameState;

// 历史记录结构
typedef struct {
    int x;
    int y;
    int player;
    int capturedStones;
    int boardSnapshot[BOARD_SIZE][BOARD_SIZE];
    time_t timestamp;
} HistoryMove;

// 游戏配置结构
typedef struct {
    int boardSize;
    float komi;
    int timeLimit;
    int enableSound;
    int enableAnimation;
    int aiDifficulty; // 1-简单 2-中等 3-困难
    char playerBlackName[MAX_NAME_LENGTH];
    char playerWhiteName[MAX_NAME_LENGTH];
} GameConfig;

// 全局变量声明
extern GameState gameState;
extern HistoryMove history[MAX_HISTORY];
extern int historyCount;
extern GameConfig config;
extern int gameMode;
extern int lastMoveX, lastMoveY;
extern int hintX, hintY;
extern IMAGE imgBoard, imgWhiteStone, imgBlackStone;
extern int imagesLoaded;

// Part 1 核心函数声明
void initGame();
void loadConfig(const char* filename);
void saveConfig(const char* filename);
int isValidMove(int x, int y);
void placeStone(int x, int y);
int hasLiberty(int x, int y, int color, int visited[BOARD_SIZE][BOARD_SIZE]);
void removeStones(int x, int y, int color);
int checkCapture(int x, int y, int color);
void undoMove();
int countTerritory(int x, int y, int* owner, int visited[BOARD_SIZE][BOARD_SIZE]);
void saveGame(const char* filename);
void loadGame(const char* filename);

// Part 2 图形渲染函数声明 (251880599 唐瑞添)
void loadImages();
void drawWhiteStone(int px, int py);
void drawBlackStone(int px, int py);
void drawStoneWithImage(int x, int y, int color);
void drawBoard();
void drawUI();

// Part 3 AI与菜单函数声明 (2518801370 李卓烨)
int evaluatePosition(int x, int y);
void getAIMove(int* x, int* y);
void calculateScore();
void showMainMenu();
void handleMenuClick(int x, int y);

// Part 4 交互控制函数声明 (251880107 马耀宗)
void handleClick(int mouseX, int mouseY);
void handleKeyboard();
void exportGameRecord(const char* filename);

#endif // PART1_CORE_H