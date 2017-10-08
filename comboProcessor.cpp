#include "stdafx.h"
#include "comboProcessor.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include  "gameMap.h"

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
			result.addMany(data.fourAttack);
			return result;
		}
	}
	return result;
}

bool dfsKill(Color color, Color targetColor, int level, ComboType comboType) {

	if (level == 0) {
		return false;
	}

	//分析选取的点
	analyzeData data = getAnalyzeData(color, getNeighbor());

	//如果对面形成活三，则转换会冲四
	if (comboType == THREE_COMBO) {
		if (color == targetColor && data.threeDenfence.count > 0) {
			comboType = FOUR_COMBO;
		}
	}
	if (color == targetColor) {
		if (data.fiveAttack.count > 0) {
			if (level == currentLevel) {
				result.p = data.fiveAttack.list[0];
			}
			return true;
		}
		points ps = getComboAttackPoints(data, comboType);
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType);
			setColor(p, NULL, color, targetColor);
			if (level == currentLevel && value) {
				result.p = p;
				result.win = true;
			}
			if (value)
				return true;
		}
		return false;
	}
	else {
		if (data.fiveAttack.count > 0) {
			return false;
		}
		points ps = getComboDefencePoints(data, comboType);
		//如果没有能防的则结束
		if (ps.count == 0) {
			return false;
		}
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType);
			setColor(p, NULL, color, targetColor);
			if (!value) {
				return false;
			}
		}
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
	dfsKill(targetColor, targetColor,
		level,FOUR_COMBO);
	if (result.win) {
		result.fourWin = true;
		return result;
	}

	//计算对手四连杀
	result.reset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO);
	if (result.win) {
		result.win = false;
		return result;
	}

	//计算我方三连杀
	result.reset();
	dfsKill(targetColor, targetColor,
		level, THREE_COMBO);
	if (result.win) {
		result.fourWin = false;
		return result;
	}
	return result;
}
