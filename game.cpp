#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "console.h"
#include <sys/timeb.h>
#include "unordered_map"
#include "PointsFactory.h"
#include "analyzer.h"
#include "comboResult.h"
#include "comboProcessor.h"

extern int boardSize;

extern int searchLevel;

extern int comboLevel;

extern int timeOut;

extern int comboTimeOut;

extern bool debugEnable;

extern bool piskvorkMessageEnable;

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

static int currentLevel;

static int maxExtend;

static int extendNodeCount;

static int innerComboNodeCount;

static int innerComboSucNodeCount;

static int cacheHitCount;

static int cacheCorretCount;

static point currentPointResult;

static gameResult lastResult;

static pointHash loseSet;

static unordered_map<long long, point> cache;

static unordered_map<long long, point> cacheLast;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

bool tryComboSearchIteration(points * neighbors, Color aiColor, gameResult *gameResult) {

	gameResult->combo = 0;
	long long targerTime = getSystemTime() + comboTimeOut;
	bool selfSearch = true;
	bool otherSearch[256];
	int loseCount = 0;
	int selfComboLevel = 0;
	int otherComboLevel = 0;
	for (int i = 0; i < neighbors->count; i++) {
		otherSearch[i] = true;
	}
	for (int level = 3; level <= comboLevel; level += 2) {
		if (getSystemTime() > targerTime) {
			if (piskvorkMessageEnable) {
				printf("MESSAGE combo time out \n");
			}
			break;
		}
		if (selfSearch) {
			comboResult result = kill(aiColor, level, targerTime);
			if (debugEnable) {
				printf("level %d value %d comboLevel %d\n", level, result.canWin, comboLevel);
			}
			gameResult->combo = level;
			if (result.canWin) {
				gameResult->value = MAX_VALUE;
				gameResult->result = point(result.p.x, result.p.y);
				return true;
			}

			if (!result.isDeep) {
				selfSearch = false;;
				selfComboLevel = max(level, selfComboLevel);
			}
		}

		
		for (int i = 0; i < neighbors->count; i++) {
			if (otherSearch[i]) {
				point p = point(neighbors->list[i].x, neighbors->list[i].y);
				//lose remove
				if (loseSet.contains(p)) {
					continue;
				}
				move(p.x, p.y, aiColor);
				comboResult result = kill(getOtherColor(aiColor), level, targerTime);
				gameResult->combo = level;
				undoMove(p.x, p.y, aiColor);
				if (result.canWin) {
					if (!loseSet.contains(p.x, p.y)) {
						loseCount++;
					}
					loseSet.add(p);
				}
				if (!result.isDeep) {
					otherSearch[i] = false;
					otherComboLevel = max(level, otherComboLevel);
				}
			}
		}
	}
	if (piskvorkMessageEnable) {
		printf("MESSAGE combo level. self %d , other %d \n", selfComboLevel, otherComboLevel);
		printf("MESSAGE lose point count %d\n", loseCount);
	}
	return false;
}

static points findHistoryPath(Color selfColor) {
	points result;
	Color color = selfColor;
	while (true) {
		if (cacheLast.find(getMapHashCode()) == cacheLast.end()) {
			break;
		}
		point p = cacheLast[getMapHashCode()];
		result.add(p);
		move(p.x, p.y, color);
		color = getOtherColor(color);
	}
	for (int i = result.count; i > 0; i--) {
		color = getOtherColor(color);
		undoMove(result.list[i-1].x, result.list[i-1].y, color);
	}
	if (piskvorkMessageEnable) {
		printf("MESSAGE hitory: ");
		printPoints(result);
	}
	return result;
}

