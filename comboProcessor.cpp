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

extern bool comboEnable;

static comboResult processorResult;

static int currentLevel;

static int deepLevel;

static long long currentTargetTime;

extern int ** boardMap;

bool returnWinValue(int level, point p) {
	if (level == currentLevel) {
		processorResult.p = p;
	}
	return true;
}

void selectAttack(points* neighbor, Color color, int& comboType) {
	tryFiveAttack(getOtherColor(color), neighbor);
	if (comboType == COMBO_FOUR) {
		if (tryFourAttack(color, neighbor)) {
			return;
		}
	}
	if (comboType == COMBO_THREE) {
		if (tryThreeOrFourAttack(color, neighbor)) {
			return;
		}
	}

	neighbor->clear();
}

void selectDefence(points* neighbor, Color color, int comboType) {
	if (comboType == COMBO_FOUR) {
		if (tryFourDefence(color, neighbor)) {
			return;
		}
	}
	if (comboType == COMBO_THREE) {
		if (tryFourDefence(color, neighbor)) {
			return;
		}
		if (tryThreeDefence(color, neighbor))
			return;
	}
	neighbor->clear();
}

static bool killDfs(int level, Color color, Color aiColor, point lastPoint, point lastLastPoint, int comboType) {
	if (!comboEnable) {
		return false;
	}

	if (getSystemTime() > currentTargetTime) {
		return false;
	}

	//if (otherWin) {
	//	return false;
	//}

	deepLevel = level < deepLevel ? level : deepLevel;
	//printf("hit\n");
	//printMap(boardMap);
	//printMoveHistory();

	if (comboType == COMBO_THREE) {
		if (color != aiColor) {
			int tempDeepLevel = deepLevel;
			deepLevel = level;
			if (killDfs(currentLevel, color, color, point(), point(), COMBO_FOUR)) {
				//printf("hit\n");
				//printMap(boardMap);
				//printMoveHistory();
				deepLevel = tempDeepLevel;
				return false;
			}
			deepLevel = tempDeepLevel;
		}
	}

	if (color == aiColor) {
		if (!hasComboAttack(color, comboType)) {
			return false;
		}
	}

	points* ps = PointsFactory::createComboNeighborPoints(comboType, level);
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
		selectAttack(ps, color, comboType);
	else
		selectDefence(ps, color, comboType);

	if (canWinCheck(color)) {
		if (color == aiColor) {
			//printf("win\n");
			//printMap(boardMap);
			//printMoveHistory();
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
		bool value = killDfs(level - 1, getOtherColor(color), aiColor, point(x, y), lastPoint, comboType);
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

comboResult killVCF(Color color, int level, long long targetTime)
{
	currentTargetTime = targetTime;
	processorResult.canWin = false;
	processorResult.isDeep = false;

	//my 4 attack
	for (int i = 3; i <= level; i++) {
		deepLevel = level;
		currentLevel = level;
		processorResult.isDeep = false;
		processorResult.canWin = killDfs(i, color, color, point(), point(), COMBO_FOUR);
		if (deepLevel == 0) {
			processorResult.isDeep = true;
		}
	}
	return processorResult;
}

comboResult kill(Color color, int level, long long targetTime)
{
	currentTargetTime = targetTime;
	processorResult.canWin = false;
	processorResult.isDeep = false;

	//my 4 attack
	deepLevel = level;
	currentLevel = level;
	processorResult.isDeep = false;
	processorResult.canWin = killDfs(level, color, color, point(), point(), COMBO_FOUR);
	if (deepLevel == 0) {
		processorResult.isDeep = true;
	}
	if (processorResult.canWin) {
		return processorResult;
	}

	////my 3 attack
	deepLevel = level;
	currentLevel = level;
	processorResult.isDeep = false;
	processorResult.canWin = killDfs(level, color, color, point(), point(), COMBO_THREE);
	if (deepLevel == 0) {
		processorResult.isDeep = true;
	}
	if (getSystemTime() > currentTargetTime) {
		processorResult.canWin = false;
	}
	return processorResult;
}

//=========================test=========================


#include"io.h"
#include"console.h"
extern int boardSize;
extern int ** boardMap;

static void init() {
	initPattern();
	boardSize = 20;
	boardMap = readMap("combo.txt");
	initGameMap(boardMap);
	printMap(boardMap);
}

static void testSelectAttack() {
	points* ps = PointsFactory::createComboNeighborPoints(0, COMBO_THREE);
	fillNeighbor(ps);
	//selectAttack(ps, BLACK, COMBO_THREE);
	printMapWithStars(boardMap, *ps);
}

void testKill() {
	long long targerTime = getSystemTime() + 5000;
	for (int i = 3; i <= 30; i += 2) {
		printf("level %d\n", i);
		if (getSystemTime() > targerTime) {
			printf("TIME OUT\n");
			break;
		}
		comboResult result = kill(BLACK, i, targerTime);
		if (getSystemTime() > targerTime) {
			printf("TIME OUT\n");
		}
		if (result.canWin) {
			printf("WIN\n");
			printMapWithStar(boardMap, result.p);
			return;
		}
		else
		{
			printf("NULL\n");
		}
		if (!result.isDeep) {
			printf("not deep");
			break;
		}
	}
}

void testCombo() {
	init();
	testKill();
}