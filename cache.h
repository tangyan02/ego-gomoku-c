#pragma once

#define ComboType int
#define THREE_COMBO 0
#define FOUR_COMBO 1

#define COMBO_TRUE 1
#define COMBO_FALSE 0
#define COMBO_EMPTY -1

void addComboEntry(long long key, bool value, int level, ComboType type);

int getComboValue(long long key, int level, ComboType type);

void cacheReset();