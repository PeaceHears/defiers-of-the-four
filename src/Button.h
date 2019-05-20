//-----------------------------------------------------------------
// Button Object
// C++ Header - Button.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string.h>
#include "Sprite.h"

//-----------------------------------------------------------------
// Consts
//-----------------------------------------------------------------
const COLORREF	CR_BG = RGB(22, 15, 28);
const COLORREF	CR_TXT = RGB(255, 255, 255);
const COLORREF	CR_TXTBG = RGB(22, 15, 28);
const COLORREF	CR_BGHOVER = RGB(255, 255, 255);
const COLORREF	CR_TXTHOVER = RGB(22, 15, 28);
const COLORREF  CR_TXTBGHOVER = RGB(255, 255, 255);
const int		BTN_WIDTH = 250;
const int		BTN_HEIGHT = 50;

//-----------------------------------------------------------------
// Button Class
//-----------------------------------------------------------------
class Button {
protected:
	// Member Variables
	Bitmap* bgBitmapDefault;
	Bitmap* bgBitmapHover;
	Sprite* btnSprite;
	bool hover;
	LPTSTR text;
	int	x, y, width, height;

public:
	// Constructor(s)/Destructor
	Button(HDC _hDC, LPTSTR _text, int _x, int _y);
	~Button();

	// General methods
	void Draw(HDC _hdc);

	// Accessor methods
	void SetHover(bool _h) { hover = _h; };
	Sprite* GetSprite() { return btnSprite; };
};
