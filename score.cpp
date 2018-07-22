#include "stdafx.h"
#include "score.h"
#include "gamemap.h"

static int value;

extern int blackPatternCount[8];
extern int whitePatternCount[8];

int score[10] = { 0,100000,100000,100,100,50,50,20, 2, 1 };

int getScoreValue(Color color, Color aiColor) {
	int result = 0;
	if (color == WHITE) {
		for (int i = 1; i < 10; i++) {
			result += score[i] * whitePatternCount[i];
			result -= score[i] * blackPatternCount[i];
		}
	}
	if (color == BLACK) {
		for (int i = 1; i < 10; i++) {
			result -= score[i] * whitePatternCount[i];
			result += score[i] * blackPatternCount[i];
		}
	}
	if (color != aiColor) {
		return -result;
	}

	return result;
}