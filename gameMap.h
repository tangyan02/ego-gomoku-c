#pragma once

#define NULL_COLOR 0
#define CURRENT_COLOR 1
#define OTHER_COLOR 2
#define INVALID_COLOR 3

Color getColor(int x, int y);

void move(int x, int y, Color color);

void undoMove(int x, int y, Color color);

void fillNeighbor(points* ps);

void fillPointLinesNeighbor(int px, int py, points* ps);

bool reachable(int x, int y);

void initGameMap(Color** value);

Color ** getMap();

long long getMapHashCode();

void updateHashCode(int x, int y, Color color);

//bool inNeighbor(int x, int y);