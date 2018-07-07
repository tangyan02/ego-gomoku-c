#include "stdafx.h"
#include "gameMap.h"

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static int blackLineKey[20][20][4];
static int whiteLineKey[20][20][4];

int blackPatternCount[8];
int whitePatternCount[8];

int blackPattern[20][20][4];
int whitePattern[20][20][4];

extern Color** map;

extern int getLineKey(int line[]);

int getMapLineKey(int x, int y, int direct, Color targetColor) {
	if (targetColor == BLACK)
		return blackLineKey[x][y][direct];
	else
		return whiteLineKey[x][y][direct];
}

int getPointTableColor(int x, int y, Color targetColor) {
	if (!reachable(x, y))
		return INVALID_COLOR;
	if (map[x][y] == NULL)
		return NULL_COLOR;
	if (map[x][y] == targetColor)
		return CURRENT_COLOR;
	if (map[x][y] == getOtherColor(targetColor))
		return OTHER_COLOR;
	return 0;
}

int updateLineKey(int x, int y, int direct, int n, Color targetColor) {
	int px = x + (n - 4)*directX[direct];
	int py = y + (n - 4)*directY[direct];
	int tableColor = getPointTableColor(x, y, targetColor);

	if (n >= 4)
		n--;
	int t = 3 << (n * 2);
	t = t ^ ((1 << 16) - 1);
	int key;
	if (targetColor == WHITE)
		key = whiteLineKey[px][py][direct];
	else
		key = blackLineKey[px][py][direct];


	key &= t;
	tableColor = tableColor << (n * 2);
	key |= tableColor;
	if (targetColor == WHITE)
		whiteLineKey[px][py][direct] = key;
	else
		blackLineKey[px][py][direct] = key;
	return key;
}

void setLineKey(int px, int py, int direct, Color targetColor) {
	int line[8];
	int x = px - 4 * directX[direct];
	int y = py - 4 * directY[direct];
	for (int i = 0; i < 4; i++) {
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
		x += directX[direct];
		y += directY[direct];
	}

	for (int i = 4; i < 8; i++) {
		x += directX[direct];
		y += directY[direct];
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
	}
	if (targetColor == BLACK)
		blackLineKey[px][py][direct] = getLineKey(line);
	if (targetColor == WHITE)
		whiteLineKey[px][py][direct] = getLineKey(line);
}

void updatePointKey(int px, int py) {
	for (int i = 0; i < 4; i++) {
		int x = px - 5 * directX[i];
		int y = py - 5 * directY[i];
		for (int k = 0; k <= 8; k++) {
			x += directX[i];
			y += directY[i];
			if (!reachable(x, y)) {
				continue;
			}
			if (k == 4)
				continue;
			updateLineKey(px, py, i, k, BLACK);
			updateLineKey(px, py, i, k, WHITE);
		}
	}
}