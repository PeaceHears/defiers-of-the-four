//-----------------------------------------------------------------
// Demon Boss Object
// C++ Header - DemonBoss1.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include <time.h>
#include "Demon.h"

//-----------------------------------------------------------------
// R_Captain Class
//-----------------------------------------------------------------
class DemonBoss1 :public Demon
{
protected:
public:
	// Constructor(s)/Destructor
	DemonBoss1(Sprite * _sprite, POINT _mapPosition);
	virtual ~DemonBoss1();
};
