#include "stdafx.h"
#include "gameMap.h"
#include "pattern.h"
#include "io.h"

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int blackPatternCount[8];
int whitePatternCount[8];

int blackPattern[20][20][4];
int whitePattern[20][20][4];

extern int patternLib[PATTERN_SIZE];

extern Color** map;

extern int boardSize;

int getPatternState(int x, int y, Color selfColor) {
	if (!reachable(x, y)) {
		return STATE_OTHER;
	}
	Color color = map[x][y];
	if (color == NULL_COLOR) {
		return STATE_BLANK;
	}
	return color == selfColor ? STATE_SELF : STATE_OTHER;
}

int* getLineArray(int x, int y, int direct, Color selfColor) {
	int line[8];
	int p = 0;

	int px = x - 4 * directX[direct];
	int py = y - 4 * directY[direct];

	for (int i = 0; i < 4; i++) {
		line[p] = getPatternState(px, py, selfColor);
		p++;
		px += directX[direct];
		py += directY[direct];
	}

	for (int i = 0; i < 4; i++) {
		px += directX[direct];
		py += directY[direct];
		line[p] = getPatternState(px, py, selfColor);
		p++;
	}

	return line;
}

int getPointPattern(int x, int y, int direct, Color selfColor) {
	int* line = getLineArray(x, y, direct, selfColor);
	return patternLib[getLineKey(line)];
}

void updatePointPattern(int x, int y) {
	for (int i = 0; i < 4; i++) {
		blackPattern[x][y][i] = getPointPattern(x, y, i, BLACK);
		whitePattern[x][y][i] = getPointPattern(x, y, i, WHITE);
	}
}

/***************************** ²âÊÔ´úÂë·Ö¸ô ***************************************/

void testPatternRecorder() {
	boardSize = 20;
	initPattern();
	map = readMap();
	printMap(map);
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			updatePointPattern(i, j);
			for (int k = 0; k < 4; k++) {
				if (whitePattern[i][j][k] > 0) {
					printf("whitePattern[%d][%d][%d]=%d\n", i, j, k, whitePattern[i][j][k]);
				}
				if (blackPattern[i][j][k] > 0) {
					printf("blackPattern[%d][%d][%d]=%d\n", i, j, k, blackPattern[i][j][k]);
				}
			}
		}
}