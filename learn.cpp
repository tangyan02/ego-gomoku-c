#include "stdafx.h"

#define PLAY_DRAW 0
#define PLAY_WIN 1
#define PLAY_NOT_FINISH 2

#include "learn.h"
#include "console.h"
#include "gameMap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "levelProcessor.h"
#include "pisqpipe.h"
#include "pattern.h"
#include "patternRecorder.h"
#include "comboProcessor.h"
#include "winChecker.h"
#include <ctime> 

extern bool debugEnable;

extern int boardSize;

extern int searchLevel;

extern int timeOut;

extern int comboTimeOut;

static int groupPlayCount;

static int groupCurrentPlayCount;

extern int comboLevel;

extern bool comboEnable;

static int openings;


static int oneTurn(player player, Color** map) {
	setBaseScore(player.score, player.sigma);
	gameResult result = search(player.color, map);
	point p = result.result;
	map[p.x][p.y] = player.color;
	//system("cls");
	//printMap(map);
	if (win(map)) {
		return PLAY_WIN;
	}
	return PLAY_NOT_FINISH;
}

static void recordPlayers(vector<player>& players,int count, int maxVersion) {
	FILE* fp;
	fp = fopen("./players.txt", "w");

	fprintf(fp, "version:%d\ngameCounts:%d\nlastPlayers:%d\ntimeOut:%d\nopeningNum:%d\nsearchLevel:%d\n", maxVersion, count, players.size(), timeOut, openings, searchLevel);

	for (int i = 0; i < players.size(); i++) {
		fprintf(fp, "%d %d\n", players[i].version, players[i].sigma);
		for (int j = 0; j < 10; j++)
			fprintf(fp, "%d ", players[i].score[j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static int selfPlay(player p1, player p2, Color** map)
{
	printf("%d/%d player1:%d %c  vs  player2:%d %c ", groupCurrentPlayCount, groupPlayCount, p1.version, getCharOfColor(p1.color), p2.version, getCharOfColor(p2.color));

	long long startTime = getSystemTime();

	bool p1Trun = true;
	int finish;
	int turnCount = 0;
	while (true)
	{
		if (p1Trun) {
			finish = oneTurn(p1, map);
		} else {
			finish = oneTurn(p2, map);
		}

		//printf("player1:%d  vs  player2:%d   %d/%d \n", p1.version, p2.version, groupCurrentPlayCount, groupPlayCount);

		turnCount++;
		if (turnCount > 80) {
			finish = PLAY_DRAW;
		}

		if (finish == PLAY_WIN || finish == PLAY_DRAW) {
			break;
		}
		p1Trun = !p1Trun;
	}

	groupCurrentPlayCount++;
	long long cost = getSystemTime() - startTime;
	if (finish != PLAY_NOT_FINISH && finish != PLAY_DRAW)
	{
		if (p1Trun) {
			printf("winner: %d turns:%d cost:%ldms\n", p1.version, turnCount, cost);
			return p1.id;
		}
		else {
			printf("winner: %d turns:%d cost:%ldms\n", p2.version, turnCount, cost);
			return p2.id;
		}
	}
	printf("draw game turns:%d cost:%ldms\n", turnCount, cost);
	return -1;
}

static player copy(player p, int& maxVersion) {
	player result;
	result.sigma = p.sigma;
	maxVersion++;
	for (int i = 0; i < 10; i++) {
		result.score[i] = p.score[i];
		result.version = maxVersion;
	}
	return result;
}

static void variation(player& p) {
	for (int i = 3; i < 10; i++) {
		int bond = max(p.score[i] / 4, 5);
		int delta = rand() % bond + 1;
		if (rand() % 2 == 0) {
			delta = -delta;
		}
		p.score[i] += delta;
	}
	p.sigma = 0;
	if (rand() % 2 == 0) {
		int delta = 1;
		if (rand() % 2 == 0) {
			delta = -delta;
		}
		if (p.sigma + delta >= 0) {
			p.sigma += delta;
		}
	}
	
}

static void accident(player& p) {
	for (int i = 3; i < 10; i++) {
		int bond = p.score[i] * 2 + 1;
		if (rand() % 2 == 0) {
			int newScore = rand() % bond + 1;
			p.score[i] = newScore;
		}
	}

	p.sigma = 0;
	if (rand() % 2 == 0) {
		int sigma = rand() % (p.sigma*2+1) + 1;
		p.sigma = sigma;
	}
}

static void breed(vector<player>& players, int& maxVersion) {
	int size = players.size();
	for (int i = 0; i < size; i++) {
		player child = copy(players[i], maxVersion);
		variation(child);
		players.push_back(child);
	}

	for (int i = 0; i < size/4; i++) {
		player child = copy(players[i], maxVersion);
		accident(child);
		players.push_back(child);
	}
}

static void winCountIncrease(player&p1 ,player&p2, int winId, int winCount[]){
	if (winId != -1) {
		winCount[winId]+=2;
	} else {
		winCount[p1.id]++;
		winCount[p2.id]++;
	}
}

static vector<player> groupPlay(vector<player> &players, int n, int openings) {
	int winCount[1000] = {0};
	for (int i = 0; i < players.size(); i++) {
		players[i].id = i;
		winCount[i] = 0;
	}
	Color ** map = getEmptyMap();
	Color ** mapTemp = getEmptyMap();

	for (int k = 1; k <= openings; k++){
		map = readMapFromOpennings(k, map);

		for (int i = 0; i < players.size(); i++)
			for (int j = i; j < players.size(); j++) {
				if (i != j) {
					copyMap(map, mapTemp);
					Color nextColor = nextColorForOpennings(mapTemp);
					players[i].color = BLACK;
					players[j].color = WHITE;
					if (players[i].color == nextColor) {
						int winId = selfPlay(players[i], players[j], mapTemp);
						winCountIncrease(players[i], players[j],winId, winCount);
					}
					else {
						int winId = selfPlay(players[j], players[i], mapTemp);
						winCountIncrease(players[i], players[j], winId, winCount);
					}

					copyMap(map, mapTemp);
					players[i].color = WHITE;
					players[j].color = BLACK;
					if (players[i].color == nextColor) {
						int winId = selfPlay(players[i], players[j], mapTemp);
						winCountIncrease(players[i], players[j], winId, winCount);
					}
					else {
						int winId = selfPlay(players[j], players[i], mapTemp);
						winCountIncrease(players[i], players[j], winId, winCount);
					}
				}
			}
	}

	//sort
	for (int i = 0; i < players.size(); i++)
		for (int j = i + 1; j < players.size(); j++)
			if (winCount[i] < winCount[j]) {
				player t = players[i];
				players[i] = players[j];
				players[j] = t;
				int temp = winCount[i];
				winCount[i] = winCount[j];
				winCount[j] = temp;
			}

	printGroupResult(winCount, players);

	vector<player> result;
	for (int i = 0; i < n; i++) {
		result.push_back(players[i]);
	}

	return result;
}


void selfLearn() {
	setbuf(stdout, NULL); 
	while (true) {
		unsigned seed;  // Random generator seed
		seed = time(0);
		srand(seed);

		FILE* fp;
		fp = fopen("players.txt", "r");

		if(fp == NULL) {
			printf("file players.txt open error");
			return;
		}

		vector<player> players;
		int n, count, version, fileTimeOut, openingNum, fileSearchLevel;
		fscanf (fp, "version:%d gameCounts:%d lastPlayers:%d timeOut:%d openingNum:%d searchLevel:%d", &version, &count, &n, &fileTimeOut, &openingNum, &fileSearchLevel);
		
		boardSize = 20;
		debugEnable = false;
		comboEnable = false;

		searchLevel = fileSearchLevel;
		timeOut = fileTimeOut;
		openings = openingNum;
		comboTimeOut = 0;
		comboLevel = 0;

		for (int i = 0; i < n; i++) {
			player p;

			fscanf (fp, "%d %d", &p.version, &p.sigma);
			for (int j = 0; j < 10; j++) {
				fscanf (fp, "%d", &p.score[j]);
			}
			players.push_back(p);
		}
		fclose(fp);

		breed(players, version);

		printf ("version:%d gamesCount:%d timeOut:%d\n", version, count, fileTimeOut);
		printPlayers(players);

		printf ("start...\n");

		groupPlayCount = players.size() * (players.size() - 1) * openings;
		groupCurrentPlayCount = 0;
		vector<player> lastPlaters = groupPlay(players, n, openings);
		count += groupPlayCount;
		recordPlayers(lastPlaters, count, version);
		printf("submit result finish\n");
	}
}

//////////////////////////////////////////////

static int baseScore[10] = { 0, 100000, 10000, 40, 50, 20, 25, 10, 12, 5 };

void testSelfPlay()
{
	player p1;
	p1.id = 0;
	p1.color = BLACK;

	player p2;
	p2.id = 1;
	p2.color = WHITE;


	for (int i = 0; i < 10; i++) {
		p1.score[i] = baseScore[i];
		p2.score[i] = baseScore[i];
	}

	p1.score[4] = 60;
	p2.score[4] = 40;

	//selfPlay(p1, p2);
}


void testGroupPlay() {
	boardSize = 20;
	player p1;
	player p2;
	player p3;

	for (int i = 0; i < 10; i++) {
		p1.score[i] = baseScore[i];
		p2.score[i] = baseScore[i];
	}

	p1.score[4] = 60;
	p1.sigma = 1;
	p1.version = 10;
	p2.score[4] = 40;
	p2.sigma = 2;
	p1.version = 12;
	p3.score[6] = 15;
	p3.sigma = 3;
	p1.version = 15;

	player players[3];
	players[0] = p1;
	players[1] = p2;
	players[2] = p3;

	vector<player> list;
	list.push_back(p1);
	list.push_back(p2);
	list.push_back(p3);
	groupPlay(list, 3, 1);
}