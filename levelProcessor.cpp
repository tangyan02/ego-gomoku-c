#include "levelProcessor.h"
#include "PointsFactory.h"
#include "analyzer.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "stdafx.h"
#include <map>
#ifdef __GNUC__
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

map<int, hash_map<int, point> > neighborSortTree[3];

int singleScore[3][20][20];

extern int boardSize;

static int scoreList[512];

static int directX[] = {0, 1, 1, 1};

static int directY[] = {1, 1, 0, -1};

extern int neighborCount[20][20];

extern int blackLineKey[20][20][4];
extern int whiteLineKey[20][20][4];
extern int patternScore[PATTERN_SIZE];

extern points moveHistory;

static int getScore(int x, int y, Color color)
{
	int value = 0;
	for (int k = 0; k < 4; k++)
	{
		if (color == BLACK)
		{
			value += patternScore[blackLineKey[x][y][k]];
			value += patternScore[whiteLineKey[x][y][k]] / 2;
		}
		if (color == WHITE)
		{
			value += patternScore[whiteLineKey[x][y][k]];
			value += patternScore[blackLineKey[x][y][k]] / 2;
		}
	}
	return value;
}

void updateNeighborSortTree(int x, int y, bool isAdd)
{
	if (neighborCount[x][y] > 1)
	{
		return;
	}
	for (Color color = 1; color <= 2; color++)
	{
		if (singleScore[color][x][y] >= 0 )
		{
			int score = singleScore[color][x][y];
			int pointKey = x * boardSize + y;

			map<int, hash_map<int, point> >::iterator item = neighborSortTree[color].find(score);
			if (item != neighborSortTree[color].end())
			{
				item->second.erase(pointKey);
			}

			singleScore[color][x][y] = -1;
		}
		if (neighborCount[x][y] > 0)
		{
			int score = getScore(x, y, color);
			int pointKey = x * boardSize + y;

			map<int, hash_map<int, point> >::iterator item = neighborSortTree[color].find(score);
			if (item == neighborSortTree[color].end())
			{
				hash_map<int, point> hashMap;
				hashMap.insert(pair<int, point>(pointKey, point(x, y)));
				neighborSortTree[color].insert(pair<int, hash_map<int, point> >(score, hashMap));
			}
			else
			{
				item->second.insert(pair<int, point>(pointKey, point(x, y)));
			}

			singleScore[color][x][y] = score;
		}
	}
}

void qsort(point *list, int score[], int l, int r)
{
	int x = l;
	int y = r;
	int mid = score[(x + y) / 2];
	while (x < y)
	{
		while (score[x] > mid)
			x++;
		while (score[y] < mid)
			y--;
		if (x <= y)
		{
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
	if (x < r)
		qsort(list, score, x, r);
	if (l < y)
		qsort(list, score, l, y);
}

void selectAndSortPoints(points *neighbors, Color color)
{
	int count = 0;
	map<int, hash_map<int, point> >::iterator iter;
	for(iter = neighborSortTree[color].begin(); iter!=neighborSortTree[color].end(); iter++){
		count += iter->second.size();
		hash_map<int, point>::iterator item = iter->second.begin();
		for (; item != iter->second.end();item++){
			for (int x = 0; x < 20;x++)
				for (int y = 0; y < 20;y++){
					if(item->second.x == x && item->second.y == y){
						printf("%d,%d  ", x, y);
					}
				}
		}
	}
	printf("\n");
	for (int i = 0; i < neighbors->count;i++)
	{
		for (int x = 0; x < 20; x++)
			for (int y = 0; y < 20; y++)
			{
				if (neighbors->list[i].x == x && neighbors->list[i].y == y)
				{
					printf("%d,%d  ", x, y);
				}
			}
	}
	if (count != neighbors->count)
	{
		printf("\nnum not same!!! color=%d count=%d neighborsCount=%d\n",color , count, neighbors->count);
	}
	else
	{
		printf("num same!!!\n");
	}

	//������
	if (tryFiveAttack(color, neighbors))
		return;
	//�³���
	if (tryFourDefence(color, neighbors))
		return;
	//�Ը�����
	if (tryThreeDefence(color, neighbors))
		return;

	//����
	point *list = neighbors->list;

	for (int i = 0; i < neighbors->count; i++)
	{
		scoreList[i] = getScore(list[i].x, list[i].y, color);
	}
	qsort(list, scoreList, 0, neighbors->count - 1);
}
