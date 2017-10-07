#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "analyzer.h"
#include "console.h"
#include <sys/timeb.h>

extern int boardSize;

extern int searchLevel;

extern int timeOut;

extern bool debugEnable;

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	searchStartTime = getSystemTime();
	timeOutEnable = false;
	//初始化
	initGameMap(map);
	initScore(aiColor);

	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(aiColor, neighbors);
	points ps = getExpandPoints(data, neighbors);
	int values[128];

	if (ps.count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (ps.count == 1) {
		gameResult.result = ps.list[0];
		return gameResult;
	}

	for (int level = 2; level <= searchLevel; level += 2)
	{
		long long t = getSystemTime();
		nodeCount = 0;
		point currentResult;
		int extreme = MIN_VALUE;
		Color color = aiColor;
		for (int i = 0; i < ps.count; i++) {
			point p = ps.list[i];
			setPoint(p, color, NULL, aiColor);
			int value = -dfs(level - 1, getOtherColor(color), MIN_VALUE, -extreme, aiColor);
			setPoint(p, NULL, color, aiColor);

			if (timeOutEnable) {
				setPoint(p, NULL, color, aiColor);
				break;
			}
			values[i] = value;
			if (debugEnable)
				printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - t);

			if (value >= extreme) {
				currentResult = p;
				extreme = value;

				if (extreme == MAX_VALUE) {
					gameResult.result = p;
					int speed = 0;
					if ((getSystemTime() - t) > 0)
						speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
					gameResult.speed = speed;
					gameResult.node = nodeCount;
					gameResult.value = extreme;
					gameResult.level = level;
					return gameResult;
				}
			}
		}
		if (timeOutEnable) {
			if (debugEnable)
				printf("time out\n");

			break;
		}
		//对下次迭代排序
		for (int i = 0; i < ps.count; i++) {
			for (int j = i; j < ps.count; j++) {
				if (values[i] < values[j]) {
					int tempValue = values[i];
					values[i] = values[j];
					values[j] = tempValue;
					point tempPoint = ps.list[i];
					ps.list[i] = ps.list[j];
					ps.list[j] = tempPoint;
				}
			}
		}

		gameResult.result = currentResult;
		int speed = 0;
		if ((getSystemTime() - t) > 0)
			speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
		gameResult.speed = speed;
		gameResult.node = nodeCount;
		gameResult.value = extreme;
		gameResult.level = level;

		if (debugEnable)
			printf("level %d ok, speed %d k \n", level, speed);
		if (getSystemTime() - searchStartTime > timeOut / 5) {
			timeOutEnable = true;
		}
	}

	return gameResult;
}

int dfs(int level, Color color, int alpha, int beta, Color aiColor) {
	if (getSystemTime() - searchStartTime > timeOut) {
		timeOutEnable = true;
	}
	if (timeOutEnable) {
		return 0;
	}
	//叶子分数计算
	if (level == 0) {
		nodeCount++;
		return getScoreValue();
	}
	//分析棋形
	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(color, neighbors);
	//输赢判定
	if (data.fiveAttack.count > 0) {
		if (color == aiColor) {
			return MAX_VALUE;
		}
		if (color == getOtherColor(aiColor)) {
			return MIN_VALUE;
		}
	}
	//计算扩展节点
	points ps = getExpandPoints(data, neighbors);
	//遍历扩展节点
	int extreme = MIN_VALUE;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		int value = -dfs(level - 1, getOtherColor(color), -beta, -alpha, aiColor);
		setPoint(p, NULL, color, aiColor);

		if (value > extreme) {
			extreme = value;
			if (value > alpha) {
				alpha = value;
				if (value > beta) {
					return value;
				}
			}
		}
	}
	return extreme;
}

void setPoint(point p, Color color, Color forwardColor, Color aiColor) {
	updateScore(p, color, forwardColor, aiColor);
	setColor(p.x, p.y, color);
}