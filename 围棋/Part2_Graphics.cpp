/*
 * 围棋游戏系统 - Part 2: 图形渲染模块
 * 负责人: 251880599 唐翊添
 * 实现: 棋盘绘制、棋子绘制、UI界面、动画效果
 */

#include "Part1_Core.h"

 // 加载图片资源
void loadImages() {
    if (_access("board.png", 0) == 0) {
        loadimage(&imgBoard, _T("board.png"));
    }
    if (_access("white_stone.png", 0) == 0) {
        loadimage(&imgWhiteStone, _T("white_stone.png"), STONE_RADIUS * 2, STONE_RADIUS * 2);
        imagesLoaded |= 1;
    }
    if (_access("black_stone.png", 0) == 0) {
        loadimage(&imgBlackStone, _T("black_stone.png"), STONE_RADIUS * 2, STONE_RADIUS * 2);
        imagesLoaded |= 2;
    }
}

// 绘制立体白色棋子
void drawWhiteStone(int px, int py) {
    // 底部阴影
    setfillcolor(RGB(180, 180, 180));
    solidcircle(px + 2, py + 2, STONE_RADIUS);

    // 主体渐变
    for (int i = STONE_RADIUS; i > 0; i--) {
        int gray = 245 - (STONE_RADIUS - i) * 5;
        if (gray < 200) gray = 200;
        setfillcolor(RGB(gray, gray, gray));
        solidcircle(px, py, i);
    }

    // 高光效果
    setfillcolor(RGB(255, 255, 255));
    solidcircle(px - 4, py - 4, 5);
    setfillcolor(RGB(250, 250, 250));
    solidcircle(px - 3, py - 3, 3);

    // 边缘描边
    setlinecolor(RGB(160, 160, 160));
    setlinestyle(PS_SOLID, 2);
    circle(px, py, STONE_RADIUS);
}

// 绘制立体黑色棋子
void drawBlackStone(int px, int py) {
    // 底部阴影
    setfillcolor(RGB(0, 0, 0));
    solidcircle(px + 2, py + 2, STONE_RADIUS);

    // 主体渐变
    for (int i = STONE_RADIUS; i > 0; i--) {
        int gray = 30 + (STONE_RADIUS - i) * 3;
        if (gray > 60) gray = 60;
        setfillcolor(RGB(gray, gray, gray));
        solidcircle(px, py, i);
    }

    // 高光效果
    setfillcolor(RGB(120, 120, 120));
    solidcircle(px - 5, py - 5, 4);
    setfillcolor(RGB(90, 90, 90));
    solidcircle(px - 4, py - 4, 2);

    // 边缘高光弧
    setlinecolor(RGB(80, 80, 80));
    setlinestyle(PS_SOLID, 2);
    arc(px - STONE_RADIUS, py - STONE_RADIUS,
        px + STONE_RADIUS, py + STONE_RADIUS, 3.5, 2.5);

    // 外边框
    setlinecolor(RGB(0, 0, 0));
    setlinestyle(PS_SOLID, 2);
    circle(px, py, STONE_RADIUS);
}

// 使用图片或代码绘制棋子
void drawStoneWithImage(int x, int y, int color) {
    int px = BOARD_MARGIN + x * CELL_SIZE;
    int py = BOARD_MARGIN + y * CELL_SIZE;

    if (color == BLACK) {
        if (imagesLoaded & 2) {
            putimage(px - STONE_RADIUS, py - STONE_RADIUS, &imgBlackStone);
        }
        else {
            drawBlackStone(px, py);
        }
    }
    else {
        if (imagesLoaded & 1) {
            putimage(px - STONE_RADIUS, py - STONE_RADIUS, &imgWhiteStone);
        }
        else {
            drawWhiteStone(px, py);
        }
    }

    // 落子动画效果
    if (config.enableAnimation && x == lastMoveX && y == lastMoveY) {
        for (int i = 0; i < 3; i++) {
            setlinecolor(RGB(255 - i * 50, 0, 0));
            setlinestyle(PS_SOLID, 3 - i);
            circle(px, py, STONE_RADIUS + 5 + i * 2);
        }
    }
}

