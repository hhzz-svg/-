/*
 * 围棋游戏系统 - Part 1: 核心游戏逻辑实现
 * 负责人: 251880102 忽哲
 * 实现: 游戏初始化、配置管理、落子逻辑、气的判断、提子逻辑、存取档
 */

#include "Part1_Core.h"

 // 全局变量定义
GameState gameState;
HistoryMove history[MAX_HISTORY];
int historyCount = 0;
GameConfig config;
int gameMode = 0;
int lastMoveX = -1, lastMoveY = -1;
int hintX = -1, hintY = -1;
IMAGE imgBoard, imgWhiteStone, imgBlackStone;
int imagesLoaded = 0;

// 初始化游戏
void initGame() {
    memset(&gameState, 0, sizeof(GameState));
    gameState.currentPlayer = BLACK;
    gameState.blackTime = config.timeLimit * 60;
    gameState.whiteTime = config.timeLimit * 60;
    historyCount = 0;
    lastMoveX = lastMoveY = -1;
    hintX = hintY = -1;

    loadConfig("config.txt");
}

// 加载配置文件
void loadConfig(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        // 默认配置
        config.boardSize = BOARD_SIZE;
        config.komi = 7.5f;
        config.timeLimit = 30;
        config.enableSound = 1;
        config.enableAnimation = 1;
        config.aiDifficulty = 2;
        strcpy(config.playerBlackName, "黑方");
        strcpy(config.playerWhiteName, "白方");
        saveConfig(filename);
        return;
    }

    fscanf(fp, "BoardSize=%d\n", &config.boardSize);
    fscanf(fp, "Komi=%f\n", &config.komi);
    fscanf(fp, "TimeLimit=%d\n", &config.timeLimit);
    fscanf(fp, "EnableSound=%d\n", &config.enableSound);
    fscanf(fp, "EnableAnimation=%d\n", &config.enableAnimation);
    fscanf(fp, "AIDifficulty=%d\n", &config.aiDifficulty);
    fscanf(fp, "BlackPlayer=%[^\n]\n", config.playerBlackName);
    fscanf(fp, "WhitePlayer=%[^\n]\n", config.playerWhiteName);

    fclose(fp);
}

// 保存配置文件
void saveConfig(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) return;

    fprintf(fp, "BoardSize=%d\n", config.boardSize);
    fprintf(fp, "Komi=%.1f\n", config.komi);
    fprintf(fp, "TimeLimit=%d\n", config.timeLimit);
    fprintf(fp, "EnableSound=%d\n", config.enableSound);
    fprintf(fp, "EnableAnimation=%d\n", config.enableAnimation);
    fprintf(fp, "AIDifficulty=%d\n", config.aiDifficulty);
    fprintf(fp, "BlackPlayer=%s\n", config.playerBlackName);
    fprintf(fp, "WhitePlayer=%s\n", config.playerWhiteName);

    fclose(fp);
}

// 判断是否有气（递归搜索）
int hasLiberty(int x, int y, int color, int visited[BOARD_SIZE][BOARD_SIZE]) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return 0;
    if (visited[x][y]) return 0;

    visited[x][y] = 1;

    if (gameState.board[x][y] == EMPTY) return 1;
    if (gameState.board[x][y] != color) return 0;

    return hasLiberty(x - 1, y, color, visited) ||
        hasLiberty(x + 1, y, color, visited) ||
        hasLiberty(x, y - 1, color, visited) ||
        hasLiberty(x, y + 1, color, visited);
}

// 移除死子（递归清除）
void removeStones(int x, int y, int color) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return;
    if (gameState.board[x][y] != color) return;

    gameState.board[x][y] = EMPTY;

    removeStones(x - 1, y, color);
    removeStones(x + 1, y, color);
    removeStones(x, y - 1, color);
    removeStones(x, y + 1, color);
}

// 检查并提子
int checkCapture(int x, int y, int color) {
    int opponent = (color == BLACK) ? WHITE : BLACK;
    int captured = 0;
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (gameState.board[nx][ny] == opponent) {
                int visited[BOARD_SIZE][BOARD_SIZE] = { 0 };
                if (!hasLiberty(nx, ny, opponent, visited)) {
                    // 计算要提走的子数
                    for (int p = 0; p < BOARD_SIZE; p++) {
                        for (int q = 0; q < BOARD_SIZE; q++) {
                            if (visited[p][q] && gameState.board[p][q] == opponent) {
                                captured++;
                            }
                        }
                    }
                    removeStones(nx, ny, opponent);
                }
            }
        }
    }

    return captured;
}

