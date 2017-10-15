#include "stdafx.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include "unordered_map"

#define ERROR -1

#define NULL_COLOR 0
#define CURRENT_COLOR 1
#define OTHER_COLOR 2
#define INVALID_COLOR 3

#define NO_TYPE 0
#define FIVA_ATTACK 1
#define FOUR_ATTACK 2
#define FOUR_DEFENCE 3
#define THREE_ATTACK 4
#define THREE_DEFENCE 5
#define TWO_ATTACK 6

static const int maxTableSize = 65536;

static bool fiveAttackTable[maxTableSize];
static bool fourAttackTable[maxTableSize];
static bool fourDefenceTable[maxTableSize];
static bool threeAttackTable[maxTableSize];
static bool threeDefenceTable[maxTableSize][4][4];
static bool twoAttackTable[maxTableSize];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

int getPointTableColor(Color** map, int x, int y, Color targetColor) {
	if (!reachable(x, y))
		return INVALID_COLOR;
	if (map[x][y] == NULL)
		return NULL_COLOR;
	if (map[x][y] == targetColor)
		return CURRENT_COLOR;
	if (map[x][y] == getOtherColor(targetColor))
		return OTHER_COLOR;
	return ERROR;
}

void printKey(int key) {
	for (int i = 0; i < 8; i++) {
		printf("%d", key % 4);
		key = key >> 2;
	}
	printf("\n");
}

int getLineKey(int line[]) {
	int key = 0;
	for (int i = 0; i < 8; i++) {
		key = key << 2;
		key |= line[i];
	}
	return key;
}

int getLineKey(Color** map, point p, int direct, Color targetColor) {
	int key = 0;
	int line[8];
	int x = p.x - 4 * directX[direct];
	int y = p.y - 4 * directY[direct];
	//printf("target color %d\n", targetColor);
	for (int i = 0; i < 4; i++) {
		//printf("color[%d][%d] %d \n", x, y, map[x][y]);
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
		x += directX[direct];
		y += directY[direct];
	}

	for (int i = 4; i < 8; i++) {
		x += directX[direct];
		y += directY[direct];
		//printf("color[%d][%d] %d \n", x, y, map[x][y]);
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
	}
	//printMapWithStar(map, p);
	//printf("%d %d - %d\n", p.x, p.y, direct);
	//for (int i = 0; i < 8; i++) {
	//	printf("%d", line[i]);
	//}
	//printf("\n");
	return getLineKey(line);
}

