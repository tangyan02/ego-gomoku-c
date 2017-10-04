#include "stdafx.h"
#include "io.h"
#include <iostream>  
#include <fstream>  
#include <string>  

using namespace std;

extern int boardSize;

int ** readMap()
{
	int ** map = new int*[boardSize];
	for (int i = 0; i < boardSize; i++)
	{
		map[i] = new int[boardSize];
	}

	char *filePath = "input.txt";
	ifstream file;
	file.open(filePath, ios::in);

	if (!file.is_open())
		return 0;

	std::string strLine;
	int i = 0;
	while (getline(file, strLine))
	{
		if (strLine.empty())
			continue;
		for (int j = 0; j < boardSize; j++)
		{
			char c = strLine[j];
			map[i][j] = NULL;
			if (c == 'X')
				map[i][j] = BLACK;
			if (c == 'O')
				map[i][j] = WHITE;
		}
		i++;
	}
	return map;
}


