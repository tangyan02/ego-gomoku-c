#include "stdafx.h"
#include "cache.h"
#include "unordered_map"

const int hashSize = 1 << 18;

struct comboNode {
	long long key;
	int flag;
};

extern int cacheSize;
static comboNode comboCache[hashSize];
static int comboCacheCount = 0;


int getKey(long long key) {
	return key%hashSize;
}

void addComboEntry(long long key, bool value)
{
	comboNode* node = &comboCache[getKey(key)];
	node->key = key;
	if (value)
		node->flag = COMBO_TRUE;
	else
		node->flag = COMBO_FALSE;
}

int getComboValue(long long key)
{
	if (comboCache[getKey(key)].key == key) {
		return comboCache[getKey(key)].flag;
	}
	return COMBO_EMPTY;
}

void cacheReset()
{
	for (int i = 0; i < hashSize; i++)
		comboCache[i].flag = COMBO_EMPTY;
	comboCacheCount = 0;
}
