#pragma once
#include "config.h"

bool canWinCheck(Color color);

bool TryFiveAttack(Color color, points *neighbors);

bool tryFourDefence(Color color, points *neighbors);

bool tryThreeDefence(Color color, points *neighbors);