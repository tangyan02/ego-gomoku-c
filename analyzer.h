#pragma once
#include "stdafx.h"
#include "analyzeData.h"
#include "gameMap.h"

extern int boardSize;

analyzeData getAnalyzeData(Color color, points ps);

analyzeData getAnalyzeData(Color color, points ps, bool moreAnalyze);
