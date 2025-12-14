/*
 * 围棋游戏系统 - Part 4: 交互控制与主函数
 * 负责人: 251880107 马耀宗
 * 实现: 鼠标交互、键盘控制、棋谱导出、主程序循环
 */

#include "Part1_Core.h"

 // 处理鼠标点击
void handleClick(int mouseX, int mouseY) {
    int uiX = BOARD_MARGIN + BOARD_SIZE * CELL_SIZE + 40;

    // 检查UI按钮
    if (mouseX >= uiX && mouseX <= uiX + 160) {
        int buttonY = 360;
        int buttonH = 38;
        int buttonGap = 8;

        for (int i = 0; i < 6; i++) {
            int y = buttonY + i * (buttonH + buttonGap);
            if (mouseY >= y && mouseY <= y + buttonH) {
                switch (i) {
                case 0: // 悔棋
                    undoMove();
                    drawBoard();
                    break;
                case 1: // 保存游戏
                    saveGame("savegame.txt");
                    break;
                case 2: // 载入游戏
                    loadGame("savegame.txt");
                    drawBoard();
                    break;
                case 3: // AI提示
                    getAIMove(&hintX, &hintY);
                    drawBoard();
                    break;
                case 4: // 计算目数
                    calculateScore();
                    break;
                case 5: // 返回菜单
                    gameMode = 0;
                    showMainMenu();
                    break;
                }
                return;
            }
        }
    }

    // 棋盘落子
    int x = (mouseX - BOARD_MARGIN + CELL_SIZE / 2) / CELL_SIZE;
    int y = (mouseY - BOARD_MARGIN + CELL_SIZE / 2) / CELL_SIZE;

    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (isValidMove(x, y)) {
            placeStone(x, y);

            // AI自动下棋
            if (gameMode == 2 && gameState.currentPlayer == WHITE) {
                Sleep(500); // 延迟，模拟思考
                int aiX, aiY;
                getAIMove(&aiX, &aiY);
                if (aiX >= 0 && aiY >= 0) {
                    placeStone(aiX, aiY);
                }
            }

            drawBoard();
        }
    }
}

// 处理键盘输入
void handleKeyboard() {
    if (_kbhit()) {
        char ch = _getch();

        if (gameMode == 0) {
            // 主菜单按键
            switch (ch) {
            case '1':
                gameMode = 1;
                initGame();
                drawBoard();
                break;
            case '2':
                gameMode = 2;
                initGame();
                drawBoard();
                break;
            case '3':
                loadGame("savegame.txt");
                gameMode = 1;
                drawBoard();
                break;
            case '4':
                MessageBox(GetHWnd(),
                    _T("围棋规则:\n\n1. 黑白双方轮流在交叉点上落子\n2. 被包围无气的棋子会被提走\n3. 不能下自杀手(除非能吃掉对方棋子)\n4. 全局同形禁止(打劫)\n5. 最终按目数+提子数计算胜负\n6. 白方有7.5目贴目\n\n快捷键:\nU-悔棋 S-保存 L-载入\nH-提示 C-计算 ESC-菜单\n\n难度设置: 在config.txt中修改AIDifficulty\n1-简单 2-中等 3-困难"),
                    _T("游戏说明"), MB_OK);
                break;
            case 27: // ESC
                exit(0);
                break;
            }
        }
        else {
            // 游戏中按键
            switch (ch) {
            case 'u':
            case 'U':
                undoMove();
                drawBoard();
                break;
            case 's':
            case 'S':
                saveGame("savegame.txt");
                break;
            case 'l':
            case 'L':
                loadGame("savegame.txt");
                drawBoard();
                break;
            case 'h':
            case 'H':
                getAIMove(&hintX, &hintY);
                drawBoard();
                break;
            case 'c':
            case 'C':
                calculateScore();
                break;
            case 'e':
            case 'E':
                exportGameRecord("game_record.txt");
                MessageBox(GetHWnd(), _T("棋谱已导出到 game_record.txt"), _T("提示"), MB_OK);
                break;
            case 27: // ESC
                gameMode = 0;
                showMainMenu();
                break;
            }
        }
    }
}

