#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"

extern int boardSize;

extern int nodeLimit;

extern bool moreAnalyze;

points getExpandPoints(analyzeData data, points neighbors)
{
	points result;
	//如果能连5，则连5
	if (data.fiveAttack.count > 0) {
		result.addMany(data.fiveAttack);
		return result;
	}
	//如果有对方冲4，则防冲4
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	//如果有对方活3，则防活3或者冲四
	if (data.threeDenfence.count > 0) {
		pointHash hash = pointHash();
		for (int i = 0; i < data.fourAttack.count; i++) {
			point p = data.fourAttack.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		for (int i = 0; i < data.threeDenfence.count; i++) {
			point p = data.threeDenfence.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		return result;
	}
	pointHash hash = pointHash();
	for (int i = 0; i < data.fourAttack.count; i++) {
		point p = data.fourAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < data.threeAttack.count; i++) {
		point p = data.threeAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < data.twoAttack.count; i++) {
		point p = data.twoAttack.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}
	for (int i = 0; i < neighbors.count; i++) {
		point p = neighbors.list[i];
		if (!hash.contains(p)) {
			hash.add(p);
			result.add(p);
		}
	}

	if (result.count > nodeLimit)
		result.count = nodeLimit;

	return result;
}
