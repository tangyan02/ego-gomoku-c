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

extern int ** map;

bool returnWinValue(int level, point p) {
	if (level == currentLevel) {
		processorResult.p = p;
	}
	return true;
}

void selectAttack(points* neighbor, Color color, int& comboType) {
	tryFiveAttack(getOtherColor(color), neighbor);

	if (comboType == COMBO_THREE) {
		if (tryFourDefence(color, neighbor)) {
			if (tryThreeOrFourAttack(color, neighbor)) {
				return;
			}
		}

		if (tryThreeDefence(color, neighbor)) {
			if (tryThreeOrFourAttack(color, neighbor)) {
				return;
			}
		}

		if (tryThreeOrFourAttack(color, neighbor)) {
			return;
		}
	}
	if (comboType == COMBO_FOUR) {
		if (tryFourAttack(color, neighbor)) {
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
		points* ps = PointsFactory::createTempPoints();
		tryFourAttack(color, ps);
		
		if (tryThreeDefence(color, neighbor)) {
			neighbor->addMany(ps);
			//printMapWithStars(map, *neighbor);
			return;
		}
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

	//printf("hit\n");
	//printMap(map);
	//printMoveHistory();

	if (comboType == COMBO_THREE) {
		int tempDeepLevel = deepLevel;
		int tempCurrentLevel = currentLevel;
		deepLevel = level;
		currentLevel = currentLevel * 2;
		if (killDfs(currentLevel, color, color, point(), point(), COMBO_FOUR)) {
		/*	printf("hit\n");
			printMap(map);
			printMoveHistory();*/
			deepLevel = tempDeepLevel;
			currentLevel = tempCurrentLevel;
			if (color == aiColor) {
				return true;
			}
			return false;
		}
		deepLevel = tempDeepLevel;
		currentLevel = tempCurrentLevel;
	}

	if (!hasComboAttack(aiColor, comboType)) {
		return false;
	}

	points* ps = PointsFactory::createComboNeighborPoints(comboType, level);
	if (level == currentLevel) {
		fillNeighbor(ps);
	}

	//if (comboType == COMBO_THREE) {
	//	fillNeighbor(ps);
	//}

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
			//printMap(map);
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
extern int ** map;

static void init(char* path) {
	initPattern();
	boardSize = 20;
	map = readMap(path);
	initGameMap(map);
}

static void testSelectAttack() {
	points* ps = PointsFactory::createComboNeighborPoints(0, COMBO_THREE);
	fillNeighbor(ps);
	//selectAttack(ps, BLACK, COMBO_THREE);
	printMapWithStars(map, *ps);
}

static void testSelectDefen() {
	points* ps = PointsFactory::createComboNeighborPoints(0, COMBO_THREE);
	fillNeighbor(ps);
	//selectAttack(ps, BLACK, COMBO_THREE);
	printMapWithStars(map, *ps);
}

bool testKill(Color color, bool print) {
	long long targerTime = getSystemTime() + 5000;
	for (int i = 3; i <= 30; i += 2) {
		if (print) {
			printf("level %d\n", i);
		}
		if (getSystemTime() > targerTime) {
			if (print) {
				printf("TIME OUT\n");
			}
			break;
		}
		comboResult result = kill(color, i, targerTime);
		if (getSystemTime() > targerTime) {
			if (print) {
				printf("TIME OUT\n");
			}
		}
		if (result.canWin) {
			if (print) {
				printf("WIN\n");
				printMapWithStar(map, result.p);
			}
			return true;
		}
		else
		{
			if (print) {
				printf("NULL\n");
			}
		}
		if (!result.isDeep) {
			if (print) {
				printf("not deep");
			}
			break;
		}
	}
	return false;
}

void testCombo() {
	init("combo.txt");
	printMap(map);
	testKill(BLACK, true);
}

void testAllCombo() {
	const int count = 3;
	char* paths[count] = {"combo8.txt", "combo9.txt", "combo10.txt"};
	Color colors[count] = { WHITE, BLACK, BLACK };
	bool expect[count] = { true , false, false};
	for (int i = 0; i < count; i++) {
		printf(" test: %s ", paths[i]);
		init(paths[i]);
		bool result = testKill(colors[i], false);
		if (result == expect[i]) {
			printf(" ok \n");
		}
		else {
			printf("error \n");
		}
	}
}