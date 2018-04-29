#include "stdafx.h"
#include "console.h"
#include "iostream"

using namespace std;

extern int boardSize;

void printPoints(points ps) {
	for (int i = 0; i < ps.count; i++)
	{
		point p = ps.list[i];
		printf("%d,%d ", p.x, p.y);
	}
	printf("\n");
}

void printHash(pointHash hash) {
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++) {
			if (hash.contains(point(i, j))) {
				printf("%d,%d ", i, j);
			}
		}
	}
	printf("\n");
}

void printMap(Color ** map)
{
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] == BLACK)
				cout << "¡ð";
			if (map[i][j] == WHITE)
				cout << "¡ñ";
			if (map[i][j] == STAR)
				cout << "¡ï";
			if (map[i][j] == NULL)
				cout << "¡¤";
		}
		cout << endl;
	}
}

void printMapWithStars(Color ** map, points ps)
{
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		map[p.x][p.y] = STAR;
	}
	printMap(map);
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		map[p.x][p.y] = NULL;
	}
}

void printMapWithStars(Color ** map, pointHash hash)
{
	points ps;
	for(int i = 0;i<boardSize;i++)
		for (int j = 0; j < boardSize; j++) {
			if (hash.contains(i, j))
				ps.add(point(i, j));
		}
	printMapWithStars(map, ps);
}

void printMapWithStar(Color ** map, point p)
{
	map[p.x][p.y] = STAR;
	printMap(map);
	map[p.x][p.y] = NULL;
}
