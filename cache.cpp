#include "stdafx.h"
#include "cache.h"
#include "unordered_map"

const int hashSize = 1 << 22;

int comboCacheHit = 0;

int comboCacheTotal = 0;

struct comboNode {
	long long key;
	int flag;
	int level;
};

static comboNode comboCacheThree[hashSize];
static comboNode comboCacheFour[hashSize];

int getKey(long long key) {
	if (key < 0)
		key = -key;
	return key%hashSize;
}

void addComboEntry(long long key, bool value, int level, ComboType type)
{
	comboNode* node;
	if (type == THREE_COMBO)
		node = &comboCacheThree[getKey(key)];
	else
		node = &comboCacheFour[getKey(key)];
	if (level < node->level && node->key == key && value == false)
		return;
	if (value)
		node->flag = COMBO_TRUE;
	else
		node->flag = COMBO_FALSE;
	node->key = key;
	node->level = level;
	comboCacheTotal++;
}

int getComboValue(long long key, int level, ComboType type)
{
	comboNode* node;
	if (type == THREE_COMBO)
		node = &comboCacheThree[getKey(key)];
	else
		node = &comboCacheFour[getKey(key)];
	if (node->key == key) {
		if (level > node->level && node->flag == COMBO_FALSE)
			return COMBO_EMPTY;
		comboCacheHit++;
		return node->flag;
	}
	return COMBO_EMPTY;
}

void cacheReset()
{
	for (int i = 0; i < hashSize; i++) {
		comboCacheThree[i].flag = COMBO_EMPTY;
		comboCacheThree[i].level = 0;
		comboCacheFour[i].flag = COMBO_EMPTY;
		comboCacheFour[i].level = 0;
	}
}
