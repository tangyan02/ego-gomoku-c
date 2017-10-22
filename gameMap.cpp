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

static vector<points> history;

Color ** getMap() {
	return map;
}

void updateNeighbor(int i, int j, bool isAdd) {
	if (!isAdd) {
		history.pop_back();
		return;
	}
	points ps;
	pointHash hash = pointHash();
	if (history.size() > 0) {
		for (int k = 0; k < history.back().count; k++) {
			if (history.back().list[k].x == i && history.back().list[k].y == j)
				continue;
			ps.add(history.back().list[k]);
			hash.add(history.back().list[k]);
		}
	}
	for (int k = 0; k < 8; k++) {
		int x = i + directX[k];
		int y = j + directY[k];
		if (reachable(x, y)) {
			Color color = map[x][y];
			if (color == NULL) {
				if (!hash.contains(x, y)) {
					hash.add(x, y);
					ps.add(point(x, y));
				}
			}
			else {
				if (color == map[i][j]) {
					int x1 = i + directX[k] * 3;
					int y1 = j + directY[k] * 3;
					int x2 = i - directX[k] * 2;
					int y2 = j - directY[k] * 2;
					if (reachable(x1, y1)) {
						if (map[x1][y1] == NULL) {
							if (!hash.contains(x1, y1)) {
								hash.add(x1, y1);
								ps.add(point(x1, y1));
							}
						}
					}
					if (reachable(x2, y2)) {
						if (map[x2][y2] == NULL) {
							if (!hash.contains(x2, y2)) {
								hash.add(x2, y2);
								ps.add(point(x2, y2));
							}
						}
					}
				}
			}
		}
	}
	history.push_back(ps);
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

	history.clear();

	//初始化地图和哈希码
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			Color color = map[i][j];
			if (color != NULL) {
				updateHashCode(i, j, color);
				bool isAdd = color == NULL ? false : true;
				updateNeighbor(i, j, isAdd);
			}
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			for (int k = 0; k < 4; k++) {
				setLineKey(point(i, j), k, BLACK);
				setLineKey(point(i, j), k, WHITE);
			}
		}
}

void setColor(int x, int y, Color color)
{
	if (color != NULL)
		updateHashCode(x, y, color);
	else
		updateHashCode(x, y, map[x][y]);

	map[x][y] = color;
	bool isAdd = color == NULL ? false : true;
	updateNeighbor(x, y, isAdd);
	updatePointKey(point(x, y));
}

Color getColor(point p)
{
	return map[p.x][p.y];
}

points getPointLinesNeighbor(point p) {
	points neighbor = getNeighbor();
	pointHash hash = pointHash();
	for (int i = 0; i < neighbor.count; i++) {
		point p = neighbor.list[i];
		hash.add(p);
	}
	points result;
	for (int i = 0; i < 8; i++) {
		int x = p.x;
		int y = p.y;
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
	if (history.size() == 0)
		return points();
	return history.back();
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