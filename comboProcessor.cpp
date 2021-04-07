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

bool returnWinValue(int level, point p) {
	if (level == currentLevel) {
		processorResult.p = p;
	}
	return true;
}

void selectAttack(points* neighbor, Color color, int comboType) {
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

	deepLevel = level < deepLevel ? level : deepLevel;

	if (color == aiColor) {
		if (!hasComboAttack(color, comboType)) {
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
		selectAttack(ps, color, comboType);
	else
		selectDefence(ps, color, comboType);

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

	//我方4连
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

	//我方4连
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
	//对方4连
	deepLevel = level;
	currentLevel = level;
	processorResult.canWin = killDfs(level, getOtherColor(color), getOtherColor(color), point(), point(), COMBO_FOUR);
	if (deepLevel == 0 || processorResult.canWin) {
		processorResult.isDeep = true;
		processorResult.canWin = false;
		return processorResult;
	}

	////我方3连
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

//=========================测试代码分隔=========================


#include"io.h"
#include"console.h"
extern int boardSize;
extern int ** map;

static void init() {
	initPattern();
	boardSize = 20;
	map = readMap("combo7.txt");
	initGameMap(map);
	printMap(map);
}

static void testSelectAttack() {
	points* ps = PointsFactory::createComboNeighborPoints(0);
	fillNeighbor(ps);
	selectAttack(ps, BLACK, COMBO_THREE);
	printMapWithStars(map, *ps);
}

void testKill() {
	long long targerTime = getSystemTime() + 1000;
	for (int i = 1; i <= 30; i += 2) {
		printf("level %d\n", i);
		if (getSystemTime() > targerTime) {
			break;
		}
		comboResult result = kill(BLACK, i, targerTime);
		if (result.canWin) {
			printf("WIN\n");
			printMapWithStar(map, result.p);
			return;
		}
		else
		{
			printf("NULL\n");
		}
		if (!result.isDeep) {
			break;
		}
	}
}

void testCombo() {
	init();
	testKill();
}