// 导出棋谱
void exportGameRecord(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        MessageBox(GetHWnd(), _T("导出失败!"), _T("错误"), MB_OK);
        return;
    }

    // 写入标题信息
    fprintf(fp, "=========================================\n");
    fprintf(fp, "           围棋对局棋谱记录\n");
    fprintf(fp, "=========================================\n\n");
    fprintf(fp, "黑方: %s\n", config.playerBlackName);
    fprintf(fp, "白方: %s\n", config.playerWhiteName);
    fprintf(fp, "贴目: %.1f\n", config.komi);
    fprintf(fp, "总手数: %d\n", historyCount);
    fprintf(fp, "日期: %s\n", ctime(&history[0].timestamp));
    fprintf(fp, "\n=========================================\n");
    fprintf(fp, "手数  执子  位置     手数  执子  位置\n");
    fprintf(fp, "=========================================\n");

    // 写入着法记录（两列显示）
    for (int i = 0; i < historyCount; i += 2) {
        // 第一列
        char col1 = history[i].x < 8 ? 'A' + history[i].x : 'A' + history[i].x + 1;
        int row1 = BOARD_SIZE - history[i].y;
        fprintf(fp, "%-6d%-6s%c%-8d",
            i + 1,
            history[i].player == BLACK ? "黑" : "白",
            col1, row1);

        // 第二列（如果存在）
        if (i + 1 < historyCount) {
            char col2 = history[i + 1].x < 8 ? 'A' + history[i + 1].x : 'A' + history[i + 1].x + 1;
            int row2 = BOARD_SIZE - history[i + 1].y;
            fprintf(fp, "%-6d%-6s%c%-8d",
                i + 2,
                history[i + 1].player == BLACK ? "黑" : "白",
                col2, row2);
        }
        fprintf(fp, "\n");
    }

    // 写入统计信息
    fprintf(fp, "\n=========================================\n");
    fprintf(fp, "对局统计\n");
    fprintf(fp, "=========================================\n");
    fprintf(fp, "黑方提子数: %d\n", gameState.blackCaptures);
    fprintf(fp, "白方提子数: %d\n", gameState.whiteCaptures);

    // 统计棋子数
    int blackStones = 0, whiteStones = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState.board[i][j] == BLACK) blackStones++;
            else if (gameState.board[i][j] == WHITE) whiteStones++;
        }
    }
    fprintf(fp, "黑方棋子数: %d\n", blackStones);
    fprintf(fp, "白方棋子数: %d\n", whiteStones);
    fprintf(fp, "\n=========================================\n");
    fprintf(fp, "制作: 技术科学试验班围棋项目组\n");
    fprintf(fp, "      251880102 忽哲    251880599 唐翊添\n");
    fprintf(fp, "      2518801370 李卓洵  251880107 马耀宗\n");
    fprintf(fp, "=========================================\n");

    fclose(fp);
}

// 主函数
int main() {
    // 初始化随机数种子
    srand((unsigned)time(NULL));

    // 初始化图形窗口
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(WHITE);
    cleardevice();

    // 设置窗口标题
    SetWindowText(GetHWnd(), _T("围棋游戏系统 v2.0"));

    // 加载资源
    loadImages();
    initGame();
    showMainMenu();

    // 主循环
    ExMessage msg;
    while (true) {
        // 处理鼠标消息
        if (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                if (gameMode == 0) {
                    handleMenuClick(msg.x, msg.y);
                }
                else {
                    handleClick(msg.x, msg.y);
                }
            }
        }

        // 处理键盘输入
        handleKeyboard();

        // 延时，减少CPU占用
        Sleep(10);
    }

    // 关闭图形窗口
    closegraph();
    return 0;
}