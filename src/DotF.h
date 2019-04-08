//-----------------------------------------------------------------
// DotF Application
// C++ Header - DotF.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Character.h"
#include "R_RoboCap.h"

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE   hInstance;
GameEngine* game;
HDC         offScreenDC;
HBITMAP     offScreenBitmap;

//-----------------------------------------------------------------
// Game Variables
//-----------------------------------------------------------------
Character **characters;

//-----------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------
POINT NormalizeSpeed(int x, int y);