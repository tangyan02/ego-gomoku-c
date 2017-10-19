#pragma once
#include"config.h"
#include"point.h"
#include"gameResult.h"

long long getSystemTime();

void setPoint(point p, Color color, Color forwardColor, Color aiColor);

int dfs(int level, Color color, Color aiColor, int parentMax, int parentMin, int extend);

gameResult search(Color aiColor, Color** map);
