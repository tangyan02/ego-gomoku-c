#include "stdafx.h"
#include "score.h"
#include "gamemap.h"

extern int blackPatternCount[10];
extern int whitePatternCount[10];

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int boardSize;

extern Color** map;

int score[10] = { 0, 100000, 10000, 80, 90, 40, 50, 20, 25, 10 };

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
		result = -result;
	}
	return result;
}

/***************************²âÊÔ´úÂë·Ö¸ô***********************************/

int getValueByPattern(Color color) {
	int result = 0;
	if (color == WHITE) {
		for (int i = 0; i < boardSize; i++)
			for (int j = 0; j < boardSize; j++) {
				for (int k = 0; k < 4; k++) {
					if (map[i][j] == WHITE) {
						result += score[whitePattern[i][j][k]];
					}
					if (map[i][j] == BLACK) {
						result -= score[blackPattern[i][j][k]];
					}
				}
			}
	}
	if (color == BLACK) {
		for (int i = 0; i < boardSize; i++)
			for (int j = 0; j < boardSize; j++)
				for (int k = 0; k < 4; k++) {
					if (map[i][j] == WHITE) {
						result -= score[whitePattern[i][j][k]];
					}
					if (map[i][j] == BLACK) {
						result += score[blackPattern[i][j][k]];
					}
				}
	}
	return result;
}