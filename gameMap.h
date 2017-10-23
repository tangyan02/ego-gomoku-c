#pragma once

#define NULL_COLOR 0
#define CURRENT_COLOR 1
#define OTHER_COLOR 2
#define INVALID_COLOR 3

void setColor(int x, int y, Color color);

Color getColor(int x, int y);

points getNeighbor();

points getPointLinesNeighbor(int px, int py);

bool reachable(int x, int y);

void initGameMap(Color** value);

Color ** getMap();

long long getMapHashCode();

void updateHashCode(int x, int y, Color color);