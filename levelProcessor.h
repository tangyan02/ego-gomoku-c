#pragma once
#include "analyzeData.h"
#include "points.h"

void sortPoints(points *neighbors, Color color);

points getExpandPoints(analyzeData data, points neighbors);