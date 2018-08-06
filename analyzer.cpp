#include "stdafx.h"
#include "analyzer.h"
#include "patternRecorder.h"

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

bool canWinCheck(Color color) {
	if (color == BLACK) {
		if (blackPatternCountInNull[PATTERN_LINE_FIVE] > 0) {
			return true;
		}
	}
	if (color == WHITE) {
		if (whitePatternCountInNull[PATTERN_LINE_FIVE] > 0) {
			return true;
		}
	}
	return false;
}