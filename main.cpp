#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"

int main()
{
	Color ** map = readMap();
	point p = search(BLACK, map);
	map[p.x][p.y] = STAR;
	printMap(map);
	getchar();
	return 0;
}

