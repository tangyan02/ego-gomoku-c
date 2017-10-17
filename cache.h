#pragma once

#define COMBO_TRUE 1
#define COMBO_FALSE 0
#define COMBO_EMPTY -1

void addComboEntry(long long key, bool value);

int getComboValue(long long key);

void cacheReset();