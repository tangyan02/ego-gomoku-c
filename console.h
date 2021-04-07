#pragma once

#include"point.h"
#include"points.h"
#include"config.h"
#include"player.h"
#include"pointHash.h"
#include<string>

void printPoints(points ps);

void printHash(pointHash hash);

void printMapInMessage(Color ** map);

void printMap(Color **map);

void printMapWithStars(Color **map, points ps);

void printMapWithStars(Color **map, pointHash hash);

void printMapWithStar(Color **map, point p);

void printPlayers(vector<player>& players);

void printGroupResult(int winCount[], vector<player>& players);

string getCharOfColor(Color color);