#pragma once
#include"comboResult.h"

#define ComboType int
#define THREE_COMBO 0
#define FOUR_COMBO 1

comboResult canKill(Color targetColor, int level, long long startTime, long long limitTime);

comboResult canKillThree(Color targetColor, int level);

comboResult canKillFour(Color targetColor, int level);