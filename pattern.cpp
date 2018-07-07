#include "stdafx.h"
#include "pattern.h"
#include "functional"

#define STATE_BLANK 0
#define STATE_SELF 1
#define STATE_OTHER 2

#define PATTERN_NULL 0
#define PATTERN_LINE_FIVE 1
#define PATTERN_ACTIVE_FOUR 2
#define PATTERN_SLEEPY_FOUR 3
#define PATTERN_ACTIVE_THREE 4
#define PATTERN_SLEEPY_THREE 5
#define PATTERN_ACTIVE_TWO 6
#define PATTERN_SLEEPY_TWO 7

static int directX[] = { 0, 1, 1, 1 };
static int directY[] = { 1, 1, 0, -1 };

int patternLib[PATTERN_SIZE] = { PATTERN_NULL };

int getLineKey(int line[]) {
	int key = 0;
	for (int i = 0; i < 8; i++) {
		key = key << 2;
		key |= line[i];
	}
	return key;
}

static void caculateLine(int i, int line[], std::function<void(int*)> caculate) {
	if (i == 0) {
		caculate(line);
		return;
	}
	for (int type = 0; type < 3; type++) {
		line[i - 1] = type;
		caculateLine(i - 1, line, caculate);
	}
}

static void abstructCalculate(int line[], int targetPattern, int comparedPattern) {
	int currentKey = getLineKey(line);
	int count = 0;
	for (int i = 0; i < 8; i++) {
		if (line[i] == STATE_BLANK) {
			line[i] = STATE_SELF;
			if (patternLib[getLineKey(line)] == comparedPattern && patternLib[currentKey] == PATTERN_NULL) {
				count++;
			}
			line[i] = STATE_BLANK;
		}
	}
	//活四需要判断两个点，其他的只需要一个
	bool countCheck = false;
	if (targetPattern == PATTERN_ACTIVE_FOUR) {
		countCheck = count >= 2;
	}
	else if (targetPattern == PATTERN_SLEEPY_FOUR) {
		countCheck = count == 1;
	}
	else {
		countCheck = count >= 1;
	}
	if (countCheck) {
		patternLib[currentKey] = targetPattern;
	}
}

void initPattern() {
	int line[8];
	//计算长5
	caculateLine(8, line,
		[](int line[]) {
		for (int i = 0; i < 5; i++) {
			int count = 0;
			for (int k = i; k < i + 4; k++) {
				if (line[k] == STATE_SELF) {
					count++;
				}
			}
			if (count == 4) {
				patternLib[getLineKey(line)] = PATTERN_LINE_FIVE;
				return;
			}
		}
	});

	//计算长5以外的情形
	int targetPatterns[] = { PATTERN_ACTIVE_FOUR, PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE, 
		PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO };
	int comparedPatterns[] = { PATTERN_LINE_FIVE, PATTERN_LINE_FIVE, PATTERN_ACTIVE_FOUR, 
		PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE, PATTERN_SLEEPY_THREE };
	for (int i = 0; i < 6; i++) {
		int targetPattern = targetPatterns[i];
		int comparedPattern = comparedPatterns[i];
		caculateLine(8, line,
			[targetPattern, comparedPattern](int line[]) {
			abstructCalculate(line, targetPattern, comparedPattern);
		});
	}
}

/* 
********************************* 测试代码分隔 ************************************
*/


static char getRealPoint(int value) {
	if (value == STATE_SELF)
		return 'O';
	if (value == STATE_OTHER)
		return 'X';
	return '.';
}

static void printLine(int line[]) {
	for (int i = 0; i < 8; i++) {
		if (i == 4)
			printf("v");
		printf("%c", getRealPoint(line[i]));
	}
	printf("\n");
}

static void printKey(int key) {
	for (int i = 0; i < 8; i++) {
		if (i == 4)
			printf("v");
		printf("%d", key % 4);
		key = key >> 2;
	}
	printf("\n");
}

static void printPatterns() {
	int line[8];
	//输出查看
	char* patternName[] = { "长5", "活4", " 眠4", "活3",
		"眠3", "活2", "眠2" };
	int patternCodes[] = { PATTERN_LINE_FIVE,PATTERN_ACTIVE_FOUR, PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE,
		PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO };

	for (int i = 0; i < 7; i++) {
		int patternCode = patternCodes[i];
		printf("%s\n", patternName[i]);
		caculateLine(8, line,
			[patternCode](int line[]) {
			if (patternLib[getLineKey(line)] == patternCode) {
				printLine(line);
			}
		});
	}
}

void testPattern() {
	initPattern();
	printPatterns();
}