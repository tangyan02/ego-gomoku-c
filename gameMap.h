#pragma once

#define NULL_COLOR 0
#define CURRENT_COLOR 1
#define OTHER_COLOR 2
#define INVALID_COLOR 3

void setColor(int x, int y, Color color);

Color getColor(point p);

int getMapLineKey(int x, int y, int direct, Color targetColor);

int getPointTableColor(int x, int y, Color targetColor);

points getNeighbor();

points getPointLinesNeighbor(point p);

bool reachable(int x, int y);

void initGameMap(Color** value);

Color ** getMap();

long long getMapHashCode();

void updateHashCode(int x, int y, Color color);