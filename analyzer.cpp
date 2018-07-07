#include "stdafx.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include "unordered_map"
#include "patternRecorder.h"

#define ERROR -1

#define NULL_COLOR 0
#define CURRENT_COLOR 1
#define OTHER_COLOR 2
#define INVALID_COLOR 3

#define NO_TYPE 0
#define FIVA_ATTACK 1
#define FOUR_ATTACK 2
#define FOUR_DEFENCE 3
#define THREE_ATTACK 4
#define THREE_DEFENCE 5
#define TWO_ATTACK 6

extern Color** map;

bool fiveAttackTable[MAX_TABLE_SIZE];
bool fourAttackTable[MAX_TABLE_SIZE];
bool fourDefenceTable[MAX_TABLE_SIZE];
bool threeAttackTable[MAX_TABLE_SIZE];
bool twoAttackTable[MAX_TABLE_SIZE];
bool threeDefenceTable[MAX_TABLE_SIZE][4][4][4][4];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static bool inited = false;

int getPointTableColor(Color** map, int x, int y, Color targetColor) {
	if (!reachable(x, y))
		return INVALID_COLOR;
	if (map[x][y] == NULL)
		return NULL_COLOR;
	if (map[x][y] == targetColor)
		return CURRENT_COLOR;
	if (map[x][y] == getOtherColor(targetColor))
		return OTHER_COLOR;
	return ERROR;
}

void printKey(int key) {
	for (int i = 0; i < 8; i++) {
		printf("%d", key % 4);
		key = key >> 2;
	}
	printf("\n");
}

void printLine(int line[]) {
	for (int i = 0; i < 8; i++)
		if (line[i] == 3)
			return;
	for (int i = 0; i < 8; i++)
		printf("%d", line[i]);
	printf("\n");
}

static int getLineKey(int line[]) {
	int key = 0;
	for (int i = 0; i < 8; i++) {
		key = key << 2;
		key |= line[i];
	}
	return key;
}

