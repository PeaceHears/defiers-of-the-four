//-----------------------------------------------------------------
// Demon Base Object
// C++ Source - DemonBase.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DemonBase.h"
#include "Sprite.h"

//-----------------------------------------------------------------
// DemonBase Constructor(s)/Destructor
//-----------------------------------------------------------------
DemonBase::DemonBase(POINT _mapPosition, int _base_number, Sprite* _sprite)
{
	mapPosition = _mapPosition;
	base_number = _base_number;
	sprite = _sprite;

}

DemonBase::~DemonBase()
{
}
