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

void setColor(point point, Color color, Color forwardColor, Color aiColor) {
	updateScore(point, color, forwardColor, aiColor);
	setColor(point.x, point.y, color);
}

points getComboAttackPoints(analyzeData data, ComboType comboType) {
	//如果有对方冲4，则防冲4
	points result;
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	//如果有对方活3，冲四
	if (data.threeDenfence.count > 0) {
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
	//如果有对方冲4，则防冲4
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	if (comboType == THREE_COMBO) {
		//如果有对方活3，则防活3或者冲四
		if (data.threeDenfence.count > 0) {
			result.addMany(data.threeDenfence);
			//result.addMany(data.fourAttack);
			return result;
		}
	}
	return result;
}

void recordCombo(bool value) {
	addComboEntry(getMapHashCode(), value);
}

bool dfsKill(Color color, Color targetColor, int level, ComboType comboType, point* father, point* grandFather) {
	//超时判断
	if (getSystemTime() - startTime > limitTime) {
		result.timeOut = true;
		return false;
	}
	//查询缓存
	long long hashCode = getMapHashCode();
	if (containsCombo(hashCode))
		return getComboValue(hashCode);

	if (level == 0) {
		result.node++;
		return false;
	}
	//分析前两步周围的点
	points basePoints;
	if (father == nullptr)
		basePoints = getNeighbor();
	else {
		if (grandFather == nullptr)
			basePoints = getPointLinesNeighbor(*father);
		else {
			points fatherPoints = getPointLinesNeighbor(*father);
			points grandPoints = getPointLinesNeighbor(*grandFather);
			pointHash hash;
			for (int i = 0; i < fatherPoints.count; i++) {
				point p = fatherPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(fatherPoints.list[i]);
				}
			}
			for (size_t i = 0; i < grandPoints.count; i++) {
				point p = grandPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(grandPoints.list[i]);
				}
			}
		}
	}
	analyzeData data = getAnalyzeData(color, basePoints, true);
	//检查对面有没有被迫形成潜在的三四以及三四，如果有则转换为冲四
	if (comboType == THREE_COMBO)
		if (targetColor == color) {
			if (data.doubleWeakFourDefence.count > 0)
				comboType = FOUR_COMBO;
			if (data.weakThreeAndFourDefence.count > 0) 
				comboType = FOUR_COMBO;
		}
	//如果对面形成活三，则转换为冲四
	if (comboType == THREE_COMBO) {
		if (color == targetColor && data.threeDenfence.count > 0) {
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
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p, NULL, color, targetColor);
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
		//如果没有能防的则结束
		if (ps.count == 0) {
			recordCombo(false);
			return false;
		}
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p, NULL, color, targetColor);
			if (!value) {
				recordCombo(false);
				return false;
			}
		}
		recordCombo(true);
		return true;
	}
}

comboResult canKill(Color targetColor, int level, long long startTimeValue, long long limitTimeValue)
{
	if (level % 2 == 0) {
		level++;
	}
	startTime = startTimeValue;
	limitTime = limitTimeValue;
	currentLevel = level;
	//计算我方四连杀
	result.reset();
	cacheReset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = true;
		return result;
	}
	//计算对手四连杀
	result.reset();
	cacheReset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.win = false;
		return result;
	}

	//计算我方三连杀
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
