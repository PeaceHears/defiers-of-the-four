//-----------------------------------------------------------------
// R_Constrobot Object
// C++ Source - R_Constrobot.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "R_Constrobot.h"

//-----------------------------------------------------------------
// R_Constrobot Constructor(s)/Destructor
//-----------------------------------------------------------------

R_Constrobot::R_Constrobot(string _name, string _description, Sprite * _sprite, Sprite * _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed)
	:Robot(_name, _description, _sprite, _menuSprite, _healthPoint, _speed, _mapPosition, _fireSpeed, CS_AI, false)
{
	name = { "Constrobot" };
	description = { "Builds a wall or demolish a wall at a stroke" };
	robotType = R_CONSTROBOT;
	stats.armor = 5;
	// Set up first ability
	time_t now;
	time(&now);

	// Ability 1
	// Build wall
	abilities[0].cooldown = 10;
	abilities[0].duration = 0;
	abilities[0].name = { "Build Wall" };
	abilities[0].ready = true;
	abilities[0].active = false;
	abilities[0].usedTime = now;

	// Ability 2
	// Demolish wall
	abilities[1].cooldown = 20;
	abilities[1].duration = 5;
	abilities[1].name = { "Unbreakable" };
	abilities[1].ready = true;
	abilities[1].active = false;
	abilities[1].usedTime = now;
}

R_Constrobot::~R_Constrobot()
{
}


//-----------------------------------------------------------------
// R_Constrobot General Methods
//-----------------------------------------------------------------


void R_Constrobot::UseAbility1() {
	PlaySound((LPCWSTR)IDW_CONSTROBOT_1, hInstance, SND_ASYNC | SND_RESOURCE);
	time(&abilities[0].usedTime);
	abilities[0].ready = false;
	time_t now;
	AddStatusMessage("*built!*", time(&now) + 2);
}

void R_Constrobot::UseAbility2() {

	time(&abilities[1].usedTime);
	abilities[1].active = true;
	abilities[1].ready = false;
	time_t now;
	AddStatusMessage("*destructive*", time(&now) + 3);
}
