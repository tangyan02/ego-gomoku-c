#include "stdafx.h"
#include "gameMap.h"

int **map;

extern int boardSize;

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

points getNeighbor()
{
	points result;
	bool signal[20][20];

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			signal[i][j] = false;

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			if (map[i][j] != NULL) {
				for (int x = i - 2; x <= i + 2; x++)
					for (int y = j - 2; y <= j + 2; y++)
						if (reach(x, y))
							if (map[x][y] == NULL) {
								signal[x][y] = true;
							}
			}
		}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			if (signal[i][j]) {
				result.add(point(i, j));
			}

	return result;
}

bool reach(int x, int y)
{
	if (x < 0 || y < 0 || x >= boardSize || y >= boardSize)
		return false;
	return true;
}
