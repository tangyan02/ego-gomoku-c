#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "PointsFactory.h"
#include "analyzer.h"


extern int boardSize;

static int scoreList[512];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

extern int score[10];

extern points moveHistory;

extern Color ** map;

static const int crossWeight = 50;
static const int crossLimit = 3;

int getScore(int x, int y, Color color) {
	int value = 0;
	for (int k = 0; k < 4; k++) {
		if (color == BLACK)
			value += score[blackPattern[x][y][k]];
		if (color == WHITE)
			value += score[whitePattern[x][y][k]];
	}
	return value;
}

void qsort(point *list, int score[], int l, int r) {
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
			point p = list[x];
			list[x] = list[y];
			list[y] = p;
			x++;
			y--;
		}
	}
	if (x < r) qsort(list, score, x, r);
	if (l < y) qsort(list, score, l, y);
}

bool cross(int x1, int y1, int x2, int y2) {
	if (x1 == x2 && abs(y1 - y2) <= crossLimit) {
		return true;
	}
	if (y1 == y2 && abs(x1 - x2) <= crossLimit) {
		return true;
	}
	if (x1 - x2 == y1 - y2 && abs(x1 - x2) <= crossLimit) {
		return true;
	}
	if (x1 + x2 == y1 + y2 && abs(y1 - y2) <= crossLimit) {
		return true;
	}
	return false;
}

void selectAndSortPoints(points *neighbors, Color color) {
	//连长五
	if (tryFiveAttack(color, neighbors))
		return;
	//堵冲四
	if (tryFourDefence(color, neighbors))
		return;
	//对付活三
	if (tryThreeDefence(color, neighbors))
		return;

	//排序
	point* list = neighbors->list;
	
	//历史2步的米字位置优先计算
	/*point pointLast1;
	point pointLast2;
	if (moveHistory.count > 2) {
		pointLast1 = moveHistory.list[moveHistory.count - 1];
		pointLast2 = moveHistory.list[moveHistory.count - 2];
	}*/

	for (int i = 0; i < neighbors->count; i++) {
		scoreList[i] = getScore(list[i].x, list[i].y, color);

		//如果和历史节点是米字位置关系，则优先计算
		/*if (moveHistory.count > 2) {
			if (cross(pointLast1.x, pointLast1.y, list[i].x, list[i].y)) {
				scoreList[i] += crossWeight;
			}
			else if (cross(pointLast2.x, pointLast2.y, list[i].x, list[i].y)) {
				scoreList[i] += crossWeight;
			}
		}*/
	}
	qsort(list, scoreList, 0, neighbors->count - 1);
}

