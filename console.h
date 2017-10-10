#pragma once
#include"point.h"
#include"points.h"
#include"config.h"
#include"pointHash.h"

void printPoints(points ps);

void printHash(pointHash hash);

void printMap(Color **map);

void printMapWithStars(Color **map, points ps);

void printMapWithStar(Color **map, point p);