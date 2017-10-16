#include "stdafx.h"
#include "cache.h"
#include "unordered_map"

extern int cacheSize;
unordered_map<long long, int> comboCache;
int comboCacheCount = 0;

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
	comboCache.clear();
	comboCacheCount = 0;
}
