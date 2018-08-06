#include "stdafx.h"
#include "comboProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];



static bool killDfs(int level, Color color, Color aiColor) {


	if (level == 0) {
		return false;
	}

	points* ps;
	fillNeighbor(ps);

	for (int i = 0; i < ps->count; i++) {
		int x = ps->list[i].x;
		int y = ps->list[i].y;
		move(x, y, color, aiColor);
		bool value = killDfs(level - 1, color, aiColor);
		undoMove(x, y, color, aiColor);

		if (color == aiColor) {
			if (value) {
				return false;
			}
		}

		if (color != aiColor) {
			if (!value) {
				return false;
			}
		}

	}

	if (color == aiColor) {
		return false;
	}

	if (color != aiColor) {
		return true;
	}
}

point * kill(Color color, int level)
{
	return nullptr;
}
