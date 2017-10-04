#include "stdafx.h"
#include "console.h"
#include "iostream"

using namespace std;

extern int boardSize;

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
