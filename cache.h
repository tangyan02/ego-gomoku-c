#pragma once

long long getFinalKey(long long key, int alpha, int beta);

bool containsSearchKey(long long key, int alpha, int beta);

int getSearchValue(long long key, int alpha, int beta);

void addSearchEntry(long long key, int value, int alpha, int beta);

bool containsCombo(long long key);

void addComboEntry(long long key, bool value);

bool getComboValue(long long key);

void cacheReset();