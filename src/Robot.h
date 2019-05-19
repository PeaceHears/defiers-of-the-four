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
	R_WOLOLO
};

struct Ability {
	std::string name;
	int cooldown;
	int duration;
	bool ready;
	time_t usedTime;
};
typedef struct Ability Ability;

//-----------------------------------------------------------------
// Robot Class
//-----------------------------------------------------------------
class Robot:public Character
{
protected:
	
	// Member Variables
	ControlStatus controlStatus;
	bool menuHover;
	Ability	abilities[2];
	RobotType robotType;

	//vector<Demon*> currentTargets;

	// Helper Methods
	void Free();

public:
	// Constructor(s)/Destructor
	Robot(std::string _name, std::string _description, Sprite* _sprite, Sprite* _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed, ControlStatus _controlStatus, bool _menuHover=false);
	virtual ~Robot();

	// General Methods
	void Update();
	virtual void UseAbility1() {};
	virtual void UseAbility2() {};
	
	void UseSuperPower1(vector<Robot*> robots) { // heal robot that has min heal point
	/*if (superpower1avaliable)
	{


		Robot *minRobot = NULL;
		int minHealthPoint = 10000;
		for (auto &Robot : robots) {
			if (Robot->GetHealth() < minHealthPoint) {
				minHealthPoint = Robot->GetHealth();
				minRobot = Robot;
			}
		}

		minRobot->Heal(10);
		superpower1avaliable = false;
		time(&superpower1usagetime);
	}*/
	}
	void UseSuperPower2(vector<Robot*> robots) { // heal all robots
		/*if (superpower2avaliable) {
			for (auto &Robot : robots) {
				Robot->Heal(10);
			}
			superpower2avaliable = false;
			time(&superpower2usagetime);
		}*/
	}

	// Accessor Methods
	void SetMenuHover(bool _hover) { menuHover = _hover; };
	bool GetMenuHover() { return menuHover; };
	void SetControlStatus(ControlStatus _status) { controlStatus = _status; };
	ControlStatus GetControlStatus() { return controlStatus; };
	Ability *GetAbility(int _idx) { return &abilities[_idx]; };
	RobotType GetRobotType() { return robotType; };

	bool IsAbilityReady(int i) { return abilities[i].ready; };
	void SetAbilityReady(int i, bool r) { abilities[i].ready = r; };
	time_t GetAbilityUsedTime(int i) { return abilities[i].usedTime; };
	int GetAbilityDuration(int i) { return abilities[i].duration; };
	int GetAbilityCooldown(int i) { return abilities[i].cooldown; };
};