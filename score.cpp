#include "stdafx.h"
#include "score.h"
#include "gamemap.h"

static int value;

extern int boardSize;

static int directX[] = { 0, 1, 1, 1 };
static int directY[] = { 1, 1, 0, -1 };

//使用动态规划
//定义：    f[i][j][k] 表示以i,j为终点，在方向k上的统计
//转移方程: f[i][j][k] = a[i][j][k] + f[i-dx[k]][j-dy[k]][k]
static int blackCount[20][20][4];
static int whiteCount[20][20][4];

void initScore(Color aiColor)
{
	value = 0;
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			point p = point(i, j);
			Color color = getColor(p);
			if (color != NULL) {
				updateScore(p, color, NULL, aiColor);
			}
		}
	}
}

void updateScore(point point, Color color, Color forwardColor, Color aiColor)
{
	for (int i = 0; i < 4; i++) {
		int x = point.x - directX[i];
		int y = point.y - directY[i];
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

int getScoreValue()
{
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
