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

static comboNode comboCacheThree[3][hashSize];
static comboNode comboCacheFour[3][hashSize];

int getKey(long long key) {
	if (key < 0)
		key = -key;
	return key%hashSize;
}

void addComboEntry(long long key, bool value, int level, ComboType type, Color color)
{
	comboNode* node;
	if (type == THREE_COMBO)
		node = &comboCacheThree[color][getKey(key)];
	else
		node = &comboCacheFour[color][getKey(key)];
	if (level < node->level && node->key == key)
		return;
	if (value)
		node->flag = COMBO_TRUE;
	else
		node->flag = COMBO_FALSE;
	node->key = key;
	node->level = level;
	comboCacheTotal++;
}

int getComboValue(long long key, int level, ComboType type, Color color)
{
	comboNode* node;
	if (type == THREE_COMBO)
		node = &comboCacheThree[color][getKey(key)];
	else
		node = &comboCacheFour[color][getKey(key)];
	if (node->key == key) {
		if (level > node->level) {
			return COMBO_EMPTY;
		}
		comboCacheHit++;
		return node->flag;
	}
	return COMBO_EMPTY;
}

void cacheReset()
{
	
}
