#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "analyzeData.h"
#include "analyzer.h"

void testPlay() {
	Color ** map = readMap();
	point p = search(BLACK, map);
	map[p.x][p.y] = STAR;
	printMap(map);
}

void testAnalyze() {
	Color ** map = readMap("analyze.txt");
	initGameMap(map);
	initScore(WHITE);
	analyzeData data = getAnalyzeData(WHITE, getNeighbor());

	printf("%d\n", data.threeAttack.count);
	for (int i = 0; i < data.threeAttack.count; i++) {
		point p = data.threeAttack.list[i];
		map[p.x][p.y] = STAR;
	}
	printMap(map);
}

void testNeighbor() {
	Color ** map = readMap("analyze.txt");
	initGameMap(map);
	points ps = getNeighbor();
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		map[p.x][p.y] = STAR;
	}
	printMap(map);
}

int main()
{
	testPlay();
	getchar();
	return 0;
}
