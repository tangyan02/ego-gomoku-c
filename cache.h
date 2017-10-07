#pragma once

void initCache(int size);

bool containsSearchKey(long long key);

int getSearchValue(long long key);

void addSearchEntry(long long key, int value);

void cacheReset();