// 绘制棋盘
void drawBoard() {
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

    // 棋盘外框装饰
    setlinecolor(RGB(139, 90, 43));
    setlinestyle(PS_SOLID, 6);
    rectangle(BOARD_MARGIN - 15, BOARD_MARGIN - 15,
        BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 15,
        BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 15);

    setlinestyle(PS_SOLID, 2);
    rectangle(BOARD_MARGIN - 12, BOARD_MARGIN - 12,
        BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 12,
        BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 12);

    // 绘制网格
    setlinecolor(RGB(0, 0, 0));
    setlinestyle(PS_SOLID, 1);

    for (int i = 0; i < BOARD_SIZE; i++) {
        int pos = BOARD_MARGIN + i * CELL_SIZE;
        line(BOARD_MARGIN, pos,
            BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, pos);
        line(pos, BOARD_MARGIN,
            pos, BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
    }

    // 绘制星位
    int starPoints[9][2] = {
        {3, 3}, {3, 9}, {3, 15},
        {9, 3}, {9, 9}, {9, 15},
        {15, 3}, {15, 9}, {15, 15}
    };

    for (int i = 0; i < 9; i++) {
        int x = BOARD_MARGIN + starPoints[i][0] * CELL_SIZE;
        int y = BOARD_MARGIN + starPoints[i][1] * CELL_SIZE;
        setfillcolor(RGB(0, 0, 0));
        solidcircle(x, y, 5);
        setfillcolor(RGB(100, 100, 100));
        solidcircle(x - 1, y - 1, 3);
    }

    // 绘制坐标标记
    setbkmode(TRANSPARENT);
    settextcolor(RGB(80, 50, 20));
    settextstyle(16, 0, _T("Arial"));

    for (int i = 0; i < BOARD_SIZE; i++) {
        TCHAR label[3];
        // 横坐标 A-T (跳过I)
        _stprintf(label, _T("%c"), i < 8 ? 'A' + i : 'A' + i + 1);
        outtextxy(BOARD_MARGIN + i * CELL_SIZE - 5, BOARD_MARGIN - 30, label);
        outtextxy(BOARD_MARGIN + i * CELL_SIZE - 5,
            BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 18, label);

        // 纵坐标 1-19
        _stprintf(label, _T("%d"), BOARD_SIZE - i);
        int offset = (BOARD_SIZE - i) >= 10 ? 30 : 25;
        outtextxy(BOARD_MARGIN - offset, BOARD_MARGIN + i * CELL_SIZE - 8, label);
        outtextxy(BOARD_MARGIN + (BOARD_SIZE - 1) * CELL_SIZE + 18,
            BOARD_MARGIN + i * CELL_SIZE - 8, label);
    }

    // 绘制棋子
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState.board[i][j] != EMPTY) {
                drawStoneWithImage(i, j, gameState.board[i][j]);
            }
        }
    }

    // 标记最后一手
    if (lastMoveX >= 0 && lastMoveY >= 0) {
        int x = BOARD_MARGIN + lastMoveX * CELL_SIZE;
        int y = BOARD_MARGIN + lastMoveY * CELL_SIZE;

        setlinecolor(RGB(255, 50, 50));
        setlinestyle(PS_SOLID, 3);
        circle(x, y, STONE_RADIUS + 5);

        setlinecolor(RGB(255, 100, 100));
        setlinestyle(PS_SOLID, 2);
        circle(x, y, STONE_RADIUS + 3);
    }

    // 显示提示位置
    if (hintX >= 0 && hintY >= 0) {
        int x = BOARD_MARGIN + hintX * CELL_SIZE;
        int y = BOARD_MARGIN + hintY * CELL_SIZE;

        for (int i = 0; i < 2; i++) {
            setlinecolor(RGB(0, 255 - i * 100, 0));
            setlinestyle(PS_SOLID, 3 - i);
            rectangle(x - 10 - i * 2, y - 10 - i * 2,
                x + 10 + i * 2, y + 10 + i * 2);
        }
    }

    drawUI();
}

// 绘制UI信息
void drawUI() {
    int uiX = BOARD_MARGIN + BOARD_SIZE * CELL_SIZE + 40;

    setbkmode(TRANSPARENT);

    // 标题框
    setfillcolor(RGB(139, 90, 43));
    fillroundrect(uiX - 10, 30, uiX + 190, 90, 15, 15);
    setfillcolor(RGB(240, 200, 120));
    fillroundrect(uiX - 8, 32, uiX + 188, 88, 12, 12);

    settextcolor(RGB(139, 90, 43));
    settextstyle(28, 0, _T("楷体"));
    TCHAR info[200];
    _stprintf(info, _T("围棋游戏系统"));
    outtextxy(uiX + 5, 45, info);

    // 制作者信息框
    settextstyle(14, 0, _T("宋体"));
    settextcolor(RGB(80, 50, 20));
    _stprintf(info, _T("制作者信息:"));
    outtextxy(uiX, 105, info);
    settextstyle(12, 0, _T("宋体"));
    _stprintf(info, _T("251880102 忽哲"));
    outtextxy(uiX + 5, 125, info);
    _stprintf(info, _T("251880599 唐翊添"));
    outtextxy(uiX + 5, 143, info);
    _stprintf(info, _T("2518801370 李卓洵"));
    outtextxy(uiX + 5, 161, info);
    _stprintf(info, _T("251880107 马耀宗"));
    outtextxy(uiX + 5, 179, info);

    // 游戏信息框
    settextstyle(16, 0, _T("宋体"));
    settextcolor(RGB(0, 0, 0));

    setfillcolor(RGB(200, 160, 90));
    fillroundrect(uiX - 10, 200, uiX + 190, 340, 10, 10);
    setfillcolor(RGB(250, 220, 170));
    fillroundrect(uiX - 8, 202, uiX + 188, 338, 8, 8);

    _stprintf(info, _T("当前执子: %s"),
        gameState.currentPlayer == BLACK ? _T("●黑方") : _T("○白方"));
    outtextxy(uiX + 10, 215, info);

    _stprintf(info, _T("手数: %d"), gameState.moveCount);
    outtextxy(uiX + 10, 245, info);

    _stprintf(info, _T("黑方提子: %d"), gameState.blackCaptures);
    outtextxy(uiX + 10, 275, info);

    _stprintf(info, _T("白方提子: %d"), gameState.whiteCaptures);
    outtextxy(uiX + 10, 305, info);

    // 绘制按钮
    settextstyle(15, 0, _T("宋体"));
    setlinecolor(RGB(139, 90, 43));
    setlinestyle(PS_SOLID, 2);

    int buttonY = 360;
    int buttonH = 38;
    int buttonGap = 8;

    const TCHAR* buttons[] = {
        _T("悔棋 [U]"),
        _T("保存游戏 [S]"),
        _T("载入游戏 [L]"),
        _T("AI提示 [H]"),
        _T("计算目数 [C]"),
        _T("返回菜单 [ESC]")
    };

    for (int i = 0; i < 6; i++) {
        int y = buttonY + i * (buttonH + buttonGap);

        setfillcolor(RGB(200, 150, 80));
        fillroundrect(uiX, y, uiX + 160, y + buttonH, 8, 8);
        setfillcolor(RGB(240, 200, 120));
        fillroundrect(uiX + 2, y + 2, uiX + 158, y + buttonH - 2, 6, 6);

        settextcolor(RGB(80, 50, 20));
        int textX = (i == 1 || i == 2 || i == 4) ? 20 : 30;
        outtextxy(uiX + textX, y + 11, (TCHAR*)buttons[i]);
    }
}