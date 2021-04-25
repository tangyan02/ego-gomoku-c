#include "stdafx.h"
#include "score.h"
#include "gameMap.h"

extern int blackPatternCount[10];
extern int whitePatternCount[10];

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int boardSize;

extern int blackPatternTotalScore;
extern int whitePatternTotalScore;

extern Color** map;

//int score[10] = { 0, 100000, 10000, 80, 90, 40, 50, 20, 25, 10 };

//extern int baseScore[10];

int getScoreValue(Color color, Color aiColor) {
	int result = 0;
	if (color == WHITE) {
		return whitePatternTotalScore - blackPatternTotalScore;
	}
	if (color == BLACK) {
		return blackPatternTotalScore - whitePatternTotalScore;
	}
	if (color != aiColor) {
		result = -result;
	}
	return result;
}

//int getScoreValue(Color color, Color aiColor) {
//	int result = 0;
//	if (color == WHITE) {
//		for (int i = 1; i < 10; i++) {
//			result += baseScore[i] * whitePatternCount[i];
//			result -= baseScore[i] * blackPatternCount[i];
//		}
//	}
//	if (color == BLACK) {
//		for (int i = 1; i < 10; i++) {
//			result -= baseScore[i] * whitePatternCount[i];
//			result += baseScore[i] * blackPatternCount[i];
//		}
//	}
//	if (color != aiColor) {
//		result = -result;
//	}
//	return result;
//}

/***************************²âÊÔ´úÂë·Ö¸ô***********************************/

//int getValueByPattern(Color color) {
//	int result = 0;
//	if (color == WHITE) {
//		for (int i = 0; i < boardSize; i++)
//			for (int j = 0; j < boardSize; j++) {
//				for (int k = 0; k < 4; k++) {
//					if (map[i][j] == WHITE) {
//						result += baseScore[whitePattern[i][j][k]];
//					}
//					if (map[i][j] == BLACK) {
//						result -= baseScore[blackPattern[i][j][k]];
//					}
//				}
//			}
//	}
//	if (color == BLACK) {
//		for (int i = 0; i < boardSize; i++)
//			for (int j = 0; j < boardSize; j++)
//				for (int k = 0; k < 4; k++) {
//					if (map[i][j] == WHITE) {
//						result -= baseScore[whitePattern[i][j][k]];
//					}
//					if (map[i][j] == BLACK) {
//						result += baseScore[blackPattern[i][j][k]];
//					}
//				}
//	}
//	return result;
//}