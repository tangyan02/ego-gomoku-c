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

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	timeOutEnable = false;
	//��ʼ��
	initGameMap(map);
	initScore(aiColor);

	//��ʼ����
	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(aiColor, neighbors);
	points ps = getExpandPoints(data, neighbors);

	if (ps.count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (ps.count == 1) {
		gameResult.result = ps.list[0];
		return gameResult;
	}
	//��������
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
			//����ѵ�Ҷ��������ͬ��˵���Ѿ��ѵ���
			if (node == result.node)
				comboFinish = true;
			if (result.timeOut)
				break;
			node = result.node;
			gameResult.combo = level;
		}


		//�Է�����
		if (!otherComboFinish) {
			int nodeCount = 0;
			for (int i = 0; i < ps.count; i++)
			{
				point p = ps.list[i];
				if (loseSet.contains(p))
					continue;
				setPoint(p, aiColor, NULL, aiColor);
				comboResult result = canKill(getOtherColor(aiColor), level, comboStart, comboTimeOut);
				setPoint(p, NULL, aiColor, aiColor);
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
			//����ѵ�Ҷ��������ͬ��˵���Ѿ��ѵ���
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

	//�����������ʤ���Ƿ���Ч
	for (int i = 0; i < threeWins.count; i++) {
		point p = threeWins.list[i];
		if (!loseSet.contains(p)) {
			gameResult.value = MAX_VALUE;
			gameResult.result = p;
			return gameResult;
		}
	}

	//���ֻ��һ���ڵ㲻�ܣ���ִ�иýڵ�
	if (ps.count - 1 == loseSet.count) {
		for (int i = 0; i < ps.count; i++) {
			point p = ps.list[i];
			if (!loseSet.contains(p)) {
				gameResult.result = p;
				return gameResult;
			}
		}
	}

	//����ذܣ�����������
	if (ps.count == loseSet.count) {
		loseSet.reset();
	}
	//�÷�����
	searchStartTime = getSystemTime();
	cacheLast.clear();
	cache.clear();
	for (int level = 2; level <= searchLevel; level += 2)
	{
		long long t = getSystemTime();
		nodeCount = 0;

		cacheReset();
		cacheLast = cache;
		cache.clear();

		currentLevel = level;
		Color color = aiColor;
		int alpha = MIN_VALUE;
		int beta = MAX_VALUE;

		int value = dfs(level, color, aiColor, alpha, beta);

		if (timeOutEnable) {
			if (debugEnable)
				printf("time out\n");
			break;
		}

		if (!timeOutEnable) {
			gameResult.result = currentPointResult;
			int speed = 0;
			if ((getSystemTime() - t) > 0)
				speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
			gameResult.speed = speed;
			gameResult.node = nodeCount;
			gameResult.value = value;
			gameResult.level = level;
			if (debugEnable) {
				printf("level %d, speed %d k, time %lld ms\n", level, speed, getSystemTime() - t);
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

/* �㴰�ڲ��Է�
*/
int dfs(int level, Color color, Color aiColor, int alpha, int beta) {
	if (getSystemTime() - searchStartTime > timeOut) {
		timeOutEnable = true;
	}
	if (timeOutEnable) {
		return 0;
	}
	nodeCount++;
	long long hashCode = getMapHashCode();

	//Ҷ�ӷ�������
	if (level == 0) {
		return getScoreValue();
	}
	//��������
	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(color, neighbors);
	//��Ӯ�ж�
	if (data.fiveAttack.count > 0) {
		return MAX_VALUE;
	}
	//������չ�ڵ�
	points ps = getExpandPoints(data, neighbors);

	//�������Žڵ�˳��
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

	//������չ�ڵ�
	int extreme = MIN_VALUE;
	points extremePoints;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];

		setPoint(p, color, NULL, aiColor);
		int value;
		//�Ȳ�ذܱ�
		if (level == currentLevel && loseSet.contains(p)) {
			value = MIN_VALUE;
		}
		else {
			if (i == 0)
				value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha);
			else {
				//�㴰�ڲ���
				value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha);
				if (value > alpha && value < beta) {
					value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha);
				}
			}
		}
		setPoint(p, NULL, color, aiColor);

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

		if (level == currentLevel) {
			if (debugEnable)
				printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);
		}

	}

	point extremePoint = extremePoints.list[rand() % extremePoints.count];
	if (level == currentLevel) {
		currentPointResult = extremePoint;
	}

	cache[hashCode] = extremePoint;
	return extreme;
}

void setPoint(point p, Color color, Color forwardColor, Color aiColor) {
	updateScore(p, color, forwardColor, aiColor);
	setColor(p.x, p.y, color);
}