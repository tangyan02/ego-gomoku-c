#include "console.h"
#include "game.h"
#include "gameMap.h"
#include "io.h"
#include "levelProcessor.h"
#include "score.h"
#include "stdafx.h"
// #include "pisqpipe.h"
#include "comboProcessor.h"
#include "pattern.h"
#include "patternRecorder.h"

extern bool debugEnable;

extern int boardSize;

extern int searchLevel;

extern int comboTimeOut;

extern int timeOut;

void testPlay()
{
    boardSize = 20;
    debugEnable = true;
    Color **map = readMap();
    gameResult result = search(WHITE, map);
    printMapWithStar(map, result.result);
}

void removeStar(Color **map)
{
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (map[i][j] == STAR)
            {
                map[i][j] = NULL;
            }
        }
    }
}

void bugTest()
{
    boardSize = 20;
    debugEnable = true;
    //searchLevel = 2;
    Color **map = readMap("input008.txt");
    gameResult result = search(WHITE, map);
    printMapWithStar(map, result.result);
}

void caseTest()
{
    boardSize = 15;
    debugEnable = true;
    comboTimeOut = 3000;
    Color **map = readMap("input008.txt");
    gameResult result = search(BLACK, map);
    printMapWithStar(map, result.result);
}

void oneTurnSearch()
{
    boardSize = 15;
    debugEnable = false;
    comboTimeOut = 1000;
    timeOut = 4000;
    Color **map = readMap("input.txt");
    gameResult result = search(BLACK, map);
    FILE *fp = fopen("output.txt", "w");
    fprintf(fp, "%d %d\n", result.result.x, result.result.y);
    fclose(fp);
}

int main()
{
    // piskvork();
    //testCombo();
    //bugTest();
    oneTurnSearch();
    //testPlay();
    //testPattern();
    //testPatternRecorder();
    // getchar();
    return 0;
}
