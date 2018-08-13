#include "stdafx.h"
#include "pisqpipe.h"
#include <windows.h>
#include "game.h"
#include "io.h"

const char *infotext = "name=\"Ego\", author=\"TangYan\", version=\"7.0\", country=\"China\", email=\"tangyan1412@foxmail.com\"";

#define MAX_BOARD 20
static Color** map;
static unsigned seed;
extern int boardSize;
extern int timeOut;
extern int comboTimeOut;

void brain_init()
{
	if (width < 5 || height < 5) {
		pipeOut("ERROR size of the board");
		return;
	}
	if (width > MAX_BOARD || height > MAX_BOARD) {
		pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
		return;
	}
	boardSize = width;
	map = getEmptyMap();
	seed = start_time;
	initPattern();
	pipeOut("OK");
}

void brain_restart()
{
	map = getEmptyMap();
	pipeOut("OK");
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && map[x][y] == NULL;
}

void brain_my(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = BLACK;
	}
	else {
		pipeOut("ERROR my move [%d,%d]", x, y);
	}
}

void brain_opponents(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = WHITE;
	}
	else {
		pipeOut("ERROR opponents's move [%d,%d]", x, y);
	}
}

void brain_block(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = 3;
	}
	else {
		pipeOut("ERROR winning move [%d,%d]", x, y);
	}
}

int brain_takeback(int x, int y)
{
	if (x >= 0 && y >= 0 && x < width && y < height && map[x][y] != 0) {
		map[x][y] = 0;
		return 0;
	}
	return 2;
}

unsigned rnd(unsigned n)
{
	seed = seed * 367413989 + 174680251;
	return (unsigned)(UInt32x32To64(n, seed) >> 32);
}

void brain_turn()
{
	int thisTimeOut = info_time_left / 10;
	thisTimeOut = min(info_timeout_turn, thisTimeOut);
	int pointCount = 0;
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] != NULL) {
				pointCount++;
			}
		}
	}
	pipeOut("MESSAGE time limit %d", thisTimeOut);
	//timeOut = thisTimeOut;
	timeOut = thisTimeOut / 4 * 3;
	comboTimeOut = thisTimeOut - timeOut;
	gameResult result = search(BLACK, map);
	point p = result.result;
	if (result.value == MAX_VALUE) {
		pipeOut("MESSAGE ¡ï¡ï¡ï");
	}
	if (result.value == MIN_VALUE) {
		pipeOut("MESSAGE ¡ø¡ø¡ø");
	}
	pipeOut("MESSAGE ¡ð¡ñ level %d(%d) value %d (%d,%d) speed %d k (%d) ¡ñ¡ð", result.level, result.combo, result.value, p.x, p.y, result.speed, result.node);
	do_mymove(p.x, p.y);
}

void brain_end()
{
}

#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x, int y)
{
	HDC dc;
	HWND wnd;
	RECT rc;
	char c;
	wnd = GetForegroundWindow();
	dc = GetDC(wnd);
	GetClientRect(wnd, &rc);
	c = (char)(board[x][y] + '0');
	TextOut(dc, rc.right - 15, 3, &c, 1);
	ReleaseDC(wnd, dc);
}

#endif
