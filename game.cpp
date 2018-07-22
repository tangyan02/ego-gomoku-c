#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "console.h"
#include <sys/timeb.h>
#include "unordered_map"

extern int boardSize;

extern int searchLevel;

extern int comboLevel;

extern int timeOut;

extern bool debugEnable;

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

static int currentLevel;

static point currentPointResult;

static pointHash loseSet;

static unordered_map<long long, point> cache;

static unordered_map<long long, point> cacheLast;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

bool tryScoreSearchIteration(points * neighbors, Color aiColor, gameResult *gameResult, Color** map) {
	//如果必败，则朴素搜索
	bool loseFlag = false;
	if (neighbors->count == loseSet.count) {
		loseSet.reset();
		loseFlag = true;
	}
	//得分搜索
	timeOutEnable = false;
	searchStartTime = getSystemTime();
	cacheLast.clear();
	cache.clear();
	for (int level = 2; level <= searchLevel; level += 2)
	{
		long long t = getSystemTime();
		nodeCount = 0;
		currentLevel = level;

		cacheLast = cache;
		cache.clear();

		Color color = aiColor;
		int alpha = MIN_VALUE;
		int beta = MAX_VALUE;

		int value = dfs(level, color, aiColor, alpha, beta, 0);

		if (timeOutEnable) {
			if (debugEnable)
				printf("time out\n");
			break;
		}

		if (!timeOutEnable) {
			if (level > 2 && loseSet.count == neighbors->count) {
				gameResult->value = value;
				return true;
			}
			gameResult->result = point(currentPointResult.x, currentPointResult.y);
			int speed = 0;
			if ((getSystemTime() - t) > 0)
				speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
			gameResult->speed = speed;
			gameResult->node = nodeCount;
			gameResult->value = value;
			gameResult->level = level;
			if (debugEnable) {
				printf("level %d, value %d, speed %d k, x:%d y:%d time %lld ms\n", level, value, speed, gameResult->result.x, gameResult->result.y, getSystemTime() - t);
				printMapWithStar(map, currentPointResult);
			}
			if (value == MAX_VALUE)
				break;
		}
		if (getSystemTime() - searchStartTime > timeOut / 3) {
			timeOutEnable = true;
		}
	}
	if (loseFlag)
		gameResult->value = MIN_VALUE;
	return true;
}

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	//初始化
	initPattern();
	clearPatternRecord();
	initGameMap(map);

	//初始分析
	points neighbors = getNeighbor();
	selectAndSortPoints(&neighbors, aiColor);

	if (neighbors.count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (neighbors.count == 1) {
		gameResult.result = neighbors.list[0];
		return gameResult;
	}

	//得分迭代搜索
	tryScoreSearchIteration(&neighbors, aiColor, &gameResult, map);
	return gameResult;
}

bool canWinCheck(points *neighbors, Color color) {
	//输赢判定
	Color result = win(getMap());
	if (result == color) {
		return true;
	}
	return false;
}

void moveHistoryBestToFirst(points * neighbors) {
	if (cacheLast.find(getMapHashCode()) != cacheLast.end()) {
		point p = cacheLast[getMapHashCode()];
		for (int i = 0; i < neighbors->count; i++)
			if (neighbors->list[i].x == p.x && neighbors->list[i].y == p.y) {
				for (int j = i; j > 0; j--) {
					neighbors->list[j] = neighbors->list[j - 1];
				}
				neighbors->list[0] = p;
				break;
			}
	}
}

/* 零窗口测试法
*/
int dfs(int level, Color color, Color aiColor, int alpha, int beta, int extend) {
	if (getSystemTime() - searchStartTime > timeOut) {
		timeOutEnable = true;
	}
	if (timeOutEnable) {
		return 0;
	}
	nodeCount++;

	//叶子分数计算
	if (level == 0) {
		return getScoreValue(color, aiColor);
	}

	//获取扩展节点
	points neighbors = getNeighbor();
	if (canWinCheck(&neighbors, color)) {
		return MAX_VALUE;
	}

	//排序
	selectAndSortPoints(&neighbors, color);

	//调整最优节点顺序
	moveHistoryBestToFirst(&neighbors);

	//遍历扩展节点
	int extreme = MIN_VALUE;
	points extremePoints;
	for (int i = 0; i < neighbors.count; i++) {
		point p = point(neighbors.list[i].x, neighbors.list[i].y);

		move(p.x, p.y, color, aiColor);
		int value;
		//先查必败表
		if (level == currentLevel && extend == 0 && loseSet.contains(p)) {
			value = MIN_VALUE;
		}
		else {
			if (i == 0) {
				value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
			}
			else {
				//零窗口测试
				value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha, extend);
				if (value > alpha && value < beta) {
					value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
				}
			}
		}
		undoMove(p.x, p.y, color, aiColor);
		if (value >= extreme) {
			if (value > extreme) {
				extremePoints.clear();
			}
			extreme = value;
			extremePoints.add(p);

			if (value > alpha) {
				alpha = value;
				if (value > beta) {
					cache[getMapHashCode()] = p;
					return value;
				}
			}
		}

		if (level == currentLevel && extend == 0) {
			if (debugEnable)
				printf("(%d, %d) value: %d nodes: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);

			if (value == MAX_VALUE) {
				currentPointResult = p;
				return value;
			}
			if (value == MIN_VALUE) {
				if (!loseSet.contains(p))
					loseSet.add(p);
			}
		}
	}

	point extremePoint = extremePoints.list[rand() % extremePoints.count];
	if (level == currentLevel && extend == 0) {
		currentPointResult = point(extremePoint.x, extremePoint.y);
	}

	cache[getMapHashCode()] = extremePoint;
	return extreme;
}