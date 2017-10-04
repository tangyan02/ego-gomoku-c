#pragma once

void setColor(int x, int y, Color color);

points getNeighbor();

bool reach(int x, int y);

void initGameMap(Color** value);

Color ** getMap();