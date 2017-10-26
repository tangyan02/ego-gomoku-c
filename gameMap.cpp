#include "stdafx.h"
#include "gameMap.h"
#include "stdlib.h"
#include "analyzer.h"
#include "vector"
#include <time.h>  

int **map;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

static long long weightBlack[20][20];
static long long weightWhite[20][20];

static long long hashCode = 0;
static int neighborCount[20][20];

Color ** getMap() {
	return map;
}

void addNeighborCount(int x, int y, bool isAdd) {
	if (isAdd)
		neighborCount[x][y]++;
	else
		neighborCount[x][y]--;
}

void updateNeighbor(int i, int j, bool isAdd, Color pointColor) {
	for (int k = 0; k < 8; k++) {
		int x = i + directX[k];
		int y = j + directY[k];
		if (reachable(x, y)) {
			Color color = map[x][y];
			if (color == NULL) {
				addNeighborCount(x, y, isAdd);
			}
			else {
				if (color == pointColor) {
					int x1 = i + directX[k] * 3;
					int y1 = j + directY[k] * 3;
					int x2 = i - directX[k] * 2;
					int y2 = j - directY[k] * 2;
					if (reachable(x1, y1)) {
						if (map[x1][y1] == NULL) {
							addNeighborCount(x1, y1, isAdd);
						}
					}
					if (reachable(x2, y2)) {
						if (map[x2][y2] == NULL) {
							addNeighborCount(x2, y2, isAdd);
						}
					}
				}
			}
		}
	}
}

void initGameMap(Color** value) {
	map = value;
	//初始化哈希权值
	srand((unsigned)time(NULL));

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			for (int k = 0; k < 4; k++) {
				weightBlack[i][j] = weightBlack[i][j] << 15 | rand();
				weightWhite[i][j] = weightWhite[i][j] << 15 | rand();
			}
		}

	//初始化地图和哈希码
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			Color color = map[i][j];
			if (color != NULL) {
				updateHashCode(i, j, color);
				bool isAdd = color == NULL ? false : true;
				updateNeighbor(i, j, true, color);
			}
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			for (int k = 0; k < 4; k++) {
				setLineKey(i, j, k, BLACK);
				setLineKey(i, j, k, WHITE);
			}
		}
}

void setColor(int x, int y, Color color)
{
	if (color != NULL)
		updateHashCode(x, y, color);
	else
		updateHashCode(x, y, map[x][y]);

	bool isAdd = color == NULL ? false : true;
	Color pointColor = isAdd ? color : map[x][y];
	map[x][y] = color;
	updateNeighbor(x, y, isAdd, pointColor);
	updatePointKey(x, y);
}

Color getColor(int x, int y)
{
	return map[x][y];
}

points getPointLinesNeighbor(int px, int py) {
	points neighbor = getNeighbor();
	pointHash hash = pointHash();
	for (int i = 0; i < neighbor.count; i++) {
		point p = neighbor.list[i];
		hash.add(p);
	}
	points result;
	for (int i = 0; i < 8; i++) {
		int x = px;
		int y = py;
		for (int k = 0; k < 4; k++) {
			x += directX[i];
			y += directY[i];
			if (!reachable(x, y)) {
				break;
			}
			if (map[x][y] != NULL)
				continue;
			point newPoint = point(x, y);
			if (hash.contains(newPoint)) {
				result.add(newPoint);
			}
		}
	}
	return result;
}

points getNeighbor() {
	points result;
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			if (neighborCount[i][j] > 0 && map[i][j] == NULL)
				result.add(point(i, j));
	//printMap(map);
	return result;
}

bool reachable(int x, int y)
{
	if (x < 0 || y < 0 || x >= boardSize || y >= boardSize)
		return false;
	return true;
}


long long getMapHashCode()
{
	return hashCode;
}

void updateHashCode(int x, int y, Color color)
{
	if (color != NULL) {
		if (color == BLACK) {
			hashCode ^= weightBlack[x][y];
		}
		if (color == WHITE) {
			hashCode ^= weightWhite[x][y];
		}
	}
}