/*
 * 围棋游戏系统 - Part 3: AI与菜单模块
 * 负责人: 2518801370 李卓洵
 * 实现: AI算法、主菜单、菜单交互、计分系统
 */

#include "Part1_Core.h"

 // 评估位置价值
int evaluatePosition(int x, int y) {
    int score = 0;
    int color = gameState.currentPlayer;

    // 基础位置价值
    int centerX = BOARD_SIZE / 2;
    int centerY = BOARD_SIZE / 2;
    int distToCenter = abs(x - centerX) + abs(y - centerY);

    // 角部价值最高
    if ((x <= 3 || x >= 15) && (y <= 3 || y >= 15)) {
        score += 25;
    }
    // 边部次之
    else if (x <= 3 || x >= 15 || y <= 3 || y >= 15) {
        score += 15;
    }
    // 中腹
    else {
        score += 10 - distToCenter / 2;
    }

    // 星位加分
    int starPoints[9][2] = {
        {3, 3}, {3, 9}, {3, 15},
        {9, 3}, {9, 9}, {9, 15},
        {15, 3}, {15, 9}, {15, 15}
    };
    for (int i = 0; i < 9; i++) {
        if (x == starPoints[i][0] && y == starPoints[i][1]) {
            score += 10;
            break;
        }
    }

    // 检查周围棋子
    int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    int friendCount = 0;
    int enemyCount = 0;
    int opponent = (color == BLACK) ? WHITE : BLACK;

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (gameState.board[nx][ny] == color) {
                friendCount++;
                score += 5;
            }
            else if (gameState.board[nx][ny] == opponent) {
                enemyCount++;
                score += 3;
            }
        }
    }

    // 检查是否能吃子
    gameState.board[x][y] = color;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (gameState.board[nx][ny] == opponent) {
                int visited[BOARD_SIZE][BOARD_SIZE] = { 0 };
                if (!hasLiberty(nx, ny, opponent, visited)) {
                    // 计算能吃的子数
                    int captureCount = 0;
                    for (int p = 0; p < BOARD_SIZE; p++) {
                        for (int q = 0; q < BOARD_SIZE; q++) {
                            if (visited[p][q]) captureCount++;
                        }
                    }
                    score += 40 * captureCount; // 吃子大幅加分
                }
            }
        }
    }

    // 检查自己的气
    int libertyCount = 0;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (gameState.board[nx][ny] == EMPTY) {
                libertyCount++;
            }
        }
    }
    score += libertyCount * 3;

    gameState.board[x][y] = EMPTY;

    // 根据难度调整
    if (config.aiDifficulty == 1) {
        score += rand() % 20; // 简单模式增加随机性
    }
    else if (config.aiDifficulty == 3) {
        // 困难模式考虑更多因素
        score += (friendCount - enemyCount) * 2;
    }

    return score;
}

// AI落子
void getAIMove(int* x, int* y) {
    int bestScore = -1;
    int candidates[BOARD_SIZE * BOARD_SIZE][3];
    int candidateCount = 0;

    // 评估所有合法位置
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (isValidMove(i, j)) {
                int score = evaluatePosition(i, j);
                candidates[candidateCount][0] = i;
                candidates[candidateCount][1] = j;
                candidates[candidateCount][2] = score;
                candidateCount++;

                if (score > bestScore) {
                    bestScore = score;
                }
            }
        }
    }

    // 在最优解中随机选择
    if (candidateCount > 0) {
        int threshold = bestScore - (10 - config.aiDifficulty * 3);
        int goodMoves[BOARD_SIZE * BOARD_SIZE][2];
        int goodCount = 0;

        for (int i = 0; i < candidateCount; i++) {
            if (candidates[i][2] >= threshold) {
                goodMoves[goodCount][0] = candidates[i][0];
                goodMoves[goodCount][1] = candidates[i][1];
                goodCount++;
            }
        }

        if (goodCount > 0) {
            int choice = rand() % goodCount;
            *x = goodMoves[choice][0];
            *y = goodMoves[choice][1];
        }
        else {
            *x = candidates[0][0];
            *y = candidates[0][1];
        }
    }
    else {
        *x = *y = -1;
    }
}

