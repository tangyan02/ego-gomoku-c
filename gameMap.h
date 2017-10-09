#pragma once

void setColor(int x, int y, Color color);

Color getColor(point p);

points getNeighbor();

points getPointLinesNeighbor(point p);

bool reachable(int x, int y);

void initGameMap(Color** value);

Color ** getMap();

long long getMapHashCode();

void updateHashCode(int x, int y, Color color);