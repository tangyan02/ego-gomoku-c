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


void printMapInMessage(Color ** map)
{
	for (int j = 0; j < boardSize; j++)
	{
		cout << "MESSAGE ";
		for (int i = 0; i < boardSize; i++)
		{
			if (map[i][j] == BLACK)
				cout << "x";
			if (map[i][j] == WHITE)
				cout << "o";
			if (map[i][j] == STAR)
				cout << "*";
			if (map[i][j] == NULL)
				cout << ".";
			if (map[i][j] == FLAG)
				cout << 'F';
		}
		cout << endl;
	}
}

void printMap(Color ** map)
{
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] == BLACK)
				cout << "x ";
			if (map[i][j] == WHITE)
				cout << "o ";
			if (map[i][j] == STAR)
				cout << "* ";
			if (map[i][j] == NULL)
				cout << ". ";
			if (map[i][j] == FLAG)
				cout << "F ";
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

void printPlayers(vector<player>& players)
{
	printf("players:\n");
	for (int i = 0; i < players.size(); i++) {
		printf("version:%d sigma:%d score:", players[i].version, players[i].sigma);
		for (int j = 0; j < 10; j++) {
			printf("%d ", players[i].score[j]);
		}
		printf("\n");
	}
}

void printGroupResult(int winCount[], vector<player>& players)
{
	printf("===========\n");
	printf("group result:\n");
	for (int i = 0; i < players.size(); i++) {
		printf("%d: %d", players[i].version, winCount[i]);
		printf(" sigma:%d",players[i].sigma);
		printf(" scoore:");
		for (int j = 0; j < 10; j++) {
			printf("%d ", players[i].score[j]);
		}
		printf("\n");
	}
}

char getCharOfColor(Color color)
{
	if (color == BLACK) {
		return 'x';
	}
	if (color == WHITE) {
		return 'o';
	}
	return '?';
}
