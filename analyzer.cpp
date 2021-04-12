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

extern Color ** boardMap;

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
				//����
				if (selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
					selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					)
					ps->add(neighbors->list[i]);
				//��3
				if (otherPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR) {
					ps->add(neighbors->list[i]);
					//��3����
					if (otherPatternCountInNull[PATTERN_ACTIVE_FOUR] == 1) {
						int px = neighbors->list[i].x - 4 * directX[k];
						int py = neighbors->list[i].y - 4 * directY[k];
						for (int j = 0; j < 9; j++) {
							if (otherPatter[px][py][k] == PATTERN_SLEEPY_FOUR) {
								if (reachable(px, py) && boardMap[px][py] == NULL)
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

bool tryFourAttack(Color color, points *neighbors) {
	if (color == WHITE) {
		return checkFourAttack(neighbors, whitePatternCountInNull, whitePattern);
	}
	if (color == BLACK) {
		return checkFourAttack(neighbors, blackPatternCountInNull, blackPattern);
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