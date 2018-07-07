#pragma once

int getMapLineKey(int x, int y, int direct, Color targetColor);

int getPointTableColor(int x, int y, Color targetColor);

void updatePointKey(int px, int py);

void setLineKey(int px, int py, int direct, Color targetColor);