#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "analyzer.h"

extern int boardSize;

extern int nodeLimit;

extern bool moreAnalyze;

static int score[128];

extern bool fiveAttackTable[MAX_TABLE_SIZE];
extern bool fourAttackTable[MAX_TABLE_SIZE];
extern bool fourDefenceTable[MAX_TABLE_SIZE];
extern bool threeAttackTable[MAX_TABLE_SIZE];
extern bool twoAttackTable[MAX_TABLE_SIZE];
extern bool threeDefenceTable[MAX_TABLE_SIZE][4][4];

static int fiveAttackScore = 100000;
static int fourDefenceScore = 10000;
static int fourAttackScore = 1000;
static int threeAttackScore = 100;
static int twoAttackScore = 20;

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int getScore(point p, Color color) {
	int value = 0;
	for (int i = 0; i < 4; i++) {
		int key = getMapLineKey(p.x, p.y, i, color);
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
	points fourAttack;
	points threeDefence;
	for (int i = 0; i < neighbors->count; i++) {
		for (int k = 0; k < 4; k++) {
			//冲五和防四
			int key = getMapLineKey(neighbors->list[i].x, neighbors->list[i].y, k, color);
			if (fourDefenceTable[key] || fiveAttackTable[key]) {
				neighbors->list[0] = neighbors->list[i];
				neighbors->count = 1;
				return;
			}
			//保存冲四
			if (fourAttackTable[key]) {
				fourAttack.add(neighbors->list[i]);
			}
			//保存防御活三
			point leftPoint = point(neighbors->list[i].x - 5 * directX[k], neighbors->list[i].y - 5 * directY[k]);
			point rightPoint = point(neighbors->list[i].x + 5 * directX[k], neighbors->list[i].y + 5 * directY[k]);
			int left = getPointTableColor(leftPoint.x, leftPoint.y, color);
			int right = getPointTableColor(rightPoint.x, rightPoint.y, color);
			if (threeDefenceTable[key][left][right])
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
		score[i] = getScore(neighbors->list[i], color);
	}
	sort(neighbors, score);
}

points getExpandPoints(analyzeData data, points neighbors)
{
	points result;
	//如果能连5，则连5
	if (data.fiveAttack.count > 0) {
		result.addMany(data.fiveAttack);
		return result;
	}
	//如果有对方冲4，则防冲4
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	//如果有对方活3，则防活3或者冲四
	if (data.threeDefence.count > 0) {
		pointHash hash = pointHash();
		for (int i = 0; i < data.fourAttack.count; i++) {
			point p = data.fourAttack.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		for (int i = 0; i < data.threeDefence.count; i++) {
			point p = data.threeDefence.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		return result;
	}
	pointHash hash = pointHash();
	for (int i = 0; i < data.fourAttack.count; i++) {
		point p = data.fourAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < data.threeAttack.count; i++) {
		point p = data.threeAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < data.twoAttack.count; i++) {
		point p = data.twoAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < neighbors.count; i++) {
		point p = neighbors.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}

	if (result.count > nodeLimit)
		result.count = nodeLimit;

	return result;
}
