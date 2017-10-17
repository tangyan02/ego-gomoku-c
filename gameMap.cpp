#include "stdafx.h"
#include "gameMap.h"
#include "stdlib.h"
#include <time.h>  

int **map;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

static long long weightBlack[20][20];
static long long weightWhite[20][20];
static int blackLineKey[20][20][4];
static int whiteLineKey[20][20][4];

long long hashCode = 0;

Color ** getMap() {
	return map;
}

int getMapLineKey(int x, int y, int direct, Color targetColor) {
	if (targetColor == BLACK)
		return blackLineKey[x][y][direct];
	else
		return whiteLineKey[x][y][direct];
}

int getPointTableColor(int x, int y, Color targetColor) {
	if (!reachable(x, y))
		return INVALID_COLOR;
	if (map[x][y] == NULL)
		return NULL_COLOR;
	if (map[x][y] == targetColor)
		return CURRENT_COLOR;
	if (map[x][y] == getOtherColor(targetColor))
		return OTHER_COLOR;
	return 0;
}

static int getLineKey(int line[]) {
	int key = 0;
	for (int i = 0; i < 8; i++) {
		key = key << 2;
		key |= line[i];
	}
	return key;
}

static void printKey(int key) {
	for (int i = 0; i < 8; i++) {
		printf("%d", key % 4);
		key = key >> 2;
	}
	printf("\n");
}

int updateLineKey(int x, int y, int direct, int n, Color targetColor) {
	int px = x + (n - 4)*directX[direct];
	int py = y + (n - 4)*directY[direct];
	int tableColor = getPointTableColor(x, y, targetColor);

	if (n >= 4)
		n--;
	int t = 3 << (n * 2);
	t = t ^ ((1 << 16) - 1);
	int key;
	if (targetColor == WHITE)
		key = whiteLineKey[px][py][direct];
	else
		key = blackLineKey[px][py][direct];
	key &= t;
	tableColor = tableColor << (n * 2);
	key |= tableColor;
	if (targetColor == WHITE)
		whiteLineKey[px][py][direct] = key;
	else
		blackLineKey[px][py][direct] = key;
	return key;
}

void setLineKey(point p, int direct, Color targetColor) {
	int line[8];
	int x = p.x - 4 * directX[direct];
	int y = p.y - 4 * directY[direct];
	for (int i = 0; i < 4; i++) {
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
		x += directX[direct];
		y += directY[direct];
	}

	for (int i = 4; i < 8; i++) {
		x += directX[direct];
		y += directY[direct];
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
	}
	if (targetColor == BLACK)
		blackLineKey[p.x][p.y][direct] = getLineKey(line);
	if (targetColor == WHITE)
		whiteLineKey[p.x][p.y][direct] = getLineKey(line);
}

void updatePointKey(point p) {
	for (int i = 0; i < 4; i++) {
		int x = p.x - 5 * directX[i];
		int y = p.y - 5 * directY[i];
		for (int k = 0; k <= 8; k++) {
			x += directX[i];
			y += directY[i];
			if (!reachable(x, y)) {
				continue;
			}
			if (k == 4)
				continue;
			updateLineKey(p.x, p.y, i, k, BLACK);
			updateLineKey(p.x, p.y, i, k, WHITE);
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

points getNeighbor()
{
	points result;
	pointHash hash = pointHash();

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			if (map[i][j] != NULL) {
				for (int k = 0; k < 8; k++) {
					int x = i + directX[k];
					int y = j + directY[k];
					if (reachable(x, y)) {
						Color color = map[x][y];
						if (color == NULL) {
							hash.add(x, y);
						}
						else {
							if (color == map[i][j]) {
								int x1 = i + directX[k] * 3;
								int y1 = j + directY[k] * 3;
								int x2 = i - directX[k] * 2;
								int y2 = j - directY[k] * 2;
								if (reachable(x1, y1)) {
									if (map[x1][y1] == NULL) {
										hash.add(x1, y1);
									}
								}
								if (reachable(x2, y2)) {
									if (map[x2][y2] == NULL) {
										hash.add(x2, y2);
									}
								}
							}
						}
					}
				}
			}
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			if (hash.contains(i, j)) {
				result.add(point(i, j));
			}

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