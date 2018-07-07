#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "analyzer.h"
#include "patternRecorder.h"

extern int boardSize;

static int score[128];

extern bool fiveAttackTable[MAX_TABLE_SIZE];
extern bool fourAttackTable[MAX_TABLE_SIZE];
extern bool fourDefenceTable[MAX_TABLE_SIZE];
extern bool threeAttackTable[MAX_TABLE_SIZE];
extern bool twoAttackTable[MAX_TABLE_SIZE];
extern bool threeDefenceTable[MAX_TABLE_SIZE][4][4][4][4];

static int fiveAttackScore = 100000;
static int fourDefenceScore = 10000;
static int fourAttackScore = 1000;
static int threeAttackScore = 100;
static int twoAttackScore = 20;

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int getScore(int x, int y, Color color) {
	int value = 0;
	for (int i = 0; i < 4; i++) {
		int key = getMapLineKey(x, y, i, color);
		if (fiveAttackTable[key])
			value += fiveAttackScore;
		if (fourDefenceTable[key])
			value += fourDefenceScore;
		if (fourAttackTable[key])
			value += fourAttackScore;
		if (threeAttackTable[key])
			value += threeAttackScore;
		if (twoAttackTable[key])
			value += twoAttackScore;
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

void sortPoints(points *neighbors, Color color) {
	//棋类知识检查
	for (int i = 0; i < neighbors->count; i++) {
		//冲五
		for (int k = 0; k < 4; k++) {
			int key = getMapLineKey(neighbors->list[i].x, neighbors->list[i].y, k, color);
			if (fiveAttackTable[key]) {
				neighbors->list[0] = neighbors->list[i];
				neighbors->count = 1;
				return;
			}
		}
	}
	points fourAttack;
	points threeDefence;
	for (int i = 0; i < neighbors->count; i++) {
		for (int k = 0; k < 4; k++) {
			//防四
			int key = getMapLineKey(neighbors->list[i].x, neighbors->list[i].y, k, color);
			if (fourDefenceTable[key]) {
				neighbors->list[0] = neighbors->list[i];
				neighbors->count = 1;
				return;
			}
			//保存冲四
			if (fourAttackTable[key]) {
				fourAttack.add(neighbors->list[i]);
			}
			//保存防御活三
			if (getThreeDefenseTable(key, neighbors->list[i].x, neighbors->list[i].y, k, color))
				threeDefence.add(neighbors->list[i]);
		}
	}
	if (threeDefence.count > 0) {
		int count = 0;
		for (int i = 0; i < fourAttack.count; i++) {
			neighbors->list[count] = fourAttack.list[i];
			count++;
		}
		for (int i = 0; i < threeDefence.count; i++) {
			neighbors->list[count] = threeDefence.list[i];
			count++;
		}
		neighbors->count = count;
		return;
	}

	//计算得分
	for (int i = 0; i < neighbors->count; i++) {
		score[i] = getScore(neighbors->list[i].x, neighbors->list[i].y, color);
	}
	sort(neighbors, score);
}

