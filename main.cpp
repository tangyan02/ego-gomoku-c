#include "stdafx.h"
#include "console.h"
#include "io.h"

int main()
{
	printf("hello");
	int ** map = readMap();
	printMap(map);
	getchar();
	return 0;
}