// 计算目数
void calculateScore() {
    int blackStones = 0, whiteStones = 0;
    int blackTerritory = 0, whiteTerritory = 0;

    // 统计棋子数
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState.board[i][j] == BLACK) {
                blackStones++;
            }
            else if (gameState.board[i][j] == WHITE) {
                whiteStones++;
            }
        }
    }

    // 简单的地域统计
    int visited[BOARD_SIZE][BOARD_SIZE] = { 0 };
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState.board[i][j] == EMPTY && !visited[i][j]) {
                int owner = EMPTY;
                int territory = countTerritory(i, j, &owner, visited);
                if (owner == BLACK) {
                    blackTerritory += territory;
                }
                else if (owner == WHITE) {
                    whiteTerritory += territory;
                }
            }
        }
    }

    float blackScore = (float)(blackStones + blackTerritory) + gameState.blackCaptures;
    float whiteScore = (float)(whiteStones + whiteTerritory) + gameState.whiteCaptures + config.komi;

    TCHAR msg[400];
    _stprintf(msg, _T("目数统计:\n\n黑方: %d子 + %d地 + %d提子 = %.1f目\n白方: %d子 + %d地 + %d提子 + %.1f贴目 = %.1f目\n\n%s胜 %.1f目"),
        blackStones, blackTerritory, gameState.blackCaptures, blackScore,
        whiteStones, whiteTerritory, gameState.whiteCaptures, config.komi, whiteScore,
        blackScore > whiteScore ? _T("黑方") : _T("白方"),
        (float)fabs(blackScore - whiteScore));

    MessageBox(GetHWnd(), msg, _T("对局结果"), MB_OK);
}

// 主菜单
void showMainMenu() {
    cleardevice();

    // 渐变背景
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        int r = 220 - y / 15;
        int g = 179 - y / 20;
        int b = 92 - y / 25;
        if (r < 180) r = 180;
        if (g < 140) g = 140;
        if (b < 60) b = 60;
        setlinecolor(RGB(r, g, b));
        line(0, y, WINDOW_WIDTH, y);
    }

    setbkmode(TRANSPARENT);
    settextcolor(RGB(139, 90, 43));

    // 标题
    settextstyle(50, 0, _T("楷体"));
    TCHAR title[] = _T("围 棋 游 戏");
    outtextxy(320, 80, title);

    settextstyle(16, 0, _T("宋体"));

    // 菜单按钮
    setlinecolor(RGB(139, 90, 43));
    setlinestyle(PS_SOLID, 3);

    int buttonX = 340;
    int buttonW = 220;
    int startY = 200;
    int buttonH = 50;
    int gap = 20;

    const TCHAR* menuItems[] = {
        _T("人人对战 [1]"),
        _T("人机对战 [2]"),
        _T("载入游戏 [3]"),
        _T("游戏说明 [4]"),
        _T("退出 [ESC]")
    };

    for (int i = 0; i < 5; i++) {
        int y = startY + i * (buttonH + gap);

        setfillcolor(RGB(200, 150, 80));
        fillroundrect(buttonX, y, buttonX + buttonW, y + buttonH, 10, 10);
        setfillcolor(RGB(240, 200, 120));
        fillroundrect(buttonX + 2, y + 2, buttonX + buttonW - 2, y + buttonH - 2, 8, 8);

        settextcolor(RGB(80, 50, 20));
        int textX = buttonX + (buttonW - textwidth((TCHAR*)menuItems[i])) / 2;
        outtextxy(textX, y + 15, (TCHAR*)menuItems[i]);
    }

    // 制作者信息
    settextstyle(13, 0, _T("宋体"));
    settextcolor(RGB(100, 60, 20));
    outtextxy(320, 600, _T("制作者: 技术科学试验班"));
    outtextxy(280, 620, _T("251880102 忽哲    251880599 唐翊添"));
    outtextxy(280, 640, _T("2518801370 李卓洵  251880107 马耀宗"));
}

// 处理菜单点击
void handleMenuClick(int x, int y) {
    int buttonX = 340;
    int buttonW = 220;
    int startY = 200;
    int buttonH = 50;
    int gap = 20;

    if (x >= buttonX && x <= buttonX + buttonW) {
        for (int i = 0; i < 5; i++) {
            int btnY = startY + i * (buttonH + gap);
            if (y >= btnY && y <= btnY + buttonH) {
                switch (i) {
                case 0: // 人人对战
                    gameMode = 1;
                    initGame();
                    drawBoard();
                    break;
                case 1: // 人机对战
                    gameMode = 2;
                    initGame();
                    drawBoard();
                    break;
                case 2: // 载入游戏
                    loadGame("savegame.txt");
                    gameMode = 1;
                    drawBoard();
                    break;
                case 3: // 游戏说明
                    MessageBox(GetHWnd(),
                        _T("围棋规则:\n\n1. 黑白双方轮流在交叉点上落子\n2. 被包围无气的棋子会被提走\n3. 不能下自杀手(除非能吃掉对方棋子)\n4. 全局同形禁止(打劫)\n5. 最终按目数+提子数计算胜负\n6. 白方有7.5目贴目\n\n快捷键:\nU-悔棋 S-保存 L-载入\nH-提示 C-计算 ESC-菜单\n\n难度设置: 在config.txt中修改AIDifficulty\n1-简单 2-中等 3-困难"),
                        _T("游戏说明"), MB_OK);
                    break;
                case 4: // 退出
                    exit(0);
                    break;
                }
                break;
            }
        }
    }
}