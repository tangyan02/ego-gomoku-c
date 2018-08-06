#pragma once
#include "config.h"

bool canWinCheck(Color color);

bool checkAndSetInFiveAttack(Color color, points *neighbors);

bool checkAndSetInFourDefence(Color color, points *neighbors);

bool checkAndSetInThreeActive(Color color, points *neighbors);