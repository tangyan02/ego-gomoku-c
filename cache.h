#pragma once

void initCache(int size);

bool containsSearchKey(long key);

int getSearchValue(long key);

void addSearchEntry(long key, int value);

void cacheReset();