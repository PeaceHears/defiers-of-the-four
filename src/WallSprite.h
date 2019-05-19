//-----------------------------------------------------------------
// Wall Sprite Object
// C++ Header - WallSprite.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Sprite.h"

//-----------------------------------------------------------------
// WallSprite Class
//-----------------------------------------------------------------
class WallSprite : public Sprite
{
protected:
	int health;
	int maxHealth;
public:
	// Constructor(s)/Destructor
	WallSprite(Bitmap* _bitmap);
	virtual ~WallSprite();

	// General methods
	void TakeHit(int _damage) { health -= _damage; };

	// Accessors
	int	GetHealth() { return health; }
	void SetHealth(int _health) { health = _health; };
	int GetPercHealth() { return float((float(health) / float(maxHealth))) * 100; };
};
