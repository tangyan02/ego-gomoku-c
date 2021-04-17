#pragma once

void testPatternRecorder();

void updatePointPattern(int x, int y);

void addPointDirectPatternCount(int x, int y, int direct);

void updateLinePatternCount(int x, int y);

void removeLinePatternCount(int x, int y);

void printPatternAnalyze();

void clearPatternRecord();

void printPatternAnalyzeInMessage();

void updateLineKey(int x, int y);

void initPatternRecord();

int countPattern(Color color, int patternCode);
