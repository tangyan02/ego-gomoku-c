#pragma once
#include "stdafx.h"
#include "analyzeData.h"
#include "gameMap.h"

void initAnalyze();

analyzeData getAnalyzeData(Color color, points ps);

analyzeData getAnalyzeData(Color color, points ps, bool moreAnalyze);
