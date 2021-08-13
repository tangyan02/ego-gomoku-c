#include "game.h"
#include "PointsFactory.h"
#include "analyzer.h"
#include "comboProcessor.h"
#include "comboResult.h"
#include "console.h"
#include "gameMap.h"
#include "levelProcessor.h"
#include "score.h"
#include "stdafx.h"
#include "unordered_map"
#include "winChecker.h"
#include <sys/timeb.h>

extern int boardSize;

extern int searchLevel;

extern int comboLevel;

extern int timeOut;

extern int comboTimeOut;

extern bool debugEnable;

static int directX[] = {0, 1, 1, 1};

static int directY[] = {1, 1, 0, -1};

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

static int currentLevel;

static point currentPointResult;

static pointHash loseSet;

static unordered_map<long long, point> cache;

static unordered_map<long long, point> cacheLast;

long long getSystemTime()
{
    struct timeb t;
    ftime(&t);
    return 1000 * t.time + t.millitm;
}

bool tryComboSearchIteration(points *neighbors, Color aiColor, gameResult *gameResult)
{

    gameResult->combo = 0;
    long long targerTime = getSystemTime() + comboTimeOut;
    bool selfSearch = true;
    bool otherSearch[256];
    int loseCount = 0;
    for (int i = 0; i < neighbors->count; i++)
    {
        otherSearch[i] = true;
    }
    for (int level = 3; level <= comboLevel; level += 2)
    {
        if (getSystemTime() > targerTime)
        {
            break;
        }
        if (selfSearch)
        {
            comboResult result = kill(aiColor, level, targerTime);
            if (debugEnable)
            {
                printf("level %d value %d comboLevel %d\n", level, result.canWin, comboLevel);
            }
            gameResult->combo = level;
            if (result.canWin)
            {
                gameResult->value = MAX_VALUE;
                gameResult->result = point(result.p.x, result.p.y);
                return true;
            }

            if (!result.isDeep)
            {
                selfSearch = false;
                ;
            }
        }

        //������չ�ڵ�
        for (int i = 0; i < neighbors->count; i++)
        {
            if (otherSearch[i])
            {
                point p = point(neighbors->list[i].x, neighbors->list[i].y);
                //�Ȳ�ذܱ�
                if (loseSet.contains(p))
                {
                    continue;
                }
                move(p.x, p.y, aiColor);
                comboResult result = kill(getOtherColor(aiColor), level, targerTime);
                gameResult->combo = level;
                undoMove(p.x, p.y, aiColor);
                if (result.canWin)
                {
                    if (!loseSet.contains(p.x, p.y))
                    {
                        loseCount++;
                    }
                    loseSet.add(point(p.x, p.y));
                }
                if (!result.isDeep)
                {
                    otherSearch[i] = false;
                }
            }
        }
    }
    // printf("MESSAGE lose point count %d\n", loseCount);
    return false;
}

bool tryScoreSearchIteration(points *neighbors, Color aiColor, gameResult *gameResult)
{
    //����ذܣ�����������
    bool loseFlag = false;
    if (neighbors->count == loseSet.count)
    {
        loseSet.reset();
        loseFlag = true;
    }
    //�÷�����
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

        if (timeOutEnable)
        {
            if (debugEnable)
                printf("time out\n");
            break;
        }

        if (!timeOutEnable)
        {
            if (level > 2 && loseSet.count == neighbors->count)
            {
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
            // printf("MESSAGE level %d, value %d, (%d ,%d), speed %d k, cost %lld ms\n", level, value, gameResult->result.x, gameResult->result.y, speed, getSystemTime() - t);
            if (debugEnable)
            {
                printMapWithStar(getMap(), currentPointResult);
            }
            if (value == MAX_VALUE)
                break;
        }
        if (getSystemTime() - searchStartTime > timeOut / 4)
        {
            timeOutEnable = true;
        }
    }
    if (loseFlag)
        gameResult->value = MIN_VALUE;
    return true;
}

