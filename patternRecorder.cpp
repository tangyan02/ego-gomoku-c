#include "stdafx.h"
#include "gameMap.h"
#include "pattern.h"
#include "io.h"
#include "functional"

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int blackPatternTotalScore;
int whitePatternTotalScore;

int blackPatternCount[10];
int whitePatternCount[10];

int blackPatternCountInNull[10];
int whitePatternCountInNull[10];

int blackPattern[20][20][4];
int whitePattern[20][20][4];

int blackLineKey[20][20][4];
int whiteLineKey[20][20][4];

int keyFullWithBlank[8];

extern int patternLib[PATTERN_SIZE];
extern int patternScore[PATTERN_SIZE];

extern Color** map;

extern int boardSize;

void clearPatternRecord() {
	blackPatternTotalScore = 0;
	whitePatternTotalScore = 0;
	for (int i = 0; i < 10; i++) {
		blackPatternCount[i] = 0;
		whitePatternCount[i] = 0;
		blackPatternCountInNull[i] = 0;
		whitePatternCountInNull[i] = 0;
	}
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			for (int k = 0; k < 4; k++) {
				blackPattern[i][j][k] = PATTERN_NULL;
				whitePattern[i][j][k] = PATTERN_NULL;
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

static void addPointDirectPatternCount(int x, int y, int direct) {
	if (map[x][y] == BLACK) {
		blackPatternCount[blackPattern[x][y][direct]]++;
		blackPatternTotalScore += patternScore[blackLineKey[x][y][direct]];
	} 
	if (map[x][y] == WHITE) {
		whitePatternCount[whitePattern[x][y][direct]]++;
		whitePatternTotalScore += patternScore[whiteLineKey[x][y][direct]];
	} 
	if (map[x][y] == NULL) {
		blackPatternCountInNull[blackPattern[x][y][direct]]++;
		whitePatternCountInNull[whitePattern[x][y][direct]]++;
	}
}

static void removePointDirectPatternCount(int x, int y, int direct) {
	if (map[x][y] == BLACK) {
		blackPatternCount[blackPattern[x][y][direct]]--;
		blackPatternTotalScore -= patternScore[blackLineKey[x][y][direct]];
	}
	if (map[x][y] == WHITE) {
		whitePatternCount[whitePattern[x][y][direct]]--;
		whitePatternTotalScore -= patternScore[whiteLineKey[x][y][direct]];
	}
	if (map[x][y] == NULL) {
		blackPatternCountInNull[blackPattern[x][y][direct]]--;
		whitePatternCountInNull[whitePattern[x][y][direct]]--;
	}
}

int getInsertedKey(int key, int offset, int state) {
	key = key & keyFullWithBlank[offset];
	key = (key | (state << ((7 - offset) * 2)));
	return key;
}

void updateLineKey(int x, int y) {
	int stateWhite = getPatternState(x, y, WHITE);
	int stateBlack = getPatternState(x, y, BLACK);

	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 7; p >= 4; p--) {
			if (reachable(px, py)) {
				blackLineKey[px][py][direct] = getInsertedKey(blackLineKey[px][py][direct], p, stateBlack);
				whiteLineKey[px][py][direct] = getInsertedKey(whiteLineKey[px][py][direct], p, stateWhite);
			}
			px += directX[direct];
			py += directY[direct];
		}
		for (int p = 3; p >= 0; p--) {
			px += directX[direct];
			py += directY[direct];
			if (reachable(px, py)) {
				blackLineKey[px][py][direct] = getInsertedKey(blackLineKey[px][py][direct], p, stateBlack);
				whiteLineKey[px][py][direct] = getInsertedKey(whiteLineKey[px][py][direct], p, stateWhite);
			}
		}
	}
}

void initPointLineKey(int x, int y) {
	int line[8];
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				blackLineKey[px][py][direct] = buildLineAndGetKey(line, px, py, direct, BLACK);
				whiteLineKey[px][py][direct] = buildLineAndGetKey(line, px, py, direct, WHITE);
			}
			px += directX[direct];
			py += directY[direct];
		}
	}
}

