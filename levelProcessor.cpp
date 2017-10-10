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
	if (moreAnalyze) {
		//如果对手有双四进攻点
		if (data.doubleWeakFourDefence.count > 0) {
			point deadPoint = data.doubleWeakFourDefence.list[0];
			points linePoints = getPointLinesNeighbor(deadPoint);
			pointHash linePointsHash;
			linePointsHash.addMany(linePoints);

			PointSet result;
			result.add(deadPoint);
			result.addMany(data.fourAttack);
			for (int i = 0; i < data.weakFourDefence.count; i++)
			{
				point p = data.weakFourDefence.list[i];
				if (linePointsHash.contains(p)) {
					result.add(p);
				}
			}
			return result.ps;
		}
		//如果对手有三四进攻点
		if (data.weakThreeAndFourDefence.count > 0) {
			point deadPoint = data.weakThreeAndFourDefence.list[0];
			points linePoints = getPointLinesNeighbor(deadPoint);
			pointHash linePointsHash;
			linePointsHash.addMany(linePoints);

			PointSet result;
			result.add(deadPoint);
			result.addMany(data.fourAttack);

			for (int i = 0; i < data.weakThreeDefence.count; i++)
			{
				point p = data.weakThreeDefence.list[i];
				if (linePointsHash.contains(p)) {
					result.add(p);
				}
			}
			for (int i = 0; i < data.weakFourDefence.count; i++)
			{
				point p = data.weakFourDefence.list[i];
				if (linePointsHash.contains(p)) {
					result.add(p);
				}
			}
			return result.ps;
		}
		//如果对手有双三进攻点
		if (data.doubleWeakThreeDefence.count > 0) {
			point deadPoint = data.doubleWeakThreeDefence.list[0];
			points linePoints = getPointLinesNeighbor(deadPoint);
			pointHash linePointsHash;
			linePointsHash.addMany(linePoints);

			PointSet result;
			result.add(deadPoint);
			result.addMany(data.fourAttack);

			for (int i = 0; i < data.weakThreeDefence.count; i++)
			{
				point p = data.weakThreeDefence.list[i];
				if (linePointsHash.contains(p)) {
					result.add(p);
				}
			}
			return result.ps;
		}
	}

	pointHash hash = pointHash();
	result.addMany(data.fourAttack);
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
