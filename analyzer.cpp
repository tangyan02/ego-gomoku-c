#include "stdafx.h"
#include "analyzer.h"
#include "patternRecorder.h"
#include "PointsFactory.h"
#include "gameMap.h"
#include "score.h"

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern Color ** map;

bool canWinCheck(Color color) {
	if (color == BLACK) {
		if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0) {
			return true;
		}
	}
	if (color == WHITE) {
		if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0) {
			return true;
		}
	}
	return false;
}

bool checkOnePattern(points *neighbors, int *patternCountInNull, int patter[][20][4], int pattern) {
	if (patternCountInNull[pattern] > 0)
	{
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == pattern) {
					neighbors->list[0] = neighbors->list[i];
					neighbors->count = 1;
					return true;
				}
			}
	}
	return false;
}

bool checkFourAttack(points *neighbors, int *patternCountInNull, int patter[][20][4]) {
	if (patternCountInNull[PATTERN_ACTIVE_FOUR] > 0 || patternCountInNull[PATTERN_SLEEPY_FOUR] > 0)
	{
		points* ps = PointsFactory::createLevelProcessorTempPoints();
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
					patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					) {
					ps->add(neighbors->list[i]);
				}
			}
		neighbors->clear();
		for (int i = 0; i < ps->count; i++) {
			neighbors->add(ps->list[i]);
		}
		return true;
	}
	return false;
}

bool checkDoubleFourAttack(points* neighbors, int* patternCountInNull, int patter[][20][4]) {
	if (patternCountInNull[PATTERN_SLEEPY_FOUR] > 1)
	{
		for (int i = 0; i < neighbors->count; i++){
			int fourAttack = 0;
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					) {
					fourAttack++;
				}
			}
			if (fourAttack >= 2) {
				point p = neighbors->list[i];
				neighbors->clear();
				neighbors->add(p);
				
				//map[p.x][p.y] = FLAG;
				//printMap(map);
				//map[p.x][p.y] = NULL;
				return true;
			}
		}
	}
	return false;
}


bool checkDoubleComboDefence(points* neighbors, int* patternCountInNull, int* MyPatternCountInNull, int patter[][20][4], int MyPatter[][20][4]) {
	if (patternCountInNull[PATTERN_SLEEPY_FOUR] > 0 || patternCountInNull[PATTERN_ACTIVE_THREE] > 0)
	{
		int directThree[4];
		int directFour[4];
		points* ps = PointsFactory::createLevelProcessorTempPoints();
		for (int i = 0; i < neighbors->count; i++) {
			int directThreeCount = 0;
			int directFourCount = 0;
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					) {
					directFour[directFourCount++] = k;
				}
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_THREE
					) {
					directThree[directThreeCount++] = k;
				}
			}
			if (directFourCount + directThreeCount >= 2) {
				ps->add(neighbors->list[i]);

				//line three defence
				for (int k = 0; k < directThreeCount; k++) {
					int px = neighbors->list[i].x - directX[directThree[k]] * 4;
					int py = neighbors->list[i].y - directY[directThree[k]] * 4;
					for (int i = 0; i <= 8; i++) {
						if (i != 4) {
							if (reachable(px, py)) {
								if (map[px][py] == NULL) {
									ps->add(point(px, py));
								}
							}
						}
						px += directX[directThree[k]];
						py += directY[directThree[k]];
					}
				}

				//line four defence
				for (int k = 0; k < directFourCount; k++) {
					int px = neighbors->list[i].x - directX[directFour[k]] * 4;
					int py = neighbors->list[i].y - directY[directFour[k]] * 4;
					for (int i = 0; i <= 8; i++) {
						if (i != 4) {
							if (inNeighbor(px, py)) {
								if (reachable(px, py)) {
									if (map[px][py] == NULL) {
										ps->add(point(px, py));
									}
								}
							}
						}
						px += directX[directFour[k]];
						py += directY[directFour[k]];
					}
				}

				// my four attack
				if (MyPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0 || MyPatternCountInNull[PATTERN_SLEEPY_FOUR] > 0) {
					for (int t = 0; t < neighbors->count; t++)
						for (int k = 0; k < 4; k++) {
							if (MyPatter[neighbors->list[t].x][neighbors->list[t].y][k] == PATTERN_ACTIVE_FOUR ||
								MyPatter[neighbors->list[t].x][neighbors->list[t].y][k] == PATTERN_SLEEPY_FOUR
								) {
								ps->add(neighbors->list[t]);
							}
						}
				}

				// my three attack
				if (directFourCount == 0) {
					if (MyPatternCountInNull[PATTERN_ACTIVE_THREE] || MyPatternCountInNull[PATTERN_SLEEPY_THREE] > 0) {
						for (int t = 0; t < neighbors->count; t++)
							for (int k = 0; k < 4; k++) {
								if (MyPatter[neighbors->list[t].x][neighbors->list[t].y][k] == PATTERN_ACTIVE_THREE ||
									MyPatter[neighbors->list[t].x][neighbors->list[t].y][k] == PATTERN_SLEEPY_THREE
									) {
									ps->add(neighbors->list[t]);
								}
							}
					}
				}

		/*		if (MyPatter == whitePattern) {
					if (ps->count > 0) {
						map[ps->list[0].x][ps->list[0].y] = STAR;
						for (int i = 0; i <= ps->count; i++) {
							if (map[ps->list[i].x][ps->list[i].y] != NULL) {
								printf("what!\n");
								for (int i = 0; i <= ps->count; i++) {
									printf("%d,%d ", ps->list[i].x,ps->list[i].y);
								}
								printf("\n");
								printMap(map);
								exit(0);
							}
							map[ps->list[i].x][ps->list[i].y] = FLAG;
						}
						if (map[ps->list[0].x][ps->list[0].y] != NULL) {
						}
						printMap(map);
						for (int i = 0; i <= ps->count; i++) {
							map[ps->list[i].x][ps->list[i].y] = NULL;
						}
					}
				}*/

				neighbors->clear();
				for (int i = 0; i < ps->count; i++) {
					neighbors->add(ps->list[i]);
				}
				return true;
			}
		}
	}
	return false;
}

