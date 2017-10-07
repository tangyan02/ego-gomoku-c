#include "stdafx.h"
#include "cache.h"
#include "unordered_map"

unordered_map<long long, int> cache;
int cacheSize = 0;
int cacheCount = 0;

void initCache(int size)
{
	cacheSize = size;
}

bool containsSearchKey(long long key)
{
	if (cache.find(key) != cache.end())
		return true;
	return false;
}

int getSearchValue(long long key)
{
	return cache[key];
}

void addSearchEntry(long long key, int value)
{
	if (cacheCount > cacheSize)
		return;
	cache[key] = value;
	cacheCount++;
}

void cacheReset()
{
	cache.clear();
	cacheCount = 0;
}
