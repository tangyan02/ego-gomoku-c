#pragma once

void setColor(int x, int y, Color color);

Color getColor(point p);

points getNeighbor();

bool reachable(int x, int y);

void initGameMap(Color** value);

Color ** getMap();