bool checkThreeOrFourAttack(points *neighbors, int *patternCountInNull, int patter[][20][4]) {
	if (patternCountInNull[PATTERN_ACTIVE_FOUR] > 0 || patternCountInNull[PATTERN_SLEEPY_FOUR] > 0 || patternCountInNull[PATTERN_ACTIVE_THREE] > 0)
	{
		points* ps = PointsFactory::createLevelProcessorTempPoints();
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
					patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR ||
					patter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_THREE
					) {
					ps->add(neighbors->list[i]);
				}
			}
		neighbors->clear();
		for (int i = 0; i < ps->count; i++) {
			neighbors->add(ps->list[i]);
		}
		return true;
	}
	return false;
}

bool checkActiveThreeDefence(points *neighbors, int *otherPatternCountInNull, int selfPatter[][20][4], int otherPatter[][20][4]) {
	if (otherPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0)
	{
		points* ps = PointsFactory::createLevelProcessorTempPoints();
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				//³åËÄ
				if (selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
					selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					)
					ps->add(neighbors->list[i]);
				//·À3
				if (otherPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR) {
					ps->add(neighbors->list[i]);
					//¶Ï3·ÀÓù
					if (otherPatternCountInNull[PATTERN_ACTIVE_FOUR] == 1) {
						int px = neighbors->list[i].x - 4 * directX[k];
						int py = neighbors->list[i].y - 4 * directY[k];
						for (int j = 0; j < 9; j++) {
							if (otherPatter[px][py][k] == PATTERN_SLEEPY_FOUR) {
								if (reachable(px, py) && map[px][py] == NULL)
									ps->add(point(px, py));
							}
							px += directX[k];
							py += directY[k];
						}
					}
				}
			}
		neighbors->clear();
		for (int i = 0; i < ps->count; i++) {
			neighbors->add(ps->list[i]);
		}
		return true;
	}
	return false;
}

bool tryFiveAttack(Color color, points *neighbors) {
	if (color == WHITE) {
		return checkOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_LINE_FIVE);

	}
	if (color == BLACK) {
		return checkOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_LINE_FIVE);
	}
	return false;
}

bool tryActiveFourAttack(Color color, points* neighbors) {
	if (color == WHITE) {
		return checkOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_ACTIVE_FOUR);

	}
	if (color == BLACK) {
		return checkOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_ACTIVE_FOUR);
	}
	return false;
}

bool tryFourAttack(Color color, points *neighbors) {
	if (color == WHITE) {
		return checkFourAttack(neighbors, whitePatternCountInNull, whitePattern);
	}
	if (color == BLACK) {
		return checkFourAttack(neighbors, blackPatternCountInNull, blackPattern);
	}
	return false;
}

bool tryDoubleFourAttack(Color color, points* neighbors) {
	if (color == WHITE) {
		return checkDoubleFourAttack(neighbors, whitePatternCountInNull, whitePattern);
	}
	if (color == BLACK) {
		return checkDoubleFourAttack(neighbors, blackPatternCountInNull, blackPattern);
	}
	return false;
}


bool tryThreeOrFourAttack(Color color, points *neighbors) {
	if (color == WHITE) {
		if (blackPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0) {
			return checkFourAttack(neighbors, whitePatternCountInNull, whitePattern);
		}
		return checkThreeOrFourAttack(neighbors, whitePatternCountInNull, whitePattern);
	}
	if (color == BLACK) {
		if (whitePatternCountInNull[PATTERN_ACTIVE_FOUR] > 0) {
			return checkFourAttack(neighbors, blackPatternCountInNull, blackPattern);
		}
		return checkThreeOrFourAttack(neighbors, blackPatternCountInNull, blackPattern);
	}
	return false;
}

bool tryDoubleComboDefence(Color color, points* neighbors) {
	if (color == WHITE) {
		return checkDoubleComboDefence(neighbors, blackPatternCountInNull, whitePatternCountInNull, blackPattern ,whitePattern);
	}
	if (color == BLACK) {
		return checkDoubleComboDefence(neighbors, whitePatternCountInNull, blackPatternCountInNull, whitePattern, blackPattern);
	}
	return false;
}

bool tryFourDefence(Color color, points *neighbors) {
	if (color == WHITE) {
		return checkOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_LINE_FIVE);
	}
	if (color == BLACK) {
		return checkOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_LINE_FIVE);
	}
	return false;
}

bool tryThreeDefence(Color color, points *neighbors) {
	if (color == WHITE) {
		return checkActiveThreeDefence(neighbors, blackPatternCountInNull, whitePattern, blackPattern);
	}
	if (color == BLACK) {
		return checkActiveThreeDefence(neighbors, whitePatternCountInNull, blackPattern, whitePattern);
	}
	return false;
}


point findOnePattern(points *neighbors, int *patternCountInNull, int patter[][20][4], int pattern) {
	if (patternCountInNull[pattern] > 0)
	{
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				if (patter[neighbors->list[i].x][neighbors->list[i].y][k] == pattern) {
					return neighbors->list[i];
				}
			}
	}
	return point();
}

point getFiveAttack(points *neighbors, Color color) {
	if (color == WHITE) {
		return findOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_LINE_FIVE);

	}
	if (color == BLACK) {
		return findOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_LINE_FIVE);
	}
	return point();
}

bool hasComboAttack(Color color, int comboType) {
	if (comboType == COMBO_THREE) {
		if (color == BLACK) {
			if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0 ||
				blackPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0 ||
				blackPatternCountInNull[PATTERN_SLEEPY_FOUR] > 0 ||
				blackPatternCountInNull[PATTERN_ACTIVE_THREE] > 0
				)
				return true;
		}
		if (color == WHITE) {
			if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0 ||
				whitePatternCountInNull[PATTERN_ACTIVE_FOUR] > 0 ||
				whitePatternCountInNull[PATTERN_SLEEPY_FOUR] > 0 ||
				whitePatternCountInNull[PATTERN_ACTIVE_THREE] > 0
				)
				return true;
		}
	}
	if (comboType == COMBO_FOUR) {
		if (color == BLACK) {
			if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0 ||
				blackPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0 ||
				blackPatternCountInNull[PATTERN_SLEEPY_FOUR] > 0
				)
				return true;
		}
		if (color == WHITE) {
			if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0 ||
				whitePatternCountInNull[PATTERN_ACTIVE_FOUR] > 0 ||
				whitePatternCountInNull[PATTERN_SLEEPY_FOUR] > 0
				)
				return true;
		}
	}
	return false;
}

//======================test code =======================

#include"io.h"
#include"console.h"
extern int boardSize;
extern int** map;

static void init() {
	initPattern();
	boardSize = 20;
	map = readMap("analyze4.txt");
	initGameMap(map);
	printMap(map);
	printf("================\n");
}

void testAnalyzer()
{
	init();
	points* neighbor = PointsFactory::createPointNeighborPoints(0, 0);
	fillNeighbor(neighbor);
	tryDoubleComboDefence(WHITE, neighbor);
	for (int i = 0; i < neighbor->count; i++) {
		map[neighbor->list[i].x][neighbor->list[i].y] = FLAG;
	}
	printMap(map);
}

