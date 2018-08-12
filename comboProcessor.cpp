#include "stdafx.h"
#include "comboProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "analyzer.h"
#include "comboProcessor.h"
#include "PointsFactory.h"

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

void selectAttack(points* neighbor, Color color) {
	if (tryFourAttack(color, neighbor)) {
		return;
	}
	neighbor->clear();
}

void selectDefence(points* neighbor, Color color) {
	if (tryFourDefence(color, neighbor)) {
		return;
	}
	neighbor->clear();
}

static bool killDfs(int level, Color color, Color aiColor,point lastPoint, point lastLastPoint) {
	if (color == aiColor) {
		if (!hasComboAttack(color)) {
			return false;
		}
	}

	points* ps = PointsFactory::createComboNeighborPoints(level);
	if (level == currentLevel) {
		fillNeighbor(ps);
	}
	if (level == currentLevel - 1) {
		fillPointLinesNeighbor(lastPoint.x, lastPoint.y, ps);
	}
	if (level < currentLevel - 1) {
		fillPointLinesNeighbor(lastPoint.x, lastPoint.y, ps);
		fillPointLinesNeighbor(lastLastPoint.x, lastLastPoint.y, ps);
	}

	if (level == 0) {
		return false;
	}

	if (color == aiColor)
		selectAttack(ps, color);
	else
		selectDefence(ps, color);

	if (canWinCheck(color)) {
		if (color == aiColor) {
			return returnWinValue(level, getFiveAttack(ps, color));
		}
		else {
			return false;
		}
	}

	if (ps->count == 0) {
		if (color == aiColor)
			return false;
	}

	for (int i = 0; i < ps->count; i++) {
		int x = ps->list[i].x;
		int y = ps->list[i].y;
		move(x, y, color);
		bool value = killDfs(level - 1, getOtherColor(color), aiColor, point(x, y), lastPoint);
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
	processorResult.canWin = killDfs(level, color, color, point(), point());
	return processorResult;
}

//=========================≤‚ ‘¥˙¬Î∑÷∏Ù=========================


#include"io.h"
#include"console.h"
extern int boardSize;
extern int ** map;

void testCombo() {
	initPattern();
	map = readMap("comboVCF.txt");
	initGameMap(map);
	printMap(map);

	for (int i = 1; i <= 15; i+=2) {
		printf("level %d\n", i);
		comboResult result = kill(BLACK, i);
		if (result.canWin) {
			printf("WIN\n");
			printMapWithStar(map, result.p);
			return;
		}
		else
		{
			printf("NULL\n");
		}
	}
}