#pragma once
#include"config.h"
#include"point.h"
#include"gameResult.h"

long long getSystemTime();

gameResult search(Color aiColor, Color** map);
