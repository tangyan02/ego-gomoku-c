#include "stdafx.h"
#include "gameMap.h"

int **map;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

Color ** getMap() {
	return map;
}

void initGameMap(Color** value) {
	map = value;
}

void setColor(int x, int y, Color color)
{
	map[x][y] = color;
}

Color getColor(point p)
{
	return map[p.x][p.y];
}

points getNeighbor()
{
	points result;
	pointHash hash = pointHash(boardSize);

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
