//-----------------------------------------------------------------
// Bitmap Object
// C++ Header - Robot.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include "Character.h"

//-----------------------------------------------------------------
// Custom Data Types
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Robot Class
//-----------------------------------------------------------------
class WololoRobot :public Character
{
protected:

	// Member Variables
	ControlStatus controlStatus;
	int cooldown1, cooldown2;
	bool menuHover;
	//vector<Demon*> currentTargets;
	// Helper Methods
	void Free();

public:
	// Constructor(s)/Destructor
	WololoRobot(std::string _name, std::string _description, Sprite* _sprite, Sprite* _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed, ControlStatus _controlStatus, int _cooldown1, int _cooldown2, bool _menuHover = false);
	virtual ~WololoRobot();

	// General Methods
	void Update();

	// Accessor Methods
	void SetMenuHover(bool _hover) { menuHover = _hover; };
	bool GetMenuHover() { return menuHover; };
	void SetControlStatus(ControlStatus _status) { controlStatus = _status; };
	ControlStatus GetControlStatus() { return controlStatus; };
	void utility1(vector<Character*> robots) { // heal robot that has min heal point
		
		for (auto &Robot : robots) {
			
		}
	}
	void utility2(vector<Character*> robots ) { // heal all robots
		for (auto &Robot : robots) {
			Robot->Heal(10);
		}
	}
};