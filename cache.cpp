#include "stdafx.h"
#include "cache.h"
#include "unordered_map"

unordered_map<long long, int> cache;
extern int cacheSize;
int cacheCount = 0;

unordered_map<long long, int> comboCache;
int comboCacheCount = 0;


long long getFinalKey(long long key, int alpha, int beta) {
	key ^= alpha;
	key ^= beta;
	return key;
}

bool containsSearchKey(long long key, int alpha, int beta)
{
	key = getFinalKey(key, alpha, beta);
	if (cache.find(key) != cache.end())
		return true;
	return false;
}

int getSearchValue(long long key, int alpha, int beta)
{
	key = getFinalKey(key, alpha, beta);
	return cache[key];
}

void addSearchEntry(long long key, int value, int alpha, int beta)
{
	key = getFinalKey(key, alpha, beta);
	if (cacheCount > cacheSize)
		return;
	cache[key] = value;
	cacheCount++;
}

bool containsCombo(long long key)
{
	if (comboCache.find(key) != comboCache.end())
		return true;
	return false;
}


void addComboEntry(long long key, bool value)
{
	if (comboCacheCount > cacheSize)
		return;
	comboCache[key] = value;
	comboCacheCount++;
}

bool getComboValue(long long key)
{
	return comboCache[key];
}

void cacheReset()
{
	cache.clear();
	comboCache.clear();
	cacheCount = 0;
	comboCacheCount = 0;
}
