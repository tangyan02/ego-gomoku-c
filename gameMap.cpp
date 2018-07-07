#include "stdafx.h"
#include "gameMap.h"
#include "stdlib.h"
#include "analyzer.h"
#include "vector"
#include <time.h>  
#include "patternRecorder.h"

int **map;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

static long long weightBlack[20][20];
static long long weightWhite[20][20];

static long long hashCode = 0;
static int neighborCount[20][20];

static int left, right, top, bottom;

Color ** getMap() {
	return map;
}

void addNeighborCount(int x, int y, bool isAdd) {
	if (isAdd)
		neighborCount[x][y]++;
	else
		neighborCount[x][y]--;
}

void updateSide(int i, int j) {
	if (j - 2 < left) left = j - 2;
	if (j + 2 > right) right = j + 2;
	if (i - 2 < top) top = i - 2;
	if (i + 2 > bottom) bottom = i + 2;
	if (left < 0) left = 0;
	if (top < 0) top = 0;
	if (right >= boardSize) right = boardSize - 1;
	if (bottom >= boardSize) bottom = boardSize - 1;
}

void updateNeighbor(int i, int j, bool isAdd, Color pointColor) {
	updateSide(i, j);
	for (int x = i - 2; x <= i + 2; x++)
		for (int y = j - 2; y <= j + 2; y++)
			if (reachable(x, y)) {
				Color color = map[x][y];
				if (i == x && j == y)
					continue;
				if(color == NULL)
					addNeighborCount(x, y, isAdd);
			}
}

void initGameMap(Color** value) {
	map = value;
	right = 0;
	bottom = 0;
	left = boardSize - 1;
	top = boardSize - 1;
	//初始化哈希权值
	srand((unsigned)time(NULL));

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			for (int k = 0; k < 4; k++) {
				weightBlack[i][j] = weightBlack[i][j] << 15 | rand();
				weightWhite[i][j] = weightWhite[i][j] << 15 | rand();
			}
		}


	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			neighborCount[i][j] = 0;
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
			if (neighborCount[x][y] > 0 && map[x][y] == NULL) {
				result.add(point(x, y));
			}
		}
	}
	return result;
}

points getNeighbor() {
	points result;
	for (int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++)
			if (neighborCount[i][j] > 0 && map[i][j] == NULL)
				result.add(point(i, j));
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