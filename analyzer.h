#pragma once
#include "stdafx.h"
#include "analyzeData.h"
#include "gameMap.h"

#define MAX_TABLE_SIZE 1<<16

void initAnalyze();

analyzeData getAnalyzeData(Color color, points *ps);

bool getThreeDefenseTable(int key, int x, int y, int direct, Color color);