#pragma once

#define PATTERN_SIZE 1<<16

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
#define PATTERN_ACTIVE_ONE 8
#define PATTERN_SLEEPY_ONE 9

void initPattern();

int getLineKey(int line[]);

void setBaseScore(int score[], int k);

void testPattern();

