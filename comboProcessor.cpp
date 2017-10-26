#include "stdafx.h"
#include "comboProcessor.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include  "gameMap.h"
#include  "cache.h"

#define ComboType int
#define THREE_COMBO 0
#define FOUR_COMBO 1

static int currentLevel;

static long long startTime;

static long long limitTime;

comboResult result;

void setColor(int px, int py, Color color, Color forwardColor, Color aiColor) {
	updateScore(px, py, color, forwardColor, aiColor);
	setColor(px, py, color);
}

points getComboAttackPoints(analyzeData data, ComboType comboType) {
	//����жԷ���4�������4
	points result;
	if (data.fourDefence.count > 0) {
		result.add(data.fourDefence.list[0]);
		return result;
	}
	//����жԷ���3������
	if (data.threeDefence.count > 0) {
		result.addMany(data.fourAttack);
		return result;
	}

	result.addMany(data.fourAttack);
	if (comboType == THREE_COMBO) {
		result.addMany(data.threeAttack);
	}
	return result;
}

points getComboDefencePoints(analyzeData data, ComboType comboType) {
	points result;
	//����жԷ���4�������4
	if (data.fourDefence.count > 0) {
		result.add(data.fourDefence.list[0]);
		return result;
	}
	if (comboType == THREE_COMBO) {
		//����жԷ���3�������3���߳���
		if (data.threeDefence.count > 0) {
			result.addMany(data.threeDefence);
			result.addMany(data.fourAttack);
			return result;
		}
	}
	return result;
}

void recordCombo(bool value) {
	addComboEntry(getMapHashCode(), value);
}

bool dfsKill(Color color, Color targetColor, int level, ComboType comboType, point* father, point* grandFather) {
	//��ʱ�ж�
	if (getSystemTime() - startTime > limitTime) {
		result.timeOut = true;
		return false;
	}
	//��ѯ����
	long long hashCode = getMapHashCode();
	int comboValue = getComboValue(hashCode);
	if (comboValue != COMBO_EMPTY) {
		if (comboValue == COMBO_TRUE)
			return true;
		else
			return false;
	}

	if (level == 0) {
		result.node++;
		return false;
	}
	//����ǰ������Χ�ĵ�
	points basePoints;
	if (father == nullptr)
		basePoints = getNeighbor();
	else {
		if (grandFather == nullptr)
			basePoints = getPointLinesNeighbor(father->x, father->y);
		else {
			points fatherPoints = getPointLinesNeighbor(father->x, father->y);
			points grandPoints = getPointLinesNeighbor(grandFather->x, grandFather->y);
			pointHash hash;
			for (int i = 0; i < fatherPoints.count; i++) {
				point p = fatherPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(fatherPoints.list[i]);
				}
			}
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
	//��������γɻ�������ת��Ϊ����
	if (comboType == THREE_COMBO) {
		if (color == targetColor && data.threeDefence.count > 0) {
			comboType = FOUR_COMBO;
		}
	}
	if (color == targetColor) {
		if (data.fiveAttack.count > 0) {
			if (level == currentLevel) {
				result.p = data.fiveAttack.list[0];
				result.win = true;
			}
			recordCombo(true);
			return true;
		}
		points ps = getComboAttackPoints(data, comboType);
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
				recordCombo(true);
				return true;
			}
		}
		recordCombo(false);
		return false;
	}
	else {
		if (data.fiveAttack.count > 0) {
			recordCombo(false);
			return false;
		}
		points ps = getComboDefencePoints(data, comboType);
		//���û���ܷ��������
		if (ps.count == 0) {
			recordCombo(false);
			return false;
		}
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p.x, p.y, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p.x, p.y, NULL, color, targetColor);
			if (!value) {
				recordCombo(false);
				return false;
			}
		}
		recordCombo(true);
		return true;
	}
}


bool canKillThree(Color targetColor, int level) {
	currentLevel = level;
	startTime = 0;
	limitTime = 10 * 1000;
	currentLevel = level;
	//�����ҷ�����ɱ
	result.reset();
	cacheReset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) 
		return true;

	//�����������ɱ
	result.reset();
	cacheReset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) 
		result.win = false;
		return false;
	

	//�����ҷ�����ɱ
	result.reset();
	cacheReset();
	dfsKill(targetColor, targetColor,
		level, THREE_COMBO, nullptr, nullptr);
	if (result.win) 
		return true;
	return false;
}

bool canKillFour(Color targetColor, int level) {
	currentLevel = level;
	result.reset();
	cacheReset();
	startTime = 0;
	limitTime = 10 * 1000;
	return dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
}

comboResult canKill(Color targetColor, int level, long long startTimeValue, long long limitTimeValue)
{
	if (level % 2 == 0) {
		level++;
	}
	startTime = startTimeValue;
	limitTime = limitTimeValue;
	currentLevel = level;
	//�����ҷ�����ɱ
	result.reset();
	cacheReset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = true;
		return result;
	}
	//�����������ɱ
	result.reset();
	cacheReset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.win = false;
		return result;
	}

	//�����ҷ�����ɱ
	result.reset();
	cacheReset();
	dfsKill(targetColor, targetColor,
		level, THREE_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = false;
		return result;
	}
	return result;
}
