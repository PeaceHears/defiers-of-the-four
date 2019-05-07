//-----------------------------------------------------------------
// DotF Application
// C++ Header - DotF.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <algorithm>
#include <queue> 
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Character.h"
#include "Robot.h"
#include "Button.h"
#include "Demon.h"

//-----------------------------------------------------------------
// Custom types
//-----------------------------------------------------------------
typedef vector< vector<int>> map;

//-----------------------------------------------------------------
// Resolution
//-----------------------------------------------------------------
const int RES_W = 1024;
const int RES_H = 768;

//-----------------------------------------------------------------
// Scene Flags
//-----------------------------------------------------------------
enum SCENE {
	MENU_MAIN = 0,
	MENU_SELECT_PLAYERS = 1,
	MENU_SELECT_ROBOTS = 2,
	GAME_PLAY = 3,
	GAME_PAUSE = 4
};

Bitmap*     _pWallBitmap;
Sprite*     _pWalls[100];
Bitmap*     _pEnemyBaseBitmap;

map CreateMap(bool _fLevel);
queue <POINT> pathFinding(POINT targetposition, POINT startingposition, map currentmap);
int number_of_enemyCenter;
Sprite*     _pEnemyBase[20];
Sprite*     _robot[2];
Bitmap* _robotbmp;
map maps[8];
POINT bulletRotation;
Bitmap *bmSheep;
int bulletCount = 0;
Bitmap *bmTile1;
RECT path;
queue<POINT> pathQ;

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE   hInstance;
GameEngine* game;
HDC         offScreenDC;
HBITMAP     offScreenBitmap;
SCENE		currentScene;

//-----------------------------------------------------------------
// Characters
//-----------------------------------------------------------------
vector<Robot*>	robots;
vector<int>	selectedRobots;
int			playerNum; // single player or multiplayer

//-----------------------------------------------------------------
// Menu Bitmaps / Sprites
//-----------------------------------------------------------------
Bitmap  	*bmMenuBackground;
Bitmap  	*bmMenuTitle;
Bitmap  	*bmMenuStar;

//-----------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------
vector<Button*>	menuMainButtons, menuPlayersButtons, menuRobotsButtons;
Button			*btnPlay, *btnExit;
Button			*btn1Player, *btn2Player, *btnBackToMain;
Button			*btnReady, *btnBackToPlayers;


//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Menu functions
//-----------------------------------------------------------------
void	HandleMenuButtonClick(int _x, int _y);
void	HandleMenuButtonHover(int _x, int _y);
void	DrawRobotsOnMenu(HDC _hDC);

//-----------------------------------------------------------------
// Utility
//-----------------------------------------------------------------
LPWSTR ConvertString(const std::string& instr);
