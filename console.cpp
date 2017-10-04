#include "stdafx.h"
#include "console.h"
#include "iostream"

using namespace std;

extern int boardSize;

void printMap(int ** map)
{
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] == BLACK)
				cout << "X";
			if (map[i][j] == WHITE)
				cout << "O";
			if (map[i][j] == NULL)
				cout << ".";
		}
		cout << endl;
	}
}
