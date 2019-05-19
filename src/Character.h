//-----------------------------------------------------------------
// Character Object
// C++ Header - Character.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include <stack>
#include <vector>
#include "Sprite.h"

using std::stack;
using std::vector;

//-----------------------------------------------------------------
// Custom types
//-----------------------------------------------------------------
typedef vector<vector<int>> Map;
struct CharacterStats {
	int fireSpeed;
	int fireDelay;
	int health;
	int maxHealth;
	int speed;
	int armor;
};
typedef struct CharacterStats CharacterStats;

//-----------------------------------------------------------------
// Class data
//-----------------------------------------------------------------
enum AITask {
	AT_IDLE,
	AT_ROAM,
	AT_FOLLOW,
	AT_ATTACK
};
enum ControlStatus {
	CS_P1,
	CS_P2,
	CS_AI
};

//-----------------------------------------------------------------
// Character Class
//-----------------------------------------------------------------
class Character {
protected:
	std::string name;
	std::string description;
	Sprite *sprite;
	Sprite *menuSprite;
	POINT mapPosition;
	bool firstCreated = true;
	POINT fireDirection;
	int	curFireDelay;
	CharacterStats baseStats;
	CharacterStats stats;
	vector<Character*> currentTargets;
	stack<POINT> path;
	Character *target;
	AITask task;
	bool isRobot;
	bool ready;

	vector<Character*> nearbyRobots;
	vector<Character*> nearbyDemons;

public:
	// Constructor(s)/Destructor
	Character(std::string _name, std::string _description, Sprite* _sprite, Sprite *_menuSprite,
		int healthPoint, int _speed, POINT _mapPosition, int _fireSpeed);
	virtual ~Character();

	// Overload operator
	bool operator==(const Character& other) {
		return sprite->GetPosition().left == other.sprite->GetPosition().left
			&& sprite->GetPosition().top == other.sprite->GetPosition().top
			&& sprite->GetPosition().right == other.sprite->GetPosition().right
			&& sprite->GetPosition().bottom == other.sprite->GetPosition().bottom;
	}

	// General methods
	inline void ClearPath() { while (!path.empty()) path.pop(); };
	inline void TakeHit(int _damage) { stats.health -= _damage; };
	inline void Heal(int _healAmount) {
		stats.health += _healAmount;

		if (stats.health > stats.maxHealth) {
			stats.health = stats.maxHealth;
		}
	}
	inline void Move() {
		if (path.empty()) return;

		POINT next = (POINT)path.top();
		POINT current = { sprite->GetPosition().left, sprite->GetPosition().top };

		int dy = next.y - current.y;
		int dx = next.x - current.x;
		ready = false;
		if (dx == 0 && dy == 0) {
			sprite->SetPosition(next);
			path.pop();
			ready = true;
		}
		else if (dy != 0 && abs(dy) <= stats.speed) {
			current.y = next.y;
			sprite->SetPosition(current);
		}
		else if (dx != 0 && abs(dx) <= stats.speed) {
			current.x = next.x;
			sprite->SetPosition(current);
		}
		else if (abs(dx) > abs(dy)) {
			current.x += stats.speed * (dx / abs(dx));
			sprite->SetPosition(current);
		}
		else {
			current.y += stats.speed * (dy / abs(dy));
			sprite->SetPosition(current);
		}
	};
	inline void ResetStatsToDefault() {
		int health = stats.health;
		stats = baseStats;
		baseStats.health = health;
	}
	inline void BoostStats(int perc) {
		stats.armor = baseStats.armor + round((baseStats.armor / 100.0) * perc);
		stats.maxHealth = baseStats.maxHealth + round((baseStats.maxHealth / 100.0) * perc);
		stats.speed = 20;
	}
	virtual void Update();

	// Accessor methods
	std::string GetName() { return name; };
	std::string GetDescription() { return description; };
	Sprite*		GetSprite() { return sprite; };
	Sprite*		GetMenuSprite() { return menuSprite; };
	int			GetHealthPoint() { return stats.health; };
	int			GetMaxHealthPoint() { return stats.maxHealth; };
	int			GetSpeed() { return stats.speed; };
	void		SetSpeed(int _speed) { stats.speed = _speed; };
	int			GetArmor() { return stats.armor; };
	void		SetArmor(int _a) { stats.armor = _a; };
	int			GetFireSpeed() { return stats.fireSpeed; };
	void		SetFireSpeed(int _fs) { stats.fireSpeed = _fs; };
	POINT		GetFireDirection() { return fireDirection; };
	void		SetFireDirection(Character *closestEnemy) {
		POINT enemypoint;
		enemypoint.x = closestEnemy->GetMapPosition().x;
		enemypoint.y = closestEnemy->GetMapPosition().y;
		fireDirection.x = enemypoint.x - mapPosition.x;
		fireDirection.y = enemypoint.y - mapPosition.y;
	};
	void		SetFireDirection(POINT pt) { fireDirection = pt; };
	int			GetCurFireDelay() { return curFireDelay; };
	void		SetCurFireDelay(int _d) { curFireDelay = _d; };
	int			GetFireDelay() { return stats.fireDelay; };
	void		SetFireDelay(int _fd) { stats.fireDelay = _fd; };
	POINT		GetMapPosition() { return mapPosition; };
	void		SetMapPosition(POINT _pos) { mapPosition = _pos; };
	BOOL		IsRobot() { return isRobot; };
	BOOL		IsReady() { return ready; }
	void		SetReady(BOOL _ready) { ready = _ready; };

	AITask		GetTask() { return task; };
	void		SetTask(AITask _at) { task = _at; };
	stack<POINT> GetPath() { return path; };
	Character*	GetTarget() { return target; };
	void		SetTarget(Character *_ch) { target = _ch; };
	void		SetPath(stack<POINT> _path) { path = _path; };
	bool		GetFirstCreated() { return firstCreated; };
	void		SetFirstCreated(bool _f) { firstCreated = _f; };
	vector<Character*>	GetCurrentTargets() { return currentTargets; };
	void			SetCurrentTargets(vector<Character*> _currentTargets) { currentTargets = _currentTargets; };
	int			GetHealth() { return stats.health; };
	vector<Character*> GetNearbyDemons() { return nearbyDemons; };
	vector<Character*> GetNearbyRobots() { return nearbyRobots; };
	void SetNearbyDemons(vector<Character*> _d) { nearbyDemons = _d; };
	void SetNearbyRobots(vector<Character*> _r) { nearbyRobots = _r; };
};
