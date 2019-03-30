// Defiers.cpp : Defines the entry point for the application.
//

#include "Defiers.h"

//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE hInstance)
{
  // Create the game engine
  _pGame = new GameEngine(hInstance, (LPTSTR) TEXT("Defiers of the Four"), 
	  (LPTSTR) TEXT("Defiers of the Four"), (WORD) IDI_GAMEICON, (WORD) IDI_GAMEICON, 600, 400);
  if (_pGame == NULL)
    return FALSE;
  
  // Set the frame rate
  _pGame->SetFrameRate(30);

  // Store the instance handle
  _hInstance = hInstance;

  return TRUE;
}

void GameStart(HWND hWindow)
{
  // Seed the random number generator
  srand(GetTickCount());

  // Create and load the bitmaps
  HDC hDC = GetDC(hWindow);

}

void GameEnd()
{
  // Cleanup the game engine
  delete _pGame;
}

void GameActivate(HWND hWindow)
{
}

void GameDeactivate(HWND hWindow)
{
}

void GamePaint(HDC hDC)
{

}

void GameCycle()
{

  // Force a repaint to redraw the golf balls
  InvalidateRect(_pGame->GetWindow(), NULL, FALSE);
}

void HandleKeys()
{
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
  
}

void MouseButtonUp(int x, int y, BOOL bLeft)
{

}

void MouseMove(int x, int y)
{
  
}

void HandleJoystick(JOYSTATE jsJoystickState)
{
}