// 判断合法落子
int isValidMove(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return 0;
    if (gameState.board[x][y] != EMPTY) return 0;

    // 临时放置棋子检查自杀
    gameState.board[x][y] = gameState.currentPlayer;

    // 先检查是否能吃子
    int opponent = (gameState.currentPlayer == BLACK) ? WHITE : BLACK;
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (gameState.board[nx][ny] == opponent) {
                int visited[BOARD_SIZE][BOARD_SIZE] = { 0 };
                if (!hasLiberty(nx, ny, opponent, visited)) {
                    gameState.board[x][y] = EMPTY;
                    return 1; // 能吃子,不是自杀手
                }
            }
        }
    }

    // 检查自己是否有气
    int visited[BOARD_SIZE][BOARD_SIZE] = { 0 };
    int valid = hasLiberty(x, y, gameState.currentPlayer, visited);

    gameState.board[x][y] = EMPTY;
    return valid;
}

// 落子
void placeStone(int x, int y) {
    if (!isValidMove(x, y)) return;

    // 保存历史
    if (historyCount < MAX_HISTORY) {
        history[historyCount].x = x;
        history[historyCount].y = y;
        history[historyCount].player = gameState.currentPlayer;
        memcpy(history[historyCount].boardSnapshot, gameState.board,
            sizeof(gameState.board));
        history[historyCount].timestamp = time(NULL);
        historyCount++;
    }

    gameState.board[x][y] = gameState.currentPlayer;
    int captured = checkCapture(x, y, gameState.currentPlayer);

    gameState.lastCaptureCount = captured;
    history[historyCount - 1].capturedStones = captured;

    if (gameState.currentPlayer == BLACK) {
        gameState.blackCaptures += captured;
    }
    else {
        gameState.whiteCaptures += captured;
    }

    lastMoveX = x;
    lastMoveY = y;
    gameState.moveCount++;
    gameState.currentPlayer = (gameState.currentPlayer == BLACK) ? WHITE : BLACK;
    hintX = hintY = -1;
}

// 悔棋
void undoMove() {
    if (historyCount == 0) {
        MessageBox(GetHWnd(), _T("无法悔棋!"), _T("提示"), MB_OK);
        return;
    }

    historyCount--;
    memcpy(gameState.board, history[historyCount].boardSnapshot,
        sizeof(gameState.board));

    gameState.currentPlayer = history[historyCount].player;
    gameState.moveCount--;

    // 恢复提子数
    gameState.blackCaptures = 0;
    gameState.whiteCaptures = 0;
    for (int i = 0; i < historyCount; i++) {
        if (history[i].player == BLACK) {
            gameState.blackCaptures += history[i].capturedStones;
        }
        else {
            gameState.whiteCaptures += history[i].capturedStones;
        }
    }

    if (historyCount > 0) {
        lastMoveX = history[historyCount - 1].x;
        lastMoveY = history[historyCount - 1].y;
    }
    else {
        lastMoveX = lastMoveY = -1;
    }
}

// 保存游戏
void saveGame(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        MessageBox(GetHWnd(), _T("保存失败!"), _T("错误"), MB_OK);
        return;
    }

    fprintf(fp, "%d %d %d %d %d\n", gameState.currentPlayer,
        gameState.blackCaptures, gameState.whiteCaptures,
        gameState.moveCount, historyCount);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(fp, "%d ", gameState.board[i][j]);
        }
        fprintf(fp, "\n");
    }

    for (int i = 0; i < historyCount; i++) {
        fprintf(fp, "%d %d %d %d\n", history[i].x, history[i].y,
            history[i].player, history[i].capturedStones);
    }

    fclose(fp);
    MessageBox(GetHWnd(), _T("保存成功!"), _T("提示"), MB_OK);
}

// 载入游戏
void loadGame(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        MessageBox(GetHWnd(), _T("未找到存档文件!"), _T("错误"), MB_OK);
        return;
    }

    fscanf(fp, "%d %d %d %d %d", &gameState.currentPlayer,
        &gameState.blackCaptures, &gameState.whiteCaptures,
        &gameState.moveCount, &historyCount);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fscanf(fp, "%d", &gameState.board[i][j]);
        }
    }

    for (int i = 0; i < historyCount; i++) {
        fscanf(fp, "%d %d %d %d", &history[i].x, &history[i].y,
            &history[i].player, &history[i].capturedStones);
    }

    if (historyCount > 0) {
        lastMoveX = history[historyCount - 1].x;
        lastMoveY = history[historyCount - 1].y;
    }

    fclose(fp);
    MessageBox(GetHWnd(), _T("载入成功!"), _T("提示"), MB_OK);
}

// 计算地域（用于点目）
int countTerritory(int x, int y, int* owner, int visited[BOARD_SIZE][BOARD_SIZE]) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return 0;
    if (visited[x][y]) return 0;

    visited[x][y] = 1;

    if (gameState.board[x][y] != EMPTY) {
        if (*owner == EMPTY) {
            *owner = gameState.board[x][y];
        }
        else if (*owner != gameState.board[x][y]) {
            *owner = -1; // 混合地域
        }
        return 0;
    }

    int count = 1;
    count += countTerritory(x - 1, y, owner, visited);
    count += countTerritory(x + 1, y, owner, visited);
    count += countTerritory(x, y - 1, owner, visited);
    count += countTerritory(x, y + 1, owner, visited);

    return count;
}