#pragma once
#include "stdafx.h"
#include "analyzeData.h"
#include "gameMap.h"

#define MAX_TABLE_SIZE 1<<16

void initAnalyze();

int getMapLineKey(int x, int y, int direct, Color targetColor);

int getPointTableColor(int x, int y, Color targetColor);

void updatePointKey(point p);

void setLineKey(point p, int direct, Color targetColor);

analyzeData getAnalyzeData(Color color, points ps);
