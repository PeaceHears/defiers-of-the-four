//-----------------------------------------------------------------
// R_Wololo Object
// C++ Header - R_Wololo.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include <time.h>

#include "Robot.h"

//-----------------------------------------------------------------
// R_Wololo Class
//-----------------------------------------------------------------
class R_Wololo:public Robot
{
protected:
public:
	// Constructor(s)/Destructor
	R_Wololo(string _name, string _description, Sprite * _sprite, Sprite * _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed);
	virtual ~R_Wololo();

	// General Methods
	void UseAbility1();
	void UseAbility2(vector<Robot*> robots);
};