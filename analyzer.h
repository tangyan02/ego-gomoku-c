#pragma once
#include "config.h"
#include "points.h"

bool canWinCheck(Color color);

bool tryFiveAttack(Color color, points *neighbors);

bool tryFourAttack(Color color, points *neighbors);

bool tryThreeOrFourAttack(Color color, points *neighbors);

bool tryFourDefence(Color color, points *neighbors);

bool tryThreeDefence(Color color, points *neighbors);

point getFiveAttack(points *neighbors, Color color);

bool hasComboAttack(Color color, int comboType);

bool isGreater(Color aiColor);