void caculate(int line[]) {
	int currentCountList[8] = { 0 };
	int otherCountList[8] = { 0 };
	for (int i = 3; i < 8; i++) {
		int head = i - 3;
		int current = 0;
		int other = 0;
		if (line[head] == INVALID_COLOR || line[i] == INVALID_COLOR) {
			continue;
		}
		for (int j = head; j <= i; j++) {
			if (line[j] == CURRENT_COLOR)
				current++;
			if (line[j] == OTHER_COLOR)
				other++;
		}
		currentCountList[i] = current;
		otherCountList[i] = other;
	}

	//表格计算
	int key = getLineKey(line);
	for (int i = 3; i < 8; i++) {
		int head = i - 3;
		if (line[head] == INVALID_COLOR || line[i] == INVALID_COLOR) {
			continue;
		}
		int current = currentCountList[i];
		int other = otherCountList[i];
		if (current == 4 && other == 0) {
			fiveAttackTable[key] = true;
		}
		if (current == 3 && other == 0) {
			fourAttackTable[key] = true;
		}
		if (current == 0 && other == 4) {
			fourDefenceTable[key] = true;
		}
		if (current == 1 && other == 0) {
			twoAttackTable[key] = true;
		}
	}

	for (int i = 4; i < 7; i++) {
		int current = currentCountList[i];
		int other = otherCountList[i];
		int headColor = line[i - 3];
		int tailColor = line[i];
		//中间情形
		if (i >= 4 && i < 7) {
			int headSideColor = line[i - 4];
			int tailSideColor = line[i + 1];
			if (headColor == INVALID_COLOR || tailColor == INVALID_COLOR) {
				continue;
			}
			if (current == 2 && other == 0) {
				if (headColor == NULL_COLOR && tailColor == NULL_COLOR) {
					if (headSideColor == NULL_COLOR || tailSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
				if (headColor == NULL_COLOR && tailColor != NULL_COLOR) {
					if (tailSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
				if (headColor != NULL_COLOR && tailColor == NULL_COLOR) {
					if (headSideColor == NULL_COLOR) {
						threeAttackTable[key] = true;
					}
				}
			}
			for (int left = 0; left < 4; left++)
				for (int right = 0; right < 4; right++) {
					if (current == 0 && other == 3) {
						if (headColor == NULL_COLOR && tailColor != NULL_COLOR) {
							if (tailSideColor == NULL_COLOR) {
								threeDefenceTable[key][left][right] = true;
							}
						}
						if (headColor != NULL_COLOR && tailColor == NULL_COLOR) {
							if (headSideColor == NULL_COLOR) {
								threeDefenceTable[key][left][right] = true;
							}
						}
					}
				}
		}
		else {
			//两边的防3有特殊情况
			for (int left = 0; left < 4; left++)
				for (int right = 0; right < 4; right++) {
					if (current == 0 && other == 3) {
						if (i == 3) {
							//OXXX-0000
							if (headColor == NULL_COLOR) {
								if (line[4] == NULL_COLOR || left == NULL_COLOR)
									threeDefenceTable[key][left][right] = true;
							}
							//XOXX-0000
							else
								if (left == NULL_COLOR)
									threeDefenceTable[key][left][right] = true;
						}
						if (i == 8) {
							//OOOO-XXXO
							if (tailColor == NULL_COLOR) {
								if (line[3] == NULL_COLOR || right == NULL_COLOR)
									threeDefenceTable[key][left][right] = true;
							}
							//OOOO-XOXX
							else
								if (right == NULL_COLOR)
									threeDefenceTable[key][left][right] = true;
						}
					}
				}
		}
	}
}

void caculate(Color** map, point p, int direct, Color targetColor) {
	int key = 0;
	int line[8];
	int x = p.x - 4 * directX[direct];
	int y = p.y - 4 * directY[direct];
	//printf("target color %d\n", targetColor);
	for (int i = 0; i < 4; i++) {
		//printf("color[%d][%d] %d \n", x, y, map[x][y]);
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
		x += directX[direct];
		y += directY[direct];
	}

	for (int i = 4; i < 8; i++) {
		x += directX[direct];
		y += directY[direct];
		//printf("color[%d][%d] %d \n", x, y, map[x][y]);
		if (!reachable(x, y))
			line[i] = INVALID_COLOR;
		else {
			if (map[x][y] == targetColor)
				line[i] = CURRENT_COLOR;
			if (map[x][y] == NULL)
				line[i] = NULL_COLOR;
			if (map[x][y] == getOtherColor(targetColor))
				line[i] = OTHER_COLOR;
		}
	}
	caculate(line);
}


void buildLine(int i, int line[]) {
	if (i == 0) {
		caculate(line);
		return;
	}
	for (int type = 0; type < 4; type++) {
		line[i - 1] = type;
		buildLine(i - 1, line);
	}
}

void initAnalyze() {
	int line[8] = { 0 };
	buildLine(8, line);
}

analyzeData getAnalyzeData(Color color, points ps) {
	Color** map = getMap();
	analyzeData result;
	pointHash fiveAttackHash;
	pointHash fourAttackHash;
	pointHash fourDefenceHash;
	pointHash threeAttackHash;
	pointHash threeDenfenceHash;
	pointHash twoAttackHash;

	for (int i = 0; i < ps.count; i++) {
		bool ok = false;
		point p = ps.list[i];
		for (int k = 0; k < 4; k++) {
			int key = getLineKey(map, p, k, color);
			if (fiveAttackTable[key]) {
				if (!fiveAttackHash.contains(p)) {
					result.fiveAttack.add(p);
					fiveAttackHash.add(p);
				}
			}
			if (fourAttackTable[key]) {
				if (!fourAttackHash.contains(p)) {
					result.fourAttack.add(p);
					fourAttackHash.add(p);
				}
			}
			if (fourDefenceTable[key]) {
				if (!fourDefenceHash.contains(p)) {
					result.fourDefence.add(p);
					fourDefenceHash.add(p);
				}
			}
			if (threeAttackTable[key]) {
				if (!threeAttackHash.contains(p)) {
					result.threeAttack.add(p);
					threeAttackHash.add(p);
				}
			}
			point leftPoint = point(p.x - 5 * directX[k], p.y - 5 * directY[k]);
			point rightPoint = point(p.x + 5 * directX[k], p.y + 5 * directY[k]);
			int left = getPointTableColor(map, leftPoint.x, leftPoint.y, color);
			int right = getPointTableColor(map, rightPoint.x, rightPoint.y, color);
			if (threeDefenceTable[key][left][right])
				if (!threeDenfenceHash.contains(p)) {
					result.threeDefence.add(p);
					threeDenfenceHash.add(p);
				}
			if (twoAttackTable[key]) {
				if (!twoAttackHash.contains(p)) {
					result.twoAttack.add(p);
					twoAttackHash.add(p);
				}
			}
		}
	}

	return result;
}

analyzeData getAnalyzeData(Color color, points ps, bool moreAnalyze)
{
	Color** map = getMap();
	analyzeData result;
	pointHash fiveAttackHash;
	pointHash fourAttackHash;
	pointHash fourDefenceHash;
	pointHash threeAttackHash;
	pointHash threeDenfenceHash;
	pointHash twoAttackHash;

	//第一维表示当前局面哈希值，第二位表示方向的二进制集合码, 用位运算识别不同方向的复合
	//暂不考虑相同方向的复合
	unordered_map<int, int> weakFourDefenceMap;
	unordered_map<int, int> weakThreeDefenceMap;
	pointHash doubleWeakFourDefenceHash;
	pointHash weakThreeAndFourDefenceHash;
	pointHash doubleWeakThreeDefenceHash;

	for (int i = 0; i < ps.count; i++)
	{
		point p = ps.list[i];
		for (int direct = 0; direct < 4; direct++) {
			int x = p.x;
			int y = p.y;
			Color otherColor = getOtherColor(color);
			for (int k = 0; k < 5; k++) {
				int colorCount = getColorCount(color, x, y, direct);
				int otherColorCount = getColorCount(otherColor, x, y, direct);
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
					if (!fourDefenceHash.contains(p)) {
						result.fourDefence.add(p);
						fourDefenceHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 1) {
					if (!twoAttackHash.contains(p)) {
						result.twoAttack.add(p);
						twoAttackHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 2) {
					if (k != 0 && k != 4) {
						int headX = x - directX[direct] * 4;
						int headY = y - directY[direct] * 4;
						if (reachable(headX, headY)) {
							Color headColor = map[headX][headY];
							Color tailColor = map[x][y];
							if (headColor == NULL && tailColor == NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
								sideX = headX - directX[direct];
								sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
							if (headColor == NULL && tailColor != NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
							if (headColor != NULL && tailColor == NULL) {
								int sideX = headX - directX[direct];
								int sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
						}
					}
				}
				if (otherColorCount == 3 && colorCount == 0) {
					int headX = x - directX[direct] * 4;
					int headY = y - directY[direct] * 4;
					if (reachable(headX, headY)) {
						Color headColor = map[headX][headY];
						Color tailColor = map[x][y];
						if (headColor != NULL && tailColor == NULL) {
							if (map[x - directX[direct]][y - directY[direct]] != NULL) {
								int sideX = headX - directX[direct];
								int sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeDenfenceHash.contains(p)) {
											result.threeDefence.add(p);
											threeDenfenceHash.add(p);
										}
									}
								}
							}
						}
						if (tailColor != NULL && headColor == NULL) {
							if (map[headX + directX[direct]][headY + directY[direct]] != NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeDenfenceHash.contains(p)) {
											result.threeDefence.add(p);
											threeDenfenceHash.add(p);
										}
									}
								}
							}
						}
						if (headColor == NULL && tailColor == NULL) {
							int sideX = x + directX[direct];
							int sideY = y + directY[direct];
							point sidePoint(sideX, sideY);
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								if (!threeDenfenceHash.contains(p)) {
									result.threeDefence.add(p);
									threeDenfenceHash.add(p);
								}
								if (!threeDenfenceHash.contains(sidePoint)) {
									result.threeDefence.add(sidePoint);
									threeDenfenceHash.add(sidePoint);
								}
							}
							sideX = headX - directX[direct];
							sideY = headY - directY[direct];
							point sidePoint2(sideX, sideY);
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								if (!threeDenfenceHash.contains(p)) {
									result.threeDefence.add(p);
									threeDenfenceHash.add(p);
								}
								if (!threeDenfenceHash.contains(sidePoint2)) {
									result.threeDefence.add(sidePoint2);
									threeDenfenceHash.add(sidePoint2);
								}
							}
						}
					}
				}

				//复杂的分析
				if (moreAnalyze) {
					if (otherColorCount == 3 && colorCount == 0) {
						//双四
						//此处位运算操作是判断是和其他三个方向做判断
						if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
							if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
								if (!doubleWeakFourDefenceHash.contains(p)) {
									doubleWeakFourDefenceHash.add(p);
									result.doubleWeakFourDefence.add(p);
								}
							}
						}
						//三四
						if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
							if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
								if (!weakThreeAndFourDefenceHash.contains(p)) {
									weakThreeAndFourDefenceHash.add(p);
									result.weakThreeAndFourDefence.add(p);
								}
							}
						}

						if (weakFourDefenceMap.find(p.hash()) == weakFourDefenceMap.end()) {
							result.weakFourDefence.add(p);
						}
						weakFourDefenceMap[p.hash()] |= 1 << direct;
					}
					if (otherColorCount == 2 && colorCount == 0) {
						int headX = x - directX[direct] * 4;
						int headY = y - directY[direct] * 4;
						if (k != 0 && k != 4) {
							if (reachable(headX, headY)) {
								Color headColor = map[headX][headY];
								Color tailColor = map[x][y];
								if (headColor == NULL && tailColor == NULL) {
									int sideX = x + directX[direct];
									int sideY = y + directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//双三
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//四三
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
									sideX = headX - directX[direct];
									sideY = headY - directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//双三
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//四三
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
								if (headColor == NULL && tailColor != NULL) {
									int sideX = x + directX[direct];
									int sideY = y + directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//双三
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//四三
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
								if (headColor != NULL && tailColor == NULL) {
									int sideX = headX - directX[direct];
									int sideY = headY - directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//双三
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//四三
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
							}
						}
					}
				}
				x += directX[direct];
				y += directY[direct];
				if (!reachable(x, y)) {
					break;
				}
			}
		}
	}
	return result;
}
