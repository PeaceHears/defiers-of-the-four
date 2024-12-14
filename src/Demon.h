//-----------------------------------------------------------------
// Bitmap Object
// C++ Header - Demon.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
using namespace std;
#include <windows.h>
#include <string>
#include <vector>
#include <queue> 
#include <random>

#include "Sprite.h"
#include "Robot.h"

#include "Resource.h"
#include "GameEngine.h"

#ifndef DEMONBASE_H
#define DEMONBASE_H
#endif


class DemonBase;


//-----------------------------------------------------------------
// Custom Data Types
//-----------------------------------------------------------------
typedef vector<vector<int>> Map;


enum DemonType {
	D_DEMON,
	D_BOSS1
};


class Coordinate2 {
public:
	int x;
	int y;
	int dist;
	Coordinate2* prev;
	POINT screenPos;

public:
	Coordinate2(int _x, int _y) {
		x = _x;
		y = _y;
		dist = 0;
		prev = NULL;
		screenPos = { (x * 32), (y * 32) };
	}
	Coordinate2(int _x, int _y, int _dist, Coordinate2* _prev) {
		x = _x;
		y = _y;
		dist = _dist;
		prev = _prev;
		screenPos = { (x * 32), (y * 32) };
	}
};

//-----------------------------------------------------------------
// Demon Class
//-----------------------------------------------------------------
class Demon : public Character
{
public:
	// Constructor(s)/Destructor
	Demon(const int _id, std::string _name, std::string _description,
		Sprite* _sprite, int _healthPoint, int _speed, POINT _mapPosition,
		DemonBase* base, int _fireSpeed);
	virtual ~Demon();

	virtual void Update() override;

	// Overload operator
	bool operator==(const Demon& other)
	{
		return sprite->GetPosition().left == other.sprite->GetPosition().left
			&& sprite->GetPosition().top == other.sprite->GetPosition().top
			&& sprite->GetPosition().right == other.sprite->GetPosition().right
			&& sprite->GetPosition().bottom == other.sprite->GetPosition().bottom;
	}

	// General Methods
	int				EuclideanDistance(Demon* demon, Robot* robot, DemonBase* demonBase);
	void			AttackByMaintainingTheDistance(int taskNumber, Demon* demon, Map currentMap, Robot* robot, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance);
	void			Situations(Map currentMap, vector<DemonBase*> demonBaseArray, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance);
	//void			seekTheRobots(Demon* demon, Map currentmap);
	void			WarnTheBaseDemons(int taskNumber, Demon* demon, Map currentMap, Demon* helplessDemon, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance);
	void			Chase(int taskNumber, Demon* demon, Robot* robot, Map currentMap);
	void			Evade(int taskNumber, Demon* demon, Robot* robot, Map currentMap);

	Robot* FindTheClosestRobot(Demon* demon, vector<Character*> robots_around_the_demon);
	DemonBase* FindTheClosestBase(Demon* demon, DemonBase* _pEnemyBase, vector<DemonBase*> demonBaseArray);
	BOOL			RestrictForTheBaseBoundaries(Demon* demon, POINT dp, Map currentMap);

	// Accessor Methods
	int				GetBaseNumber() { return base_number; };
	void			SetBaseNumber(int __base_number) { base_number = __base_number; };
	int				GetBaseNumberThatWarnedByTheHelplessDemon() { return base_number_that_warned_by_the_helpless_demon; };
	void			SetBaseNumberThatWarnedByTheHelplessDemon(int __base_number_that_warned_by_the_helpless_demon) { base_number_that_warned_by_the_helpless_demon = __base_number_that_warned_by_the_helpless_demon; };
	/*vector<Robot*>	GetCurrentTargets() { return currentTargets; };
	void			SetCurrentTargets(vector<Robot*> _currentTargets) { currentTargets = _currentTargets; };*/
	DemonBase* GetBase() { return base; };
	void			SetBase(DemonBase* _base) { base = _base; };
	DemonType		GetDemonType() { return demonType; };

	int				CoordinateDistance(Coordinate2* c1, Coordinate2* c2);
	stack<POINT>	FindPathBFS(POINT _src, POINT _dst, Map _currentMap);
	void			Attack(Demon* demon, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance);
	stack<POINT>	DemonRoam(Demon* _demon, POINT robotposition, POINT baselocation, Map currentmap);

	const int GetId() const { return id; }

protected:

	// Member Variables
	int id = 0;
	DemonType demonType;
	int base_number;
	int base_number_that_warned_by_the_helpless_demon;
	//vector<Robot*> currentTargets;
	DemonBase* base;

	// Helper Methods
	void Free();
};