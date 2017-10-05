#pragma once

void initScore(Color aiColor);

void updateScore(point point, Color color, Color forwardColor, Color aiColor);

int getValueByCount(int blackCount, int whiteCount, Color color);

int getScoreValue();