void updatePointPattern(int x, int y) {
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				int blackKey = blackLineKey[px][py][direct];
				int whiteKey = whiteLineKey[px][py][direct];
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

void updateLinePatternCount(int x, int y) {
	for (int direct = 0; direct < 4; direct++) {
		int px = x - directX[direct] * 4;
		int py = y - directY[direct] * 4;
		for (int p = 0; p <= 8; p++) {
			if (reachable(px, py)) {
				// instead to debug
				//removePointDirectPatternCount(px, py, direct);
				if (map[px][py] == BLACK) {
					blackPatternCount[blackPattern[px][py][direct]]++;
					blackPatternTotalScore += patternScore[blackLineKey[px][py][direct]];
				}
				if (map[px][py] == WHITE) {
					whitePatternCount[whitePattern[px][py][direct]]++;
					whitePatternTotalScore += patternScore[whiteLineKey[px][py][direct]];
				}
				if (map[px][py] == NULL) {
					blackPatternCountInNull[blackPattern[px][py][direct]]++;
					whitePatternCountInNull[whitePattern[px][py][direct]]++;
				}
			}
			px += directX[direct];
			py += directY[direct];
		}
	}
}

void initFullWithBlankKey() {
	int line[8] = { 3 ,3,3,3,3,3,3,3 };
	for (int i = 0; i < 8; i++) {
		line[i] = 0;
		keyFullWithBlank[i] = getLineKey(line);
		line[i] = 3;
	}
}

void initPatternRecord() {
	initFullWithBlankKey();
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			initPointLineKey(i, j);
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			updatePointPattern(i, j);
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			for (int k = 0; k < 4; k++) {
				addPointDirectPatternCount(i, j, k);
			}
		}
}

int countPattern(Color color, int patternCode)
{
	if (color == BLACK) {
		return blackPatternCountInNull[patternCode];
	}
	if (color == WHITE) {
		return whitePatternCountInNull[patternCode];
	}
	return false;
}


/***************************** 测试代码分隔 ***************************************/


static void printKey(int key) {
	for (int i = 0; i < 8; i++) {
		if (i == 4)
			printf("v");
		printf("%d", key % 4);
		key = key >> 2;
	}
	printf("\n");
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
	for (int i = 0; i < 10; i++) {
		printf("blackPatternCount[%d]=%d\n", i, blackPatternCount[i]);
		printf("whitePatternCount[%d]=%d\n", i, whitePatternCount[i]);
	}
}

void printPatternAnalyzeInMessage() {
	printMapInMessage(getMap());
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++) {
			for (int k = 0; k < 4; k++) {
				if (whitePattern[i][j][k] != PATTERN_NULL) {
					printf("MESSAGE whitePattern[%d][%d][%d]=%d\n", i, j, k, whitePattern[i][j][k]);
				}
				if (blackPattern[i][j][k] != PATTERN_NULL) {
					printf("MESSAGE blackPattern[%d][%d][%d]=%d\n", i, j, k, blackPattern[i][j][k]);
				}
			}
		}
	for (int i = 0; i < 10; i++) {
		printf("MESSAGE blackPatternCount[%d]=%d\n", i, blackPatternCount[i]);
		printf("MESSAGE whitePatternCount[%d]=%d\n", i, whitePatternCount[i]);
	}
	for (int i = 0; i < 10; i++) {
		printf("MESSAGE blackPatternCountInNull[%d]=%d\n", i, blackPatternCountInNull[i]);
		printf("MESSAGE whitePatternCountInNull[%d]=%d\n", i, whitePatternCountInNull[i]);
	}
}

void printPointPatternsInMessage(int x, int y) {
	int line[8];
	//输出查看
	char* patternName[] = { "长5", "活4", " 眠4", "活3",
		"眠3", "活2", "眠2", "活1", "眠1" };
	int patternCodes[] = { PATTERN_LINE_FIVE,PATTERN_ACTIVE_FOUR, PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE,
		PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO, PATTERN_ACTIVE_ONE, PATTERN_SLEEPY_ONE };

	for (int i = 0; i < 9; i++) {
		for (int k = 0; k < 4; k++) {
			if (blackPattern[x][y][k] == patternCodes[i]) {
				printf("MESSAGE blank pattern: %s\n", patternName[i]);
			}
			if (whitePattern[x][y][k] == patternCodes[i]) {
				printf("MESSAGE white pattern: %s\n", patternName[i]);
			}
		}
	}
}

void testSimpleMove() {
	boardSize = 20;
	initPattern();
	map = readMap("patternRecorder.txt");
	initGameMap(map);

	move(1, 1, BLACK);
	move(1, 3, BLACK);
	//undoMove(1, 3, BLACK, WHITE);
	move(0, 1, WHITE);
	undoMove(0, 1, WHITE);
	printPatternAnalyze();
}

void testInsertKey() {
	initFullWithBlankKey();
	int line[8] = { 3,0,0,1,0,0,1,2 };
	int key = getLineKey(line);
	int k = 2;
	printf("key = %d\n", key);
	int insertedKey = getInsertedKey(key, k, 1);

	line[k] = 1;
	int newKey = getLineKey(line);
	printf("newKey = %d insertedKey = %d\n", newKey, insertedKey);
	printKey(key);
	printKey(newKey);
	printKey(insertedKey);
	if (newKey == insertedKey) 
		printf("testSucceed\n");
    else
		printf("testFail\n");
}

void testPatternRecorder() {
	testSimpleMove();
}