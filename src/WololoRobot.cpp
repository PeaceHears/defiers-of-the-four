//-----------------------------------------------------------------
// Robot Object
// C++ Source - Robot.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "WololoRobot.h"

//-----------------------------------------------------------------
// Robot Constructor(s)/Destructor
//-----------------------------------------------------------------
WololoRobot::WololoRobot(std::string _name, std::string _description, Sprite* _sprite, Sprite* _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed, ControlStatus _controlStatus, int _cooldown1, int _cooldown2, bool _menuHover)
	:Character(_name, _description, _sprite, _menuSprite, _healthPoint, _speed, _mapPosition, _fireSpeed)
{
	name = _name;
	description = _description;
	sprite = _sprite;
	menuSprite = _menuSprite;
	curFireDelay = 0;

	controlStatus = _controlStatus;
	cooldown1 = _cooldown1;
	cooldown2 = _cooldown2;
	isRobot = true;

	CharacterStats pStats;
	pStats.fireDelay = 7;
	pStats.fireSpeed = _fireSpeed;
	pStats.health = pStats.maxHealth = _healthPoint;
	pStats.speed = _speed;
	stats = baseStats = pStats;
	fireDirection = { stats.fireSpeed, 0 };
}

WololoRobot::~WololoRobot()
{
}


//-----------------------------------------------------------------
// Robot General Methods
//-----------------------------------------------------------------
void WololoRobot::Update() {
	if (controlStatus != CS_AI) return;

	switch (task) {
	case AT_FOLLOW:
		if (target == NULL || path.empty()) return;
		Move();
	case AT_IDLE:
	default:
		return;
	}
}