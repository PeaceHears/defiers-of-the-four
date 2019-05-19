//-----------------------------------------------------------------
// R_Captain Object
// C++ Header - R_Captain.h
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
// Custom Data Types
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// R_Captain Class
//-----------------------------------------------------------------
class R_Captain:public Robot
{
protected:

	// Member Variables


	// Helper Methods
	void Free();

public:
	// Constructor(s)/Destructor
	R_Captain(std::string _name, std::string _description, Sprite * _sprite, Sprite * _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed);
	virtual ~R_Captain();

	// General Methods
	void UseAbility1();
	void UseAbility2();
};