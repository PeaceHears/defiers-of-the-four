//-----------------------------------------------------------------
// Game Engine Object
// C++ Header - GameEngine.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
using namespace std;
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include "Sprite.h"

//-----------------------------------------------------------------
// Joystick Flags
//-----------------------------------------------------------------
typedef WORD    JOYSTATE;
const JOYSTATE  JOY_NONE = 0x0000L,
JOY_LEFT = 0x0001L,
JOY_RIGHT = 0x0002L,
JOY_UP = 0x0004L,
JOY_DOWN = 0x0008L,
JOY_FIRE1 = 0x0010L,
JOY_FIRE2 = 0x0020L;

//-----------------------------------------------------------------
// Windows Function Declarations
//-----------------------------------------------------------------
int WINAPI        WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------
// Game Engine Function Declarations
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE hInstance);
void GameStart(HWND hWindow);
void GameEnd();
void GameActivate(HWND hWindow);
void GameDeactivate(HWND hWindow);
void GamePaint(HDC hDC);
void GameCycle();
void HandleKeys();
void MouseButtonDown(int x, int y, BOOL bLeft);
void MouseButtonUp(int x, int y, BOOL bLeft);
void MouseMove(int x, int y);
void HandleJoystick(JOYSTATE jsJoystickState);
BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee);

//-----------------------------------------------------------------
// GameEngine Class
//-----------------------------------------------------------------
class GameEngine
{
protected:
	// Member Variables
	static GameEngine*  gameEngine;
	HINSTANCE           hInstance;
	HWND                hWindow;
	TCHAR               windowClass[32];
	TCHAR               title[32];
	WORD                icon, smallIcon;
	int                 width, height;
	int                 frameDelay;
	BOOL                sleep;
	UINT                joystickID;
	RECT                joystickTrip;
	vector<Sprite*>     sprites;
	UINT                midiPlayerID;

public:
	// Constructor(s)/Destructor
	GameEngine(HINSTANCE _hInstance, LPTSTR _windowClass , LPTSTR _title,
		WORD _icon, WORD _smallIcon, int _width = 640, int _height = 480);
	virtual ~GameEngine();

	// General Methods
	static GameEngine*  GetEngine() { return gameEngine; };
	BOOL                Initialize(int _cmdShow);
	LRESULT             HandleEvent(HWND _hWindow, UINT _msg, WPARAM _wParam,
		LPARAM lParam);
	void                ErrorQuit(LPTSTR _errorMsg);
	BOOL                InitJoystick();
	void                CaptureJoystick();
	void                ReleaseJoystick();
	void                CheckJoystick();
	void                AddSprite(Sprite* _sprite);
	void                DrawSprites(HDC _hDC);
	void                UpdateSprites();
	void                CleanupSprites();
	Sprite*             IsPointInSprite(int _x, int _y);
	void                PlayMIDISong(LPTSTR _midiFileName = (LPTSTR) TEXT(""), BOOL _restart = TRUE);
	void                PauseMIDISong();
	void                CloseMIDIPlayer();
	void				PrintText(HDC _hDC, LPCWSTR _text, int _fontSize, RECT _bounds);

	// Accessor Methods
	HINSTANCE GetInstance() { return hInstance; };
	HWND      GetWindow() { return hWindow; };
	void      SetWindow(HWND _hWindow) { hWindow = _hWindow; };
	LPTSTR    GetTitle() { return title; };
	WORD      GetIcon() { return icon; };
	WORD      GetSmallIcon() { return smallIcon; };
	int       GetWidth() { return width; };
	int       GetHeight() { return height; };
	int       GetFrameDelay() { return frameDelay; };
	void      SetFrameRate(int _frameRate) { frameDelay = 1000 / _frameRate; };
	BOOL      GetSleep() { return sleep; };
	void      SetSleep(BOOL _sleep) { sleep = _sleep; };
	// Helper Methods
	BOOL	  CheckSpriteCollision(Sprite* _testSprite);
	BOOL	  CheckMoveCollision(Sprite* _testSprite);


	vector<Sprite*> GetSprites() { return sprites; };
};
