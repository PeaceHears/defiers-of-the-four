//-----------------------------------------------------------------
// Sprite Object
// C++ Source - Sprite.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Sprite.h"

//-----------------------------------------------------------------
// Sprite Constructor(s)/Destructor
//-----------------------------------------------------------------
Sprite::Sprite(Bitmap* _bitmap) {
	// Initialize the member variables
	bitmap = _bitmap;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	SetRect(&position, 0, 0, _bitmap->GetWidth(), _bitmap->GetHeight());
	CalcCollisionRect();
	velocity.x = velocity.y = 0;
	zOrder = 0;
	SetRect(&bounds, 0, 0, 1024, 768);
	boundsAction = BA_STOP;
	isHidden = FALSE;
	isDying = FALSE;
	isOneCycle = FALSE;
}

Sprite::Sprite(Bitmap* _bitmap, RECT& _bounds, BOUNDSACTION _boundsAction) {
	// Calculate a random position
	int iXPos = rand() % (_bounds.right - _bounds.left);
	int iYPos = rand() % (_bounds.bottom - _bounds.top);

	// Initialize the member variables
	bitmap = _bitmap;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	SetRect(&position, iXPos, iYPos, iXPos + _bitmap->GetWidth(),
		iYPos + _bitmap->GetHeight());
	CalcCollisionRect();
	velocity.x = velocity.y = 0;
	zOrder = 0;
	CopyRect(&bounds, &_bounds);
	boundsAction = _boundsAction;
	isHidden = FALSE;
	isDying = FALSE;
	isOneCycle = FALSE;
}

Sprite::Sprite(Bitmap* _bitmap, POINT _position, POINT _velocity, int _zOrder,
	RECT& _bounds, BOUNDSACTION _boundsAction, SPRITETYPE _spriteType) {
	// Initialize the member variables
	bitmap = _bitmap;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	SetRect(&position, _position.x, _position.y, _bitmap->GetWidth(),
		_bitmap->GetHeight());
	CalcCollisionRect();
	velocity = _position;
	zOrder = _zOrder;
	spriteType = _spriteType;
	CopyRect(&bounds, &_bounds);
	boundsAction = _boundsAction;
	isHidden = FALSE;
	isDying = FALSE;
	isOneCycle = FALSE;
}

Sprite::~Sprite() {
}

//-----------------------------------------------------------------
// Sprite General Methods
//-----------------------------------------------------------------
SPRITEACTION Sprite::Update() {
	// See if the sprite needs to be killed
	if (isDying)
		return SA_KILL;

	// Update the frame
	UpdateFrame();

	// Update the position
	POINT ptNewPosition, ptSpriteSize, ptBoundsSize;
	ptNewPosition.x = position.left + velocity.x;
	ptNewPosition.y = position.top + velocity.y;
	ptSpriteSize.x = position.right - position.left;
	ptSpriteSize.y = position.bottom - position.top;
	ptBoundsSize.x = bounds.right - bounds.left;
	ptBoundsSize.y = bounds.bottom - bounds.top;

	// Check the bounds
	// Wrap?
	if (boundsAction == BA_WRAP)
	{
		if ((ptNewPosition.x + ptSpriteSize.x) < bounds.left)
			ptNewPosition.x = bounds.right;
		else if (ptNewPosition.x > bounds.right)
			ptNewPosition.x = bounds.left - ptSpriteSize.x;
		if ((ptNewPosition.y + ptSpriteSize.y) < bounds.top)
			ptNewPosition.y = bounds.bottom;
		else if (ptNewPosition.y > bounds.bottom)
			ptNewPosition.y = bounds.top - ptSpriteSize.y;
	}
	// Bounce?
	else if (boundsAction == BA_BOUNCE)
	{
		BOOL bBounce = FALSE;
		POINT ptNewVelocity = velocity;
		if (ptNewPosition.x < bounds.left)
		{
			bBounce = TRUE;
			ptNewPosition.x = bounds.left;
			ptNewVelocity.x = -ptNewVelocity.x;
		}
		else if ((ptNewPosition.x + ptSpriteSize.x) > bounds.right)
		{
			bBounce = TRUE;
			ptNewPosition.x = bounds.right - ptSpriteSize.x;
			ptNewVelocity.x = -ptNewVelocity.x;
		}
		if (ptNewPosition.y < bounds.top)
		{
			bBounce = TRUE;
			ptNewPosition.y = bounds.top;
			ptNewVelocity.y = -ptNewVelocity.y;
		}
		else if ((ptNewPosition.y + ptSpriteSize.y) > bounds.bottom)
		{
			bBounce = TRUE;
			ptNewPosition.y = bounds.bottom - ptSpriteSize.y;
			ptNewVelocity.y = -ptNewVelocity.y;
		}
		if (bBounce)
			SetVelocity(ptNewVelocity);
	}
	// Die?
	else if (boundsAction == BA_DIE)
	{
		if ((ptNewPosition.x + ptSpriteSize.x) < bounds.left ||
			ptNewPosition.x > bounds.right ||
			(ptNewPosition.y + ptSpriteSize.y) < bounds.top ||
			ptNewPosition.y > bounds.bottom)
			return SA_KILL;
	}
	// Stop (default)
	else
	{
		if (ptNewPosition.x  < bounds.left ||
			ptNewPosition.x >(bounds.right - ptSpriteSize.x))
		{
			ptNewPosition.x = max(bounds.left, min(ptNewPosition.x,
				bounds.right - ptSpriteSize.x));
			SetVelocity(0, 0);
		}
		if (ptNewPosition.y  < bounds.top ||
			ptNewPosition.y >(bounds.bottom - ptSpriteSize.y))
		{
			ptNewPosition.y = max(bounds.top, min(ptNewPosition.y,
				bounds.bottom - ptSpriteSize.y));
			SetVelocity(0, 0);
		}
	}
	SetPosition(ptNewPosition);

	return SA_NONE;
}

Sprite* Sprite::AddSprite(){
	return NULL;
}

void Sprite::Draw(HDC _hDC) {
	// Draw the sprite if it isn't hidden
	if (bitmap != NULL && !isHidden)
	{
		// Draw the appropriate frame, if necessary
		if (numFrames == 1) {
			bitmap->Draw(_hDC, position.left, position.top, TRUE);
		}
		else {
			bitmap->DrawPart(_hDC, position.left, position.top,
				curFrame * GetWidth(), 0, GetWidth(), GetHeight(), TRUE);
		}
	}
}
