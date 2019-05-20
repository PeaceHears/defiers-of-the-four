//-----------------------------------------------------------------
// Bitmap Object
// C++ Header - DemonBase.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
using namespace std;
#include <windows.h>
#include <string>
#include <algorithm>
#include <vector>
#include "Sprite.h"
#include "Robot.h"

#ifndef DEMON_H
#define DEMON_H
#endif

class Demon; // reference

//-----------------------------------------------------------------
// DemonBase Class
//-----------------------------------------------------------------
class DemonBase
{
protected:
	// Member Variables
	Sprite* sprite;
	POINT mapPosition;
	int base_number;
	vector<Robot*> currentTargets;
	vector<Demon*> currentDemons;
	int spawnLimit = 3;

public:
	// Constructor(s)/Destructor
	DemonBase(POINT _mapPosition, int _base_number, Sprite* _demonBaseSprite);
	virtual ~DemonBase();

	// General Methods
	inline void		ReduceSpawnLimit(int _amount) { spawnLimit -= _amount; };
	inline void		AddDemonToVector(Demon* _demon) { currentDemons.push_back(_demon); };
	inline void		RemoveDemon(Demon* _demon) {
		currentDemons.erase(std::remove(currentDemons.begin(), currentDemons.end(), _demon), currentDemons.end());
	}

	// Accessor Methods
	int				GetBaseNumber() { return base_number; };
	void			SetBaseNumber(int __base_number) { base_number = __base_number; };
	vector<Robot*>	GetCurrentTargets() { return currentTargets; };
	void			SetCurrentTargets(vector<Robot*> _currentTargets) { currentTargets = _currentTargets; };
	vector<Demon*>	GetCurrentDemons() { return currentDemons; };
	void			SetCurrentDemons(vector<Demon*> _currentDemons) { currentDemons = _currentDemons; };
	POINT			GetMapPosition() { return mapPosition; };
	void			SetMapPosition(POINT __mapPosition) { mapPosition = __mapPosition; };
	Sprite*			GetSprite() { return sprite; };
	void			SetSprite(Sprite *_spr) { sprite = _spr; };
	int				GetSpawnLimit() { return spawnLimit; };
	void			SetSpawnLimit(int _sl) { spawnLimit = _sl; };
};