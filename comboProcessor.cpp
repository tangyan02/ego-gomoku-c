#include "stdafx.h"
#include "comboProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "analyzer.h"
#include "comboProcessor.h"

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

static comboResult processorResult;

static int currentLevel;

bool returnWinValue(int level, point p) {
	if (level == currentLevel) {
		processorResult.p = p;
	}
	return true;
}

static bool killDfs(int level, Color color, Color aiColor) {

	if (level == 0) {
		return false;
	}

	points temp;
	points* ps = &temp;
	fillNeighbor(ps);

	if (canWinCheck(color)) {
		return returnWinValue(level, getFiveAttack(ps, color));
	}

	if (ps->count == 0) {
		if (color == aiColor)
			return false;
		else
			return true;
	}

	for (int i = 0; i < ps->count; i++) {
		int x = ps->list[i].x;
		int y = ps->list[i].y;
		move(x, y, color);
		bool value = killDfs(level - 1, getOtherColor(color), aiColor);
		undoMove(x, y, color);

		if (color == aiColor) {
			if (value) {
				return returnWinValue(level, point(x, y));
			}
		}

		if (color != aiColor) {
			if (value == false) {
				return false;
			}
		}
	}

	if (color == aiColor)
		return false;
	else
		return true;
}

comboResult kill(Color color, int level)
{
	currentLevel = level;
	processorResult.canWin = killDfs(level, color, color);
	return processorResult;
}

//=========================≤‚ ‘¥˙¬Î∑÷∏Ù=========================


#include"io.h"
#include"console.h"
extern int boardSize;
extern int ** map;

void testCombo() {
	initPattern();
	map = readMap("combo3.txt");
	initGameMap(map);
	printMap(map);
	comboResult result = kill(BLACK, 5);
	if (result.canWin) {
		printf("WIN\n");
		printMapWithStar(map, result.p);
	}
	else 
	{
		printf("NULL");
	}
}