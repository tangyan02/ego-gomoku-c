#include "stdafx.h"
#include "gameMap.h"
#include "pattern.h"
#include "io.h"
#include "functional"

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int blackPatternCount[10];
int whitePatternCount[10];

int blackPatternCountInNull[10];
int whitePatternCountInNull[10];

int blackPattern[20][20][4];
int whitePattern[20][20][4];

extern int patternLib[PATTERN_SIZE];

extern Color** map;

extern int boardSize;


void clearPatternRecord() {
	for (int i = 0; i < 10; i++) {
		blackPatternCount[i] = 0;
		whitePatternCount[i] = 0;
		blackPatternCountInNull[i] = 0;
		whitePatternCountInNull[i] = 0;
	}
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			for (int k = 0; k < 4; k++) {
				blackPattern[20][20][4] = PATTERN_NULL;
				whitePattern[20][20][4] = PATTERN_NULL;
			}
}

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

int buildLineAndGetKey(int line[], int x, int y, int direct, Color selfColor) {
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

	return getLineKey(line);
}

void addPointDirectPatternCount(int x, int y, int direct) {
	if (map[x][y] == BLACK) {
		blackPatternCount[blackPattern[x][y][direct]]++;
	}
	if (map[x][y] == WHITE) {
		whitePatternCount[whitePattern[x][y][direct]]++;
	}
	if (map[x][y] == NULL) {
		blackPatternCountInNull[blackPattern[x][y][direct]]++;
		whitePatternCountInNull[whitePattern[x][y][direct]]++;
	}
}

void removePointDirectPatternCount(int x, int y, int direct) {
	if (map[x][y] == BLACK) {
		blackPatternCount[blackPattern[x][y][direct]]--;
	}
	if (map[x][y] == WHITE) {
		whitePatternCount[whitePattern[x][y][direct]]--;
	}
	if (map[x][y] == NULL) {
		blackPatternCountInNull[blackPattern[x][y][direct]]--;
		whitePatternCountInNull[whitePattern[x][y][direct]]--;
	}
}

void updatePointPattern(int x, int y) {
	int line[8];
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				int blackKey = buildLineAndGetKey(line, px, py, direct, BLACK);
				int whiteKey = buildLineAndGetKey(line, px, py, direct, WHITE);
				blackPattern[px][py][direct] = patternLib[blackKey];
				whitePattern[px][py][direct] = patternLib[whiteKey];
			}
			px += directX[direct];
			py += directY[direct];
		}
	}
}

void removeLinePatternCount(int x, int y) {
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				removePointDirectPatternCount(px, py, direct);
			}
			px += directX[direct];
			py += directY[direct];
		}
	}
}

void addLinePatternCount(int x, int y) {
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				addPointDirectPatternCount(px, py, direct);
			}
			px += directX[direct];
			py += directY[direct];
		}
	}
}

void printPatternAnalyze() {
	printMap(getMap());
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++) {
			for (int k = 0; k < 4; k++) {
				if (whitePattern[i][j][k] != PATTERN_NULL) {
					printf("whitePattern[%d][%d][%d]=%d\n", i, j, k, whitePattern[i][j][k]);
				}
				if (blackPattern[i][j][k] != PATTERN_NULL) {
					printf("blackPattern[%d][%d][%d]=%d\n", i, j, k, blackPattern[i][j][k]);
				}
			}
		}
	for (int i = 0; i < 8; i++) {
		printf("blackPatternCount[%d]=%d\n", i, blackPatternCount[i]);
		printf("whitePatternCount[%d]=%d\n", i, whitePatternCount[i]);
	}
}

/***************************** ²âÊÔ´úÂë·Ö¸ô ***************************************/

void testPatternRecorder() {
	boardSize = 20;
	initPattern();
	map = readMap("patternRecorder.txt");
	initGameMap(map);

	move(1, 1, BLACK, WHITE);
	move(1, 3, BLACK, WHITE);
	move(0, 1, WHITE, WHITE);
	undoMove(0, 1, WHITE, WHITE);
	printPatternAnalyze();

}