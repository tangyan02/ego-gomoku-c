#pragma once

#include "config.h"
#include "point.h"
#include "comboResult.h"

comboResult kill(Color color, int level, long long targetTime);

comboResult killVCF(Color color, long long targetTime);

void testCombo();

void testAllCombo();