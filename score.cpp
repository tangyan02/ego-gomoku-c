#include "stdafx.h"
#include "score.h"
#include "gamemap.h"

extern int blackPatternCount[8];
extern int whitePatternCount[8];

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int boardSize;

extern Color** map;

int score[10] = { 0, 100000, 10000, 90, 120, 60, 90, 45, 22, 22 };

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

/***************************���Դ���ָ�***********************************/

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