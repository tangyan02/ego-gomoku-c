#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"

int main()
{
	Color ** map = readMap();
	initGameMap(map);
	
	points ps = getNeighbor();
	for (int i = 0; i < ps.count; i++)
	{
		point p = ps.list[i];
		map[p.x][p.y] = STAR;
	}
	printMap(map);
	getchar();
	return 0;
}

