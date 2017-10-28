#include "stdafx.h"
#include "comboProcessor.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include  "gameMap.h"
#include  "cache.h"

static int currentLevel;

static long long startTime;

static long long limitTime;

comboResult result;

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

static bool timeOutEnable;

extern bool fourDefenceTable[MAX_TABLE_SIZE];
extern bool threeDefenceTable[MAX_TABLE_SIZE][4][4];
extern bool threeAttackTable[MAX_TABLE_SIZE];

void setColor(int px, int py, Color color, Color forwardColor, Color aiColor) {
	updateScore(px, py, color, forwardColor, aiColor);
	setColor(px, py, color);
}

points getComboAttackPoints(analyzeData *data, ComboType comboType) {
	//����жԷ���4�������4
	points result;
	if (data->fourDefence.count > 0) {
		result.add(data->fourDefence.list[0]);
		return result;
	}
	//����жԷ���3������
	if (data->threeDefence.count > 0) {
		result.addMany(data->fourAttack);
		return result;
	}

	result.addMany(data->fourAttack);
	if (comboType == THREE_COMBO) {
		result.addMany(data->threeAttack);
	}
	return result;
}

points getComboDefencePoints(analyzeData *data, ComboType comboType) {
	points result;
	//����жԷ���4�������4
	if (data->fourDefence.count > 0) {
		result.add(data->fourDefence.list[0]);
		return result;
	}
	if (comboType == THREE_COMBO) {
		//����жԷ���3�������3���߳���
		if (data->threeDefence.count > 0) {
			result.addMany(data->threeDefence);
			//result.addMany(data->fourAttack);
			return result;
		}
	}
	return result;
}

void recorad(long long hashCode, int value, int level, ComboType type, Color color) {
	if (timeOutEnable) {
		if (getSystemTime() - startTime > limitTime)
			return;
	}
	addComboEntry(hashCode, value, level, type, color);
}

bool dfsKill(Color color, Color targetColor, int level, ComboType comboType, point* father, point* grandFather) {
	//��ʱ�ж�
	if (timeOutEnable) {
		if (getSystemTime() - startTime > limitTime) {
			result.timeOut = true;
			return false;
		}
	}
	//��ѯ����
	long long hashCode = getMapHashCode();
	int comboValue = getComboValue(hashCode, level, comboType, targetColor);
	if (comboValue != COMBO_EMPTY) {
		//if (comboValue == COMBO_TRUE)
		//	return true;
		//if (comboValue == COMBO_FALSE)
		//	return false;
	}

	if (level == 0) {
		result.node++;
		return false;
	}
	//����ǰ������Χ�ĵ�
	points basePoints;
	if (father == nullptr || grandFather == nullptr)
		basePoints = getNeighbor();
	else {
		pointHash hash;
		if (color != targetColor) {
			points fatherPoints = getPointLinesNeighbor(father->x, father->y);
			for (int i = 0; i < fatherPoints.count; i++) {
				point p = fatherPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(fatherPoints.list[i]);
				}
			}
		}
		else {
			points fatherPoints = getPointLinesNeighbor(father->x, father->y);
			for (int i = 0; i < fatherPoints.count; i++) {
				point p = fatherPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(fatherPoints.list[i]);
				}
			}
			points grandPoints = getPointLinesNeighbor(grandFather->x, grandFather->y);
			for (int i = 0; i < grandPoints.count; i++) {
				point p = grandPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(grandPoints.list[i]);
				}
			}
		}
	}
	analyzeData data = getAnalyzeData(color, &basePoints);
	if (data.threeDefence.count > 0 && color == targetColor) {
		comboType = FOUR_COMBO;
	}

	if (color == targetColor) {
		if (data.fiveAttack.count > 0) {
			if (level == currentLevel) {
				result.p = data.fiveAttack.list[0];
				result.win = true;
			}
			recorad(hashCode, true, level, comboType, targetColor);
			return true;
		}
		points ps = getComboAttackPoints(&data, comboType);
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p.x, p.y, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p.x, p.y, NULL, color, targetColor);
			if (level == currentLevel && value) {
				result.p = p;
				result.win = true;
			}
			if (value) {
				recorad(hashCode, true, level, comboType, targetColor);
				return true;
			}
		}
		recorad(hashCode, false, level, comboType, targetColor);
		return false;
	}
	else {
		if (data.fiveAttack.count > 0) {
			recorad(hashCode, false, level, comboType, targetColor);
			return false;
		}
		points ps = getComboDefencePoints(&data, comboType);
		//���û���ܷ��������
		if (ps.count == 0) {
			recorad(hashCode, false, level, comboType, targetColor);
			return false;
		}
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p.x, p.y, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p.x, p.y, NULL, color, targetColor);
			if (!value) {
				recorad(hashCode, false, level, comboType, targetColor);
				return false;
			}
		}
		recorad(hashCode, true, level, comboType, targetColor);
		return true;
	}
}


comboResult canKillThree(Color targetColor, int level) {
	if (level % 2 == 0) {
		level++;
	}
	currentLevel = level;
	//�����ҷ�����ɱ
	result.reset();
	bool value = dfsKill(targetColor, targetColor,
		level, THREE_COMBO, nullptr, nullptr);
	return result;
}

comboResult canKillFour(Color targetColor, int level) {
	if (level % 2 == 0) {
		level++;
	}
	currentLevel = level;
	result.reset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	return result;
}

comboResult canKill(Color targetColor, int level, long long startTimeValue, long long limitTimeValue)
{
	timeOutEnable = true;
	if (level % 2 == 0) {
		level++;
	}
	startTime = startTimeValue;
	limitTime = limitTimeValue;
	currentLevel = level;

	//�����������ɱ���п��ܻ��治����
	/*result.reset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = true;
		return result;
	}*/

	//�����������ɱ
	result.reset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.win = false;
		return result;
	}

	//�����ҷ�����ɱ
	result.reset();
	dfsKill(targetColor, targetColor,
		level, THREE_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = false;
		return result;
	}
	return result;
}
