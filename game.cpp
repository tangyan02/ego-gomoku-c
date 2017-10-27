#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "analyzer.h"
#include "console.h"
#include <sys/timeb.h>
#include "unordered_map"
#include "cache.h"
#include "comboProcessor.h"

extern int boardSize;

extern int searchLevel;

extern int comboLevel;

extern int timeOut;

extern int comboTimeOut;

extern bool debugEnable;

extern int comboCacheHit;

extern int comboCacheTotal;

extern bool fiveAttackTable[MAX_TABLE_SIZE];
extern bool fourDefenceTable[MAX_TABLE_SIZE];
extern bool fourAttackTable[MAX_TABLE_SIZE];
extern bool fourDefenceTable[MAX_TABLE_SIZE];
extern bool threeAttackTable[MAX_TABLE_SIZE];
extern bool threeDefenceTable[MAX_TABLE_SIZE][4][4];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

static int currentLevel;

static int currentExtend;

static point currentPointResult;

static pointHash loseSet;

static unordered_map<long long, point> cache;

static unordered_map<long long, point> cacheLast;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	timeOutEnable = false;
	//初始化
	initAnalyze();
	initGameMap(map);
	initScore(aiColor);

	//初始分析
	points ps = getNeighbor();
	sortPoints(&ps, aiColor);

	if (ps.count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (ps.count == 1) {
		gameResult.result = ps.list[0];
		return gameResult;
	}
	//连击搜索
	int node = 0;
	int lastNodeCount = 0;
	long long comboStart = getSystemTime();
	points threeWins;
	bool comboTimeLimit = false;
	bool comboFinish = false;
	bool otherComboFinish = false;
	loseSet.reset();
	for (int level = 3; level <= comboLevel; level += 2) {
		if (!comboFinish) {
			comboResult result = canKill(aiColor, level, comboStart, comboTimeOut);
			if (result.win) {
				if (result.fourWin) {
					gameResult.value = MAX_VALUE;
					gameResult.result = result.p;
					return gameResult;
				}
				else
				{
					threeWins.add(result.p);
				}
			}
			//如果搜的叶子数量相同则，说明已经搜到底
			if (node == result.node)
				comboFinish = true;
			if (result.timeOut)
				break;
			node = result.node;
			gameResult.combo = level;
		}


		//对方连击
		if (!otherComboFinish) {
			int nodeCount = 0;
			for (int i = 0; i < ps.count; i++)
			{
				point p = ps.list[i];
				if (loseSet.contains(p))
					continue;
				setPoint(p.x, p.y, aiColor, NULL, aiColor);
				comboResult result = canKill(getOtherColor(aiColor), level, comboStart, comboTimeOut);
				setPoint(p.x, p.y, NULL, aiColor, aiColor);
				if (result.win) {
					loseSet.add(p);
				}
				nodeCount += result.node;
				if (result.timeOut) {
					comboTimeLimit = true;
					break;
				}
			}
			if (comboTimeLimit)
				break;
			//如果搜的叶子数量相同则，说明已经搜到底
			if (nodeCount == lastNodeCount)
				otherComboFinish = true;
			lastNodeCount = nodeCount;
			gameResult.combo = level;
		}
		if (debugEnable) {
			printf("combo level %d finish %lld ms\n", level, getSystemTime() - comboStart);
		}
	}
	if (debugEnable) {
		printf("three wins ");
		printPoints(threeWins);
	}
	if (debugEnable) {
		printf("loseset ");
		printHash(loseSet);
	}

	if (debugEnable) {
		printf("combo cache total %d hit %d\n", comboCacheTotal, comboCacheHit);
	}

	//检查三连击的胜利是否有效
	for (int i = 0; i < threeWins.count; i++) {
		point p = threeWins.list[i];
		if (!loseSet.contains(p)) {
			gameResult.value = MAX_VALUE;
			gameResult.result = p;
			return gameResult;
		}
	}

	//如果只有一个节点不败，则执行该节点
	if (ps.count - 1 == loseSet.count) {
		for (int i = 0; i < ps.count; i++) {
			point p = ps.list[i];
			if (!loseSet.contains(p)) {
				gameResult.result = p;
				return gameResult;
			}
		}
	}

	//如果必败，则朴素搜索
	if (ps.count == loseSet.count) {
		loseSet.reset();
	}
	//得分搜索
	searchStartTime = getSystemTime();
	cacheLast.clear();
	cache.clear();
	for (int level = 2; level <= searchLevel; level += 2)
	{
		long long t = getSystemTime();
		nodeCount = 0;

		cacheLast = cache;
		cache.clear();

		currentLevel = level;
		currentExtend = 0;
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
			if (level > 2 && loseSet.count == ps.count) {
				return gameResult;
			}
			gameResult.result = currentPointResult;
			int speed = 0;
			if ((getSystemTime() - t) > 0)
				speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
			gameResult.speed = speed;
			gameResult.node = nodeCount;
			gameResult.value = value;
			gameResult.level = level;
			gameResult.extend = currentExtend;
			if (debugEnable) {
				printf("level %d, extend %d, value %d, speed %d k, time %lld ms\n", level, currentExtend, value, speed, getSystemTime() - t);
				printMapWithStar(map, currentPointResult);
			}
			if (value == MAX_VALUE)
				break;
		}
		if (getSystemTime() - searchStartTime > timeOut / 4) {
			timeOutEnable = true;
		}
	}

	return gameResult;
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
	long long hashCode = getMapHashCode();

	//获取扩展节点
	points ps;

	int value = getScoreValue();
	if (color != aiColor)
		value = -value;
	if (value > alpha && value < beta) {
		if (canKillThree(color, currentLevel)) {
			return MAX_VALUE;
		}
	}
	else {
		if (canKillFour(color, currentLevel)) {
			return MAX_VALUE;
		}
	}

	//叶子分数计算
	if (level == 0) {
		int value = getScoreValue();
		if (color != aiColor)
			value = -value;
		bool needExpend = false;
		if (value > alpha && value < beta && extend < currentLevel) {
			ps = getNeighbor();
			for (int i = 0; i < ps.count; i++) {
				if (needExpend)
					break;
				point p = ps.list[i];
				for (int k = 0; k < 4; k++) {
					int key = getMapLineKey(p.x, p.y, k, color);
					point leftPoint = point(p.x - 5 * directX[k], p.y - 5 * directY[k]);
					point rightPoint = point(p.x + 5 * directX[k], p.y + 5 * directY[k]);
					int left = getPointTableColor(leftPoint.x, leftPoint.y, color);
					int right = getPointTableColor(rightPoint.x, rightPoint.y, color);
					if (fiveAttackTable[key]) {
						return MAX_VALUE;
					}
					if (fourDefenceTable[key] || threeDefenceTable[key][left][right] || fourAttackTable[key] || threeAttackTable[key]) {
						needExpend = true;
						break;
					}
				}
			}

		}
		if (needExpend) {
			level += 2;
			extend += 2;
			currentExtend = extend > currentExtend ? extend : currentExtend;
		}
		else
			return value;
	}

	if (ps.count == 0) {
		ps = getNeighbor();
	}

	//输赢判定
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		for (int k = 0; k < 4; k++) {
			int key = getMapLineKey(p.x, p.y, k, color);
			if (fiveAttackTable[key]) {
				return MAX_VALUE;
			}
		}
	}

	//排序
	sortPoints(&ps, color);

	//调整最优节点顺序
	if (cacheLast.find(hashCode) != cacheLast.end()) {
		point p = cacheLast[hashCode];
		for (int i = 0; i < ps.count; i++)
			if (ps.list[i].x == p.x && ps.list[i].y == p.y) {
				for (int j = i; j > 0; j--) {
					ps.list[j] = ps.list[j - 1];
				}
				ps.list[0] = p;
				break;
			}
	}

	//遍历扩展节点
	int extreme = MIN_VALUE;
	points extremePoints;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];

		setPoint(p.x, p.y, color, NULL, aiColor);
		int value;
		//先查必败表
		if (level == currentLevel && extend == 0 && loseSet.contains(p)) {
			value = MIN_VALUE;
		}
		else {
			if (i == 0)
				value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
			else {
				//零窗口测试
				value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha, extend);
				if (value > alpha && value < beta) {
					value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
				}
			}
		}
		setPoint(p.x, p.y, NULL, color, aiColor);

		if (value >= extreme) {
			if (value > extreme) {
				extremePoints.clear();
			}
			extreme = value;
			extremePoints.add(p);

			if (value > alpha) {
				alpha = value;
				if (value > beta) {
					cache[hashCode] = p;
					return value;
				}
			}
		}

		if (level == currentLevel && extend == 0) {
			if (debugEnable)
				printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);

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
		currentPointResult = extremePoint;
	}

	cache[hashCode] = extremePoint;
	return extreme;
}

void setPoint(int px, int py, Color color, Color forwardColor, Color aiColor) {
	updateScore(px, py, color, forwardColor, aiColor);
	setColor(px, py, color);
}