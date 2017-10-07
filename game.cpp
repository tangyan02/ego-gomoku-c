#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "analyzer.h"
#include "console.h"
#include "cache.h"
#include <sys/timeb.h>


extern int boardSize;

extern int searchLevel;

static int nodeCount;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

point search(Color aiColor, Color** map)
{
	long long t = getSystemTime();
	//初始化
	initGameMap(map);
	initScore(aiColor);
	initCache(5000000);

	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(aiColor, neighbors);
	points ps = getExpandPoints(data, neighbors);
	int values[128];

	point result;
	for (int level = 2; level <= searchLevel; level+=2)
	{
		cacheReset();
		nodeCount = 0;
		point currentResult;
		int extreme = MIN_VALUE;
		Color color = aiColor;
		for (int i = 0; i < ps.count; i++) {
			point p = ps.list[i];
			setPoint(p, color, NULL, aiColor);
			int value = dfs(level - 1, getOtherColor(color), extreme, MAX_VALUE, aiColor);
			values[i] = value;
			printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - t);
			if (value >= extreme) {
				currentResult = p;
				extreme = value;
			}
			setPoint(p, NULL, color, aiColor);
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

		printf("level %d ok \n", level);
		result = currentResult;
	}
	return result;
}

int dfs(int level, Color color, int parentMax, int parentMin, Color aiColor) {
	//缓存查询
	if (containsSearchKey(getMapHashCode())) {
		return getSearchValue(getMapHashCode());
	}
	//叶子分数计算
	if (level == 0) {
		nodeCount++;
		addSearchEntry(getMapHashCode(), getScoreValue());
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
	int extreme = color == aiColor ? MIN_VALUE : MAX_VALUE;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		if (color == aiColor) {
			int value = dfs(level - 1, getOtherColor(color), extreme, MAX_VALUE, aiColor);
			if (value > parentMin) {
				setPoint(p, NULL, color, aiColor);
				addSearchEntry(getMapHashCode(), value);
				return value;
			}
			if (value > extreme) {
				extreme = value;
				//如果能赢了，则直接剪掉后面的情形
				if (extreme == MAX_VALUE) {
					setPoint(p, NULL, color, aiColor);
					addSearchEntry(getMapHashCode(), extreme);
					return extreme;
				}
			}
		}
		if (color != aiColor) {
			int value = dfs(level - 1, getOtherColor(color), MIN_VALUE, extreme, aiColor);
			if (value < parentMax) {
				setPoint(p, NULL, color, aiColor);
				addSearchEntry(getMapHashCode(), value);
				return value;
			}
			if (value < extreme) {
				extreme = value;
				//如果已经输了，则直接剪掉后面的情形
				if (extreme == MIN_VALUE) {
					setPoint(p, NULL, color, aiColor);
					addSearchEntry(getMapHashCode(), extreme);
					return extreme;
				}
			}
		}
		setPoint(p, NULL, color, aiColor);
	}
	//缓存写入
	addSearchEntry(getMapHashCode(), extreme);
	return extreme;
}

void setPoint(point p, Color color, Color forwardColor, Color aiColor) {
	updateScore(p, color, forwardColor, aiColor);
	setColor(p.x, p.y, color);
}