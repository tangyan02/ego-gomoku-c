#include "stdafx.h"
#include "winChecker.h"
#include "point.h"
#include "config.h"

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

extern int boardSize;

static bool reachable(int x, int y) {
    if (x < 0 || x >= boardSize)
        return false;
    if (y < 0 || y >= boardSize)
        return false;
    return true;
}

static bool checkColors(Color color, point point, int direct, int start, int end, Color** map) {
    int x = point.x + start * (directX[direct]);
    int y = point.y + start * (directY[direct]);
    for (int i = start; i <= end; i++) {
        if (!reachable(x, y)) {
            return false;
        }
        if (map[x][y] != color) {
            return false;
        }
        x += directX[direct];
        y += directY[direct];
    }
    return true;
}

Color win(Color** map)
{
    for (int i = 0; i < boardSize; i++)
        for (int j = 0; j < boardSize; j++) {
            point point(i, j);
            Color color = map[i][j];
            if (map[i][j] != NULL) {
                for (int direct = 0; direct < 4; direct++) {
                    if (checkColors(color, point, direct, 0, 4, map)) {
                        return color;
                    }
                }
            }
        }
    return NULL;
}