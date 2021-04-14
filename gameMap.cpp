#include "stdafx.h"
#include "gameMap.h"
#include "stdlib.h"
#include "vector"
#include <time.h>  
#include "patternRecorder.h"
#include "PointsFactory.h"

int **map;

points moveHistory;

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
	
	//for (int x = i - 2; x <= i + 2; x++)
	//	for (int y = j - 2; y <= j + 2; y++)
	//		if (reachable(x, y)) {
	//			if (i == x && j == y)
	//				continue;
	//			if (map[x][y] == NULL)
	//				addNeighborCount(x, y, isAdd);
	//		}
	// 
	// 
	//for (int k = 0; k < 8; k++) {
	//	int x = i + directX[k];
	//	int y = j + directY[k];
	//	if (reachable(x, y)) {
	//		Color color = map[x][y];
	//		if (color == NULL) {
	//			addNeighborCount(x, y, isAdd);
	//		}
	//	}
	//	int x2 = x + directX[k];
	//	int y2 = y + directY[k];
	//	if (reachable(x2, y2)) {
	//		Color color = map[x2][y2];
	//		if (color == NULL) {
	//			addNeighborCount(x2, y2, isAdd);
	//		}
	//	}
	//}
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
	right = 0;
	bottom = 0;
	left = boardSize - 1;
	top = boardSize - 1;

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

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			Color color = map[i][j];
			if (color != NULL) {
				updateHashCode(i, j, color);
				bool isAdd = color == NULL ? false : true;
				updateNeighbor(i, j, true, color);
			}
		}

	initPatternRecord();	
}

void move(int x, int y, Color color) {
	removeLinePatternCount(x, y);
	updateHashCode(x, y, color);
	map[x][y] = color;
	updateNeighbor(x, y, true, color);
	updateLineKey(x, y);
	updatePointPattern(x, y);
	updateLinePatternCount(x, y);
	moveHistory.add(point(x, y));
}

void undoMove(int x, int y, Color color) {
	removeLinePatternCount(x, y);
	updateHashCode(x, y, color);
	map[x][y] = NULL_COLOR;
	updateNeighbor(x, y, false, color);
	updateLineKey(x, y);
	updatePointPattern(x, y);
	updateLinePatternCount(x, y);
	moveHistory.pop();
}

Color getColor(int x, int y)
{
	return map[x][y];
}

void fillPointLinesNeighbor(int px, int py, points* ps) {
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
				if (!ps->contains(x, y)) {
					ps->add(point(x, y));
				}
			}
		}
	}
}

void fillNeighbor(points* ps) {
	for (int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++)
			if (neighborCount[i][j] > 0 && map[i][j] == NULL)
				ps->add(point(i, j));
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

bool inNeighbor(int x, int y)
{
	return neighborCount[x][y] > 0;
}
