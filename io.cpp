#include "stdafx.h"
#include "io.h"
#include <iostream>  
#include <fstream>  
#include <string>  

using namespace std;

extern int boardSize;

Color ** getEmptyMap() {
	Color ** map = new Color*[boardSize];
	for (int i = 0; i < boardSize; i++)
		map[i] = new Color[boardSize];

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			map[i][j] = NULL;

	return map;
}

Color ** readMap()
{
	return readMap("input.txt");
}

Color ** readMap(char * path)
{
	Color ** map = new Color*[boardSize];
	for (int i = 0; i < boardSize; i++)
	{
		map[i] = new Color[boardSize];
	}

	char *filePath = path;
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