void caculate(int line[]) {
	int currentCountList[8] = { 0 };
	int otherCountList[8] = { 0 };
	for (int i = 3; i < 8; i++) {
		int head = i - 3;
		int current = 0;
		int other = 0;
		if (line[head] == INVALID_COLOR || line[i] == INVALID_COLOR) {
			continue;
		}
		for (int j = head; j <= i; j++) {
			if (line[j] == CURRENT_COLOR)
				current++;
			if (line[j] == OTHER_COLOR)
				other++;
		}
		currentCountList[i] = current;
		otherCountList[i] = other;
	}

	//表格计算
	int key = getLineKey(line);
	for (int i = 3; i < 8; i++) {
		int head = i - 3;
		if (line[head] == INVALID_COLOR || line[i] == INVALID_COLOR) {
			continue;
		}
		int current = currentCountList[i];
		int other = otherCountList[i];
		if (current == 4 && other == 0) {
			fiveAttackTable[key] = true;
		}
		if (current == 3 && other == 0) {
			fourAttackTable[key] = true;
		}
		if (current == 0 && other == 4) {
			fourDefenceTable[key] = true;
		}
		if (current == 1 && other == 0) {
			twoAttackTable[key] = true;
		}
	}

	for (int i = 3; i < 8; i++) {
		int current = currentCountList[i];
		int other = otherCountList[i];
		int headColor = line[i - 3];
		int tailColor = line[i];
		//中间情形
		if (i >= 4 && i < 7) {
			int headSideColor = line[i - 4];
			int tailSideColor = line[i + 1];
			if (headColor == INVALID_COLOR || tailColor == INVALID_COLOR) {
				continue;
			}
			if (current == 2 && other == 0) {
				if (headColor == NULL_COLOR && tailColor == NULL_COLOR) {
					if (headSideColor == NULL_COLOR || tailSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
				if (headColor == NULL_COLOR && tailColor != NULL_COLOR) {
					if (tailSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
				if (headColor != NULL_COLOR && tailColor == NULL_COLOR) {
					if (headSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
			}
			for (int left = 0; left < 4; left++)
				for (int right = 0; right < 4; right++)
					for (int leftLeft = 0; leftLeft < 4; leftLeft++)
						for (int rightRight = 0; rightRight < 4; rightRight++) {
							if (current == 0 && other == 3) {
								if (headColor == NULL_COLOR && tailColor != NULL_COLOR) {
									if (tailSideColor == NULL_COLOR) {
										threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
									}
								}
								if (headColor != NULL_COLOR && tailColor == NULL_COLOR) {
									if (headSideColor == NULL_COLOR) {
										threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
									}
								}
							}
						}
		}
		else {
			//两边的防3有特殊情况
			for (int left = 0; left < 4; left++)
				for (int right = 0; right < 4; right++)
					for (int leftLeft = 0; leftLeft < 4; leftLeft++)
						for (int rightRight = 0; rightRight < 4; rightRight++) {
							if (current == 0 && other == 3) {
								if (i == 3) {
									//OXXX-0000 
									if (headColor == NULL_COLOR) {
										if (line[4] == NULL_COLOR || left == NULL_COLOR) {
											threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
										}
									}
									else
										if (left == NULL_COLOR) {
											//XOXX-0000
											if (line[3] != NULL_COLOR) {
												threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
											}
											//XXXO-0000
											if (line[3] == NULL_COLOR && leftLeft != NULL_COLOR) {
												threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
											}
										}
								}
								if (i == 7) {
									//OOOO-XXXO
									if (tailColor == NULL_COLOR) {
										if (line[3] == NULL_COLOR || right == NULL_COLOR)
											threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
									}
									else
										if (right == NULL_COLOR) {
											//OOOO-XOXX
											if (line[4] != NULL_COLOR) {
												threeDefenceTable[key][left][right][leftLeft][rightRight] = true;												
											}
											//OOOO=OXXX
											if (line[4] == NULL_COLOR && rightRight != NULL_COLOR) {
												threeDefenceTable[key][left][right][leftLeft][rightRight] = true;
											}
										}
								}
							}
						}
		}
	}
}

void buildLine(int i, int line[]) {
	if (i == 0) {
		caculate(line);
		return;
	}
	for (int type = 0; type < 4; type++) {
		line[i - 1] = type;
		buildLine(i - 1, line);
	}
}

void initAnalyze() {
	if (inited)
		return;
	int line[8] = { 0 };
	buildLine(8, line);
	inited = true;
}


int getLineLeftColor(int x, int y, Color targetColor, int direct) {
	return getPointTableColor(x - 5 * directX[direct], y - 5 * directY[direct], targetColor);
}

int getLineLeftLeftColor(int x, int y, Color targetColor, int direct) {
	return getPointTableColor(x - 6 * directX[direct], y - 6 * directY[direct], targetColor);
}

int getLineRightColor(int x, int y, Color targetColor, int direct) {
	return getPointTableColor(x + 5 * directX[direct], y + 5 * directY[direct], targetColor);
}

int getLineRightRightColor(int x, int y, Color targetColor, int direct) {
	return getPointTableColor(x + 6 * directX[direct], y + 6 * directY[direct], targetColor);
}

bool getThreeDefenseTable(int key, int x, int y, int direct, Color color) {
	int left = getLineLeftColor(x, y, color, direct);
	int right = getLineRightColor(x, y, color, direct);
	int leftLeft = getLineLeftLeftColor(x, y, color, direct);
	int rightRight = getLineRightRightColor(x, y, color, direct);
	return threeDefenceTable[key][left][right][leftLeft][rightRight];
}

analyzeData getAnalyzeData(Color color, points *ps) {
	Color** map = getMap();
	analyzeData result;

	for (int i = 0; i < ps->count; i++) {
		bool ok = false;
		point p = ps->list[i];
		for (int k = 0; k < 4; k++) {
			int key = getMapLineKey(p.x, p.y, k, color);

			if (fiveAttackTable[key]) {
				if (!result.fiveAttack.contains(p.x, p.y)) {
					result.fiveAttack.add(p);
				}
			}
			if (fourAttackTable[key]) {
				if (!result.fourAttack.contains(p.x, p.y)) {
					result.fourAttack.add(p);
				}
			}
			if (fourDefenceTable[key]) {
				if (!result.fourDefence.contains(p.x, p.y)) {
					result.fourDefence.add(p);
				}
			}
			if (threeAttackTable[key]) {
				if (!result.threeAttack.contains(p.x, p.y)) {
					result.threeAttack.add(p);
				}
			}
			if (getThreeDefenseTable(key, p.x, p.y, k, color))
				if (!result.threeDefence.contains(p.x, p.y)) {
					result.threeDefence.add(p);
				}
		}
	}

	return result;
}