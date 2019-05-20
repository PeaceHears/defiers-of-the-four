//-----------------------------------------------------------------
// Robot Object
// C++ Header - Robot.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include <time.h>
#include "Character.h"

//-----------------------------------------------------------------
// Custom Data Types
//-----------------------------------------------------------------
enum RobotType {
	R_CAPTAIN,
	R_WOLOLO,
	R_CONSTROBOT
};

struct Ability {
	string name;
	int cooldown; // seconds to charge
	int duration; // second active status will take
	bool active; // currently active
	bool ready; // ready to use
	time_t usedTime;
};
typedef struct Ability Ability;

//-----------------------------------------------------------------
// Robot Class
//-----------------------------------------------------------------
class Robot :public Character
{
protected:
	// Member Variables
	ControlStatus controlStatus;
	bool menuHover;
	Ability	abilities[2];
	RobotType robotType;

public:
	// Constructor(s)/Destructor
	Robot(string _name, string _description, Sprite* _sprite, Sprite* _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed, ControlStatus _controlStatus, bool _menuHover = false);
	virtual ~Robot();

	// Virtual
	virtual void UseAbility1() {};
	virtual void UseAbility2() {};
	virtual void UseAbility2(vector<Robot*> _robots) {};

	// General Methods
	void Update();

	// Accessor Methods
	void SetMenuHover(bool _hover) { menuHover = _hover; };
	bool IsMenuHover() { return menuHover; };
	void SetControlStatus(ControlStatus _status) { controlStatus = _status; };
	ControlStatus GetControlStatus() { return controlStatus; };
	Ability *GetAbility(int _idx) { return &abilities[_idx]; };
	RobotType GetRobotType() { return robotType; };
	bool IsAbilityReady(int i) { return abilities[i].ready; };
	void SetAbilityReady(int i, bool r) { abilities[i].ready = r; };
	time_t GetAbilityUsedTime(int i) { return abilities[i].usedTime; };
	int GetAbilityDuration(int i) { return abilities[i].duration; };
	int GetAbilityCooldown(int i) { return abilities[i].cooldown; };
	bool IsAbilityActive(int i) { return abilities[i].active; };
	void SetAbilityActive(int i, bool a) { abilities[i].active = a; };
};
