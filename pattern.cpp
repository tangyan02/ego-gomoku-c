#include "stdafx.h"
#include "pattern.h"
#include "functional"

static int directX[] = { 0, 1, 1, 1 };
static int directY[] = { 1, 1, 0, -1 };

int patternLib[PATTERN_SIZE] = { PATTERN_NULL };
int patternScore[PATTERN_SIZE] = { PATTERN_NULL };

static int baseScore[10] = { 0, 100000, 10000, 51 ,58, 22 ,21, 8 ,10, 8 };
static int sigma = 2;

static bool inited = false;

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

static void updatePatternScore() {
	int line[8];
	caculateLine(8, line,
		[](int line[]) {
			int currentKey = getLineKey(line);
			if (patternLib[currentKey] != PATTERN_NULL) {
				patternScore[currentKey] = baseScore[patternLib[currentKey]];
				if (patternLib[currentKey] != PATTERN_LINE_FIVE && patternLib[currentKey] != PATTERN_ACTIVE_FOUR) {
					int min = 100000;
					for (int i = 0; i < 8; i++) {
						if (line[i] == STATE_BLANK) {
							line[i] = STATE_OTHER;

							int score = baseScore[patternLib[getLineKey(line)]];
							if (score < min) {
								min = score;
							}

							line[i] = STATE_BLANK;
						}
					}
					if (sigma > 0) {
						patternScore[currentKey] += min / sigma;
					}
				}
			}
		});
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
	if (inited) {
		return;
	}
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
		PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO, PATTERN_ACTIVE_ONE, PATTERN_SLEEPY_ONE };
	int comparedPatterns[] = { PATTERN_LINE_FIVE, PATTERN_LINE_FIVE, PATTERN_ACTIVE_FOUR, 
		PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE, PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO };

	for (int i = 0; i < 8; i++) {
		int targetPattern = targetPatterns[i];
		int comparedPattern = comparedPatterns[i];
		caculateLine(8, line,
			[targetPattern, comparedPattern](int line[]) {
			abstructCalculate(line, targetPattern, comparedPattern);
		});
	}

	updatePatternScore();
	//分数加成赋值
	inited = true;
}

void setBaseScore(int score[], int k)
{
	for (int i = 0; i < 10; i++) {
		baseScore[i] = score[i];
	}
	sigma = k;
	updatePatternScore();
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
	printf(" %d", patternScore[getLineKey(line)]);
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
		"眠3", "活2", "眠2", "活1", "眠1" };
	int patternCodes[] = { PATTERN_LINE_FIVE,PATTERN_ACTIVE_FOUR, PATTERN_SLEEPY_FOUR, PATTERN_ACTIVE_THREE,
		PATTERN_SLEEPY_THREE, PATTERN_ACTIVE_TWO, PATTERN_SLEEPY_TWO, PATTERN_ACTIVE_ONE, PATTERN_SLEEPY_ONE };

	for (int i = 0; i < 9; i++) {
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