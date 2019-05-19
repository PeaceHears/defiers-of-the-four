//-----------------------------------------------------------------
// Robot Object
// C++ Source - Robot.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Robot.h"

//-----------------------------------------------------------------
// Robot Constructor(s)/Destructor
//-----------------------------------------------------------------
Robot::Robot(std::string _name, std::string _description, Sprite* _sprite, Sprite* _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed, ControlStatus _controlStatus, bool _menuHover)
	:Character(_name, _description, _sprite, _menuSprite, _healthPoint, _speed, _mapPosition, _fireSpeed) 
{
	controlStatus = _controlStatus;
	isRobot = true;
}

Robot::~Robot()
{
}


//-----------------------------------------------------------------
// Robot General Methods
//-----------------------------------------------------------------
void Robot::Update() {
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