#include "stdafx.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"


static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

analyzeData getAnalyzeData(Color color, points ps)
{
	Color** map = getMap();
	analyzeData result;
	pointHash fiveAttackHash = pointHash(boardSize);
	pointHash fourAttackHash = pointHash(boardSize);
	pointHash fourDenfenceHash = pointHash(boardSize);
	pointHash threeAttackHash = pointHash(boardSize);
	pointHash threeDenfenceHash = pointHash(boardSize);

	for (int i = 0; i < ps.count; i++)
	{
		point p = ps.list[i];
		for (int i = 0; i < 4; i++) {
			int x = p.x;
			int y = p.y;
			Color otherColor = getOtherColor(color);
			for (int k = 0; k < 5; k++) {
				int colorCount = getColorCount(color, x, y, i);
				int otherColorCount = getColorCount(otherColor, x, y, i);
				if (otherColorCount == 0 && colorCount == 4) {
					if (!fiveAttackHash.contains(p)) {
						result.fiveAttack.add(p);
						fiveAttackHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 3) {
					if (!fourAttackHash.contains(p)) {
						result.fourAttack.add(p);
						fourAttackHash.add(p);
					}
				}
				if (otherColorCount == 4 && colorCount == 0) {
					fourDefence.add(point);
				}
				if (otherColorCount == 0 && colorCount == 2) {
					if (k != 0 && k != 4) {
						int headX = x - directX[i] * 4;
						int headY = y - directY[i] * 4;
						if (reachable(headX, headY)) {
							Color headColor = map[headX][headY];
							Color tailColor = map[x][y];
							if (headColor == NULL && tailColor == NULL) {
								int sideX = x + directX[i];
								int sideY = y + directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeOpenAttack.add(point);
									}
								}
								sideX = headX - directX[i];
								sideY = headY - directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeOpenAttack.add(point);
									}
								}
							}
							if (headColor == NULL && tailColor != NULL) {
								int sideX = x + directX[i];
								int sideY = y + directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeOpenAttack.add(point);
									}
								}
							}
							if (headColor != NULL && tailColor == NULL) {
								int sideX = headX - directX[i];
								int sideY = headY - directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeOpenAttack.add(point);
									}
								}
							}
						}
					}
				}
				if (otherColorCount == 3 && colorCount == 0) {
					int headX = x - directX[i] * 4;
					int headY = y - directY[i] * 4;
					if (reachable(headX, headY)) {
						Color headColor = map[headX][headY];
						Color tailColor = map[x][y];
						if (headColor != NULL && tailColor == NULL) {
							if (map[x - directX[i]][y - directY[i]] != NULL) {
								int sideX = headX - directX[i];
								int sideY = headY - directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeDefence.add(point);
									}
								}
							}
						}
						if (tailColor != NULL && headColor == NULL) {
							if (map[headX + directX[i]][headY + directY[i]] != NULL) {
								int sideX = x + directX[i];
								int sideY = y + directY[i];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										threeDefence.add(point);
									}
								}
							}
						}
						if (headColor == NULL && tailColor == NULL) {
							int sideX = x + directX[i];
							int sideY = y + directY[i];
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								threeDefence.add(point);
							}
							sideX = headX - directX[i];
							sideY = headY - directY[i];
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								threeDefence.add(point);
							}
						}
					}
				}
				x += directX[i];
				y += directY[i];
				if (reachable(x, y)) {
					break;
				}
			}
		}
	}
	return result;
}
