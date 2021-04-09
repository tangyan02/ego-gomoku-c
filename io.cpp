#include "stdafx.h"
#include "io.h"
#include <iostream>  
#include <fstream>  
#include <cstring>  

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

Color nextColorForOpennings(Color** map) {
	int whiteCount = 0;
	int blackCount = 0;
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] == BLACK)
				blackCount++;
			if (map[i][j] == WHITE)
				whiteCount++;
		}
	// white always first for openings
	if (whiteCount > blackCount)
		return BLACK;
	return WHITE;
}

Color** readMapFromOpennings(int lineNum, Color** map) {
	FILE* fp;
	fp = fopen("openings.txt", "r");
	char line[2048] = "";
	char* p=line;
	for (int i = 0; i < lineNum; i++)
	{
		for (int j = 0; j < 2048; j++){
			line[j] = '\0';
		}
		fgets(line, 2048, fp);
	}
	fclose(fp);

	//�ֶ��Ӹ�,ʹ��ʽһ��
	for (int i = 0; i < 2048;i++){
		if(line[i] == '\0') {
			line[i] = ',';
			break;
		}
	}

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			map[i][j] = NULL;

	Color color = WHITE;
	while (true)
	{
		if (p[0] == '\0') {
			break;
		}
		int x, y;
		int result = sscanf(p, "%d, %d,", &x, &y);
		x += 10;
		y += 10;
		map[x][y] = color;
		color = getOtherColor(color);

		//����ƫ����
		bool isBreak = false;
		while (*p != ',') {
			p++;
		}
		p++;
		while (*p != ',') {
			p++;
		}
		p++;
	}
	return map;
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
			if (c == 'x')
				map[i][j] = BLACK;
			if (c == 'o')
				map[i][j] = WHITE;
		}
		i++;
	}
	return map;
}


void copyMap(Color** map, Color** toMap){
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			toMap[i][j] = map[i][j];
}