gameResult search(Color aiColor, Color **map)
{
    gameResult gameResult;
    //��ʼ��
    initPattern();
    clearPatternRecord();
    initGameMap(map);
    loseSet.reset();

    //��ʼ����
    points *neighbors = PointsFactory::createPointNeighborPoints(0, 0);
    fillNeighbor(neighbors);
    selectAndSortPoints(neighbors, aiColor);

    if (neighbors->count == 0)
    {
        gameResult.result = point(boardSize / 2, boardSize / 2);
        return gameResult;
    }

    if (neighbors->count == 1)
    {
        gameResult.result = neighbors->list[0];
        return gameResult;
    }

    //��ɱ
    if (tryComboSearchIteration(neighbors, aiColor, &gameResult))
    {
        return gameResult;
    }

    //�÷ֵ�������
    tryScoreSearchIteration(neighbors, aiColor, &gameResult);

    return gameResult;
}

void moveHistoryBestToFirst(points *neighbors)
{
    if (cacheLast.find(getMapHashCode()) != cacheLast.end())
    {
        point p = cacheLast[getMapHashCode()];
        for (int i = 0; i < neighbors->count; i++)
            if (neighbors->list[i].x == p.x && neighbors->list[i].y == p.y)
            {
                for (int j = i; j > 0; j--)
                {
                    neighbors->list[j] = neighbors->list[j - 1];
                }
                neighbors->list[0] = p;
                break;
            }
    }
}

/* �㴰�ڲ��Է�
*/
int dfs(int level, Color color, Color aiColor, int alpha, int beta, int extend)
{
    if (getSystemTime() - searchStartTime > timeOut)
    {
        timeOutEnable = true;
    }
    if (timeOutEnable)
    {
        return 0;
    }
    nodeCount++;

    if (level <= 1)
    {
        int value = getScoreValue(color, aiColor);

        /*
		if (value > alpha && value < beta) {
			comboResult result = kill(color, currentLevel + 1, getSystemTime() + 1000);
			if (result.canWin) {
				return MAX_VALUE;
			}
		}
		*/

        //����

        if (value < beta && value > alpha && extend < currentLevel)
        {
            level += 2;
            extend += 2;
        }

        //Ҷ�ӷ�������
        if (level == 0)
        {
            //int value = getScoreValue(color, aiColor);
            return value;
        }
    }

    if (canWinCheck(color))
    {
        return MAX_VALUE;
    }

    //��ȡ��չ�ڵ�
    points *neighbors = PointsFactory::createPointNeighborPoints(level, extend);
    fillNeighbor(neighbors);

    //����
    selectAndSortPoints(neighbors, color);

    //�������Žڵ�˳��
    moveHistoryBestToFirst(neighbors);

    //������չ�ڵ�
    int extreme = MIN_VALUE;
    points *extremePoints = PointsFactory::createDfsTempPoints(level);
    for (int i = 0; i < neighbors->count; i++)
    {
        point p = point(neighbors->list[i].x, neighbors->list[i].y);

        move(p.x, p.y, color);
        int value;
        //�Ȳ�ذܱ�
        if (level == currentLevel && extend == 0 && loseSet.contains(p))
        {
            value = MIN_VALUE;
        }
        else
        {
            if (i == 0)
            {
                value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
            }
            else
            {
                //�㴰�ڲ���
                value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha, extend);
                if (value > alpha && value < beta)
                {
                    value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha, extend);
                }
            }
        }
        undoMove(p.x, p.y, color);
        if (value >= extreme)
        {
            if (value > extreme)
            {
                extremePoints->clear();
            }
            extreme = value;
            extremePoints->add(p);

            if (value > alpha)
            {
                alpha = value;
                if (value > beta)
                {
                    cache[getMapHashCode()] = p;
                    return value;
                }
            }
        }

        if (level == currentLevel && extend == 0)
        {
            if (debugEnable)
                printf("(%d, %d) value: %d nodes: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);

            if (value == MAX_VALUE)
            {
                currentPointResult = p;
                return value;
            }
            if (value == MIN_VALUE)
            {
                if (!loseSet.contains(p))
                    loseSet.add(p);
            }
        }
    }

    point extremePoint = extremePoints->list[rand() % extremePoints->count];
    if (level == currentLevel && extend == 0)
    {
        currentPointResult = point(extremePoint.x, extremePoint.y);
    }

    cache[getMapHashCode()] = extremePoint;
    return extreme;
}