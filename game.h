#pragma once
#include"config.h"
#include"point.h"

point search(Color aiColor, Color** map);

void setPoint(point p, Color color, Color forwardColor, Color aiColor);

int dfs(int level, Color color, int parentMax, int parentMin, Color aiColor);