bool tryScoreSearchIteration(points * neighbors, Color aiColor, gameResult *gameResult) {
	//if all lose points , normal search
	bool loseFlag = false;
	if (neighbors->count == loseSet.count) {
		loseSet.reset();
		loseFlag = true;
	}

	//normal search
	timeOutEnable = false;
	searchStartTime = getSystemTime();

	points historyPoints = findHistoryPath(aiColor);

	int startLevel = 2;
	if (historyPoints.count > 0) {
		if (piskvorkMessageEnable) {
			printf("MESSAGE find history path. point (%d, %d), path lenth: %d, last level:%d ,\n", historyPoints.list[0].x, historyPoints.list[0].y, historyPoints.count, lastResult.level - 2);
		}
		if (!loseSet.contains(historyPoints.list[0])) {
			gameResult->result = historyPoints.list[0];
			gameResult->level = min(lastResult.level - 2, historyPoints.count);
			startLevel = min(lastResult.level, historyPoints.count + 2);
			if (startLevel % 2 == 1) {
				startLevel--;
			}
			cache = cacheLast;
		}
		else {
			cacheLast.clear();
			cache.clear();
		}
	}

	for (int level = startLevel; level <= searchLevel; level += 2 )
	{
		long long t = getSystemTime();
		nodeCount = 0;
		currentLevel = level;
		maxExtend = 0;
		extendNodeCount = 0;
		cacheHitCount = 0;
		cacheCorretCount = 0;
		innerComboNodeCount = 0;
		innerComboSucNodeCount = 0;

		cacheLast = cache;
		cache.clear();

		Color color = aiColor;
		int alpha = MIN_VALUE * 2;
		int beta = MAX_VALUE * 2;

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
			gameResult->maxExtend = maxExtend;
			gameResult->extendNode = extendNodeCount;
			gameResult->cacheHit = cacheHitCount;
			gameResult->cacheCorrect = cacheCorretCount;
			gameResult->innerComboNode = innerComboNodeCount;
			gameResult->innerComboSucNode = innerComboSucNodeCount;
			if (piskvorkMessageEnable)
			{
				//printf("MESSAGE level %d, value %d, (%d ,%d), speed %d k, node %d , max extend %d, extend node %d , cache hit: %d/%d , cost %lld ms\n",
				//	level,
				//	value,
				//	gameResult->result.x,
				//	gameResult->result.y,
				//	speed,
				//	nodeCount,
				//	maxExtend,
				//	extendNodeCount,
				//	cacheCorretCount,
				//	cacheHitCount,
				//	getSystemTime() - t);
					printf("MESSAGE level %d, value %d, (%d ,%d), speed %d k, node %d , max extend %d, extend node %d , cache hit: %d/%d ,inner combo rate %d/%d ,cost %lld ms\n",
					level,
					value,
					gameResult->result.x,
					gameResult->result.y,
					speed,
					nodeCount,
					maxExtend,
					extendNodeCount,
					cacheCorretCount,
					cacheHitCount,
					innerComboSucNodeCount,
					innerComboNodeCount,
					getSystemTime() - t);
			}
			if (debugEnable) {
				printMapWithStar(getMap(), currentPointResult);
			}
			if (value >= MAX_VALUE/2)
				break;
			if (value <= MIN_VALUE/2) {
				break;
			}
		}
		if (getSystemTime() - searchStartTime > timeOut / 4) {
			timeOutEnable = true;
		}
	}
	if (loseFlag)
		gameResult->value = MIN_VALUE;

	lastResult = *gameResult;
	return true;
}

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	//init 
	initPattern();
	clearPatternRecord();
	initGameMap(map);
	loseSet.reset();

	//init neighbors
	points* neighbors = PointsFactory::createPointNeighborPoints(0, 0);
	fillNeighbor(neighbors);
	selectAndSortPoints(*neighbors, aiColor);

	if (neighbors->count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (neighbors->count == 1) {
		gameResult.result = neighbors->list[0];
		return gameResult;
	}

	//combo search, extend can always instead combo search
	if (tryComboSearchIteration(neighbors, aiColor, &gameResult)) {
		return gameResult;
	}

	//normal search
	tryScoreSearchIteration(neighbors, aiColor, &gameResult);

	return gameResult;
}

bool moveHistoryBestToFirst(points * neighbors) {
	if (cacheLast.find(getMapHashCode()) != cacheLast.end()) {
		cacheHitCount++;
		point p = cacheLast[getMapHashCode()];
		for (int i = 0; i < neighbors->count; i++)
			if (neighbors->list[i].x == p.x && neighbors->list[i].y == p.y) {
				for (int j = i; j > 0; j--) {
					neighbors->list[j] = neighbors->list[j - 1];
				}
				neighbors->list[0] = p;
				break;
			}
		return true;
	}
	return false;
}

/* alpha beta serach
*/
static int dfs(int level, Color color, Color aiColor, int alpha, int beta, int extend) {
	if (getSystemTime() - searchStartTime > timeOut) {
		timeOutEnable = true;
	}
	if (timeOutEnable) {
		return 0;
	}
	nodeCount++;
	if (extend > 0) {
		extendNodeCount++;
	}	

	int scoreValue = getScoreValue(color, aiColor);



	if (level <= 1) 
	{

		if (scoreValue < beta && scoreValue > alpha) {
			innerComboNodeCount++;
			comboResult ret = killVCF(color, 100 + getSystemTime());
			if (ret.canWin == true) {
				innerComboSucNodeCount++;
				return MAX_VALUE + scoreValue;
			}
		}

		// extend
		if (scoreValue < beta && scoreValue > alpha && extend < currentLevel) {
			level += 2;
			extend += 2;
			if (extend > maxExtend) {
				maxExtend = extend;
			}
		}

		// leaf node get value
		if (level == 0) {
			return scoreValue;
		}
	}

	if (canWinCheck(color)) {
		//printMap(getMap());
		return MAX_VALUE + scoreValue;
		//return MAX_VALUE;
	}

	//get neighbors
	points* neighbors = PointsFactory::createPointNeighborPoints(level, extend);
	fillNeighbor(neighbors);

	//sort
	selectAndSortPoints(*neighbors, color);

	//user history best
	bool existHitory = moveHistoryBestToFirst(neighbors);

	//move and dfs
	point finalPoint;
    int extrem = MIN_VALUE;
	for (int i = 0; i < neighbors->count; i++) {
		point p = point(neighbors->list[i].x, neighbors->list[i].y);

		move(p.x, p.y, color);
		int value;
		//zero window search
		if (level == currentLevel && extend == 0 && loseSet.contains(p)) {
			value = MIN_VALUE + scoreValue;
		}
		else {
			if (i == 0) {
				value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
			}
			else {
				//try zero window
				value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha, extend);
				if (value > alpha && value < beta) {
					value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
				}
			}
		}
		undoMove(p.x, p.y, color);
		if (i == 0) {
			finalPoint = p;
		}

		if (value > extrem) {
			extrem = value;
		}

		if (value > alpha) {
			alpha = value;
			finalPoint = p;
			if (value > beta) {
				cache[getMapHashCode()] = p;

			}
		    if (value >= beta) {
				return beta;
		    }
		}

		if (level == currentLevel && extend == 0) {
			if (debugEnable)
				printf("(%d, %d) value: %d nodes: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);

			if (value >= MAX_VALUE/2) {
				currentPointResult = p;
				return value;
			}
			if (value <= MIN_VALUE/2) {
				if (!loseSet.contains(p))
					loseSet.add(p);
			}
		}
	}

	if (finalPoint.x == neighbors->list[0].x && finalPoint.y == neighbors->list[0].y && existHitory) {
		cacheCorretCount++;
	}

	if (neighbors->count > 0) {
		if (level == currentLevel && extend == 0) {
			 currentPointResult = point(finalPoint.x, finalPoint.y);
		}
		cache[getMapHashCode()] = finalPoint;
	}
	return alpha;
}