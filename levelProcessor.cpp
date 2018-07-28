#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"

extern int boardSize;

static int score[512];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

int levelScore[10] = { 0, 100000,10000,100,100,50,50,20,2,1 };

extern Color ** map;

int getScore(int x, int y, Color color) {
	int value = 0;
	for (int k = 0; k < 4; k++) {
		if (color == BLACK)
			value += levelScore[blackPattern[x][y][k]];
		if (color == WHITE)
			value += levelScore[whitePattern[x][y][k]];
	}
	return value;
}

void qsort(points *neighbors, int score[], int l, int r) {
	int x = l;
	int y = r;
	int mid = score[(x + y) / 2];
	while (x < y) {
		while (score[x] > mid) x++;
		while (score[y] < mid) y--;
		if (x <= y) {
			int t = score[x];
			score[x] = score[y];
			score[y] = t;
			point p = neighbors->list[x];
			neighbors->list[x] = neighbors->list[y];
			neighbors->list[y] = p;
			x++;
			y--;
		}
	}
	if (x < r) qsort(neighbors, score, x, r);
	if (l < y) qsort(neighbors, score, l, y);
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

bool checkActiveThree(points *neighbors, int *patternCountInNull, int selfPatter[][20][4], int otherPatter[][20][4]) {
	if (patternCountInNull[PATTERN_ACTIVE_FOUR] > 0)
	{
		points ps;
		for (int i = 0; i < neighbors->count; i++)
			for (int k = 0; k < 4; k++) {
				//冲四
				if (selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
					selfPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
					)
					ps.add(neighbors->list[i]);
				//防3
				if (otherPatter[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR) {
					ps.add(neighbors->list[i]);
					//断3防御
					if (patternCountInNull[PATTERN_ACTIVE_FOUR] == 1) {
						int px = neighbors->list[i].x - 4 * directX[k];
						int py = neighbors->list[i].y - 4 * directY[k];
						for (int j = 0; j < 9; j++) {
							if (otherPatter[px][py][k] == PATTERN_SLEEPY_FOUR) {
								if (reachable(px, py) && map[px][py] == NULL)
									ps.add(point(px, py));
							}
							px += directX[k];
							py += directY[k];
						}
					}
				}
			}
		neighbors->clear();
		for (int i = 0; i < ps.count; i++) {
			neighbors->add(ps.list[i]);
		}
		return true;
	}
	return false;
}

void selectAndSortPoints(points *neighbors, Color color) {
	//连长五
	if (color == WHITE) {
		if (checkOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_LINE_FIVE))
			return;
	}
	if (color == BLACK) {
		if (checkOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_LINE_FIVE))
			return;
	}
	//堵冲四
	if (color == WHITE) {
		if (checkOnePattern(neighbors, blackPatternCountInNull, blackPattern, PATTERN_LINE_FIVE))
			return;
	}
	if (color == BLACK) {
		if (checkOnePattern(neighbors, whitePatternCountInNull, whitePattern, PATTERN_LINE_FIVE))
			return;
	}
	//对付活三
	if (color == WHITE) {
		if (checkActiveThree(neighbors, blackPatternCountInNull, whitePattern, blackPattern))
			return;
	}

	if (color == BLACK) {
		if (checkActiveThree(neighbors, whitePatternCountInNull, blackPattern, whitePattern))
			return;
	}

	//排序
	for (int i = 0; i < neighbors->count; i++) {
		score[i] = getScore(neighbors->list[i].x, neighbors->list[i].y, color);
	}
	qsort(neighbors, score, 0, neighbors->count - 1);
}

