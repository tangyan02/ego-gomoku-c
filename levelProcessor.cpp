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

void sort(points *neighbors, int score[]) {
	for (int i = 0; i < neighbors->count; i++)
		for (int j = i; j < neighbors->count; j++)
			if (score[i] < score[j]) {
				int t = score[i];
				score[i] = score[j];
				score[j] = t;
				point p = neighbors->list[i];
				neighbors->list[i] = neighbors->list[j];
				neighbors->list[j] = p;
			}
}

void selectAndSortPoints(points *neighbors, Color color) {
	//¡¨≥§ŒÂ
	if (color == WHITE) {
		if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0)
		{
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					if (whitePattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_LINE_FIVE) {
						neighbors->list[0] = neighbors->list[i];
						neighbors->count = 1;
						return;
					}
				}
		}
	}
	if (color == BLACK) {
		if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0)
		{
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					if (blackPattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_LINE_FIVE) {
						neighbors->list[0] = neighbors->list[i];
						neighbors->count = 1;
						return;
					}
				}
		}
	}
	//∂¬≥ÂÀƒ
	if (color == WHITE) {
		if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0)
		{
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					if (blackPattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_LINE_FIVE) {
						neighbors->list[0] = neighbors->list[i];
						neighbors->count = 1;
						return;
					}
				}
		}
	}
	if (color == BLACK) {
		if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0) {
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					if (whitePattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_LINE_FIVE) {
						neighbors->list[0] = neighbors->list[i];
						neighbors->count = 1;
						return;
					}
				}
		}
	}
	//∂‘∏∂ªÓ»˝
	if (color == WHITE) {
		if (blackPatternCountInNull[PATTERN_ACTIVE_FOUR] > 0)
		{
			points ps;
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					//≥ÂÀƒ
					if (whitePattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
						whitePattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
						)
						ps.add(neighbors->list[i]);
					//∑¿3
					if (blackPattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR) {
						ps.add(neighbors->list[i]);
						//∂œ3∑¿”˘
						if (blackPatternCountInNull[PATTERN_ACTIVE_FOUR] == 1) {
							int px = neighbors->list[i].x - 4 * directX[k];
							int py = neighbors->list[i].y - 4 * directY[k];
							for (int j = 0; j < 9; j++) {
								if (blackPattern[px][py][k] == PATTERN_SLEEPY_FOUR) {
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
			return;
		}
	}

	if (color == BLACK) {
		if (whitePatternCountInNull[PATTERN_ACTIVE_FOUR] > 0)
		{
			points ps;
			for (int i = 0; i < neighbors->count; i++)
				for (int k = 0; k < 4; k++) {
					//≥ÂÀƒ
					if (blackPattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR ||
						blackPattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_SLEEPY_FOUR
						)
						ps.add(neighbors->list[i]);
					//∑¿3
					if (whitePattern[neighbors->list[i].x][neighbors->list[i].y][k] == PATTERN_ACTIVE_FOUR) {
						ps.add(neighbors->list[i]);
						//∂œ3∑¿”˘
						if (whitePatternCountInNull[PATTERN_ACTIVE_FOUR] == 1) {
							int px = neighbors->list[i].x - 4 * directX[k];
							int py = neighbors->list[i].y - 4 * directY[k];
							for (int j = 0; j < 9; j++) {
								if (whitePattern[px][py][k] == PATTERN_SLEEPY_FOUR) {
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
			return;
		}
	}

	//≈≈–Ú
	for (int i = 0; i < neighbors->count; i++) {
		score[i] = getScore(neighbors->list[i].x, neighbors->list[i].y, color);
	}
	sort(neighbors, score);
}

