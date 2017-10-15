#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"

extern int boardSize;

extern int nodeLimit;

extern bool moreAnalyze;

points getExpandPoints(analyzeData data, points neighbors)
{
	points result;
	//�������5������5
	if (data.fiveAttack.count > 0) {
		result.addMany(data.fiveAttack);
		return result;
	}
	//����жԷ���4�������4
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	//����жԷ���3�������3���߳���
	if (data.threeDefence.count > 0) {
		pointHash hash = pointHash();
		for (int i = 0; i < data.fourAttack.count; i++) {
			point p = data.fourAttack.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		for (int i = 0; i < data.threeDefence.count; i++) {
			point p = data.threeDefence.list[i];
			if (!hash.contains(p)) {
				hash.add(p);
				result.add(p);
			}
		}
		return result;
	}
	if (moreAnalyze) {
		//���������˫�Ľ�����
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
		//������������Ľ�����
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
		//���������˫��������
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
