#pragma once

void initScore(Color aiColor);

void updateScore(int px, int py, Color color, Color forwardColor, Color aiColor);

int getValueByCount(int blackCount, int whiteCount, Color color);

int getScoreValue();

int getColorCount(Color color, int x, int y, int direct);