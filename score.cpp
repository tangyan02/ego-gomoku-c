#include "stdafx.h"
#include "score.h"
#include "gamemap.h"

static int value;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1 };
static int directY[] = { 1, 1, 0, -1 };

//ʹ�ö�̬�滮
//���壺    f[i][j][k] ��ʾ��i,jΪ�յ㣬�ڷ���k�ϵ�ͳ��
//ת�Ʒ���: f[i][j][k] = a[i][j][k] + f[i-dx[k]][j-dy[k]][k]
static int blackCount[20][20][4];
static int whiteCount[20][20][4];

void initScore(Color aiColor)
{
	value = 0;
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			for (int k = 0; k < 4; k++) {
				blackCount[i][j][k] = 0;
				whiteCount[i][j][k] = 0;

			}
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			Color color = getColor(i, j);
			if (color != NULL) {
				updateScore(i, j, color, NULL, aiColor);
			}
		}
	}
}

void updateScore(int px, int py, Color color, Color forwardColor, Color aiColor)
{
	for (int i = 0; i < 4; i++) {
		int x = px - directX[i];
		int y = py - directY[i];
		for (int k = 0; k < 5; k++) {
			x += directX[i];
			y += directY[i];
			int headX = x - directX[i] * 4;
			int headY = y - directY[i] * 4;
			if (!reachable(headX, headY)) {
				continue;
			}
			if (!reachable(x, y)) {
				continue;
			}
			if (forwardColor == NULL) {
				value -= getValueByCount(blackCount[x][y][i], whiteCount[x][y][i], aiColor);
				if (color == BLACK) {
					blackCount[x][y][i]++;
				}
				if (color == WHITE) {
					whiteCount[x][y][i]++;
				}
				value += getValueByCount(blackCount[x][y][i], whiteCount[x][y][i], aiColor);
			}
			if (forwardColor != NULL) {
				value -= getValueByCount(blackCount[x][y][i], whiteCount[x][y][i], aiColor);
				if (color == NULL) {
					if (forwardColor == BLACK) {
						blackCount[x][y][i]--;
					}
					if (forwardColor == WHITE) {
						whiteCount[x][y][i]--;
					}
				}
				value += getValueByCount(blackCount[x][y][i], whiteCount[x][y][i], aiColor);
			}
		}
	}
}

int getScoreValue(Color color, Color aiColor)
{
	if (color != aiColor)
		return -value;
	return value;
}

int getColorCount(Color color, int x, int y, int direct)
{
	if (color == BLACK)
		return blackCount[x][y][direct];
	if (color == WHITE)
		return whiteCount[x][y][direct];
	return 0;
}


int getValueByCount(int blackCount, int whiteCount, Color color)
{
	int ONE = 4;
	int TWO = 20;
	int THREE = 40;
	int FOUR = 90;
	float weight = 1.0f;

	int valueWhite = 0;
	if (blackCount == 0) {
		if (whiteCount == 1)
			valueWhite += ONE;
		if (whiteCount == 2)
			valueWhite += TWO;
		if (whiteCount == 3)
			valueWhite += THREE;
		if (whiteCount == 4)
			valueWhite += FOUR;
	}

	int valueBlack = 0;
	if (whiteCount == 0) {
		if (blackCount == 1)
			valueBlack += ONE;
		if (blackCount == 2)
			valueBlack += TWO;
		if (blackCount == 3)
			valueBlack += THREE;
		if (blackCount == 4)
			valueBlack += FOUR;
	}

	int value = 0;
	if (color == BLACK) {
		return valueBlack - (int)(valueWhite * weight);
	}
	if (color == WHITE) {
		return valueWhite - (int)(valueBlack * weight);
	}
	return value;
}
