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
#include <algorithm>
#include <time.h>
#include "Resource.h"
#include "Sprite.h"

using std::stack;
using std::vector;
using std::string;
using std::to_string;

extern HINSTANCE hInstance;

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

struct StatusMessage {
	string message;
	time_t end;
	COLORREF color;

	bool operator<(const StatusMessage& a) const
	{
		return end < a.end;
	}
};

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
	string name;
	string description;
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
	vector<StatusMessage> statusMessages;
	vector<Character*> nearbyRobots;
	vector<Character*> nearbyDemons;

public:
	// Constructor(s)/Destructor
	Character(string _name, string _description, Sprite* _sprite, Sprite *_menuSprite,
		int healthPoint, int _speed, POINT _mapPosition, int _fireSpeed);
	virtual ~Character();

	// Overload operator
	bool operator==(const Character& other) {
		return sprite->GetPosition().left == other.sprite->GetPosition().left
			&& sprite->GetPosition().top == other.sprite->GetPosition().top
			&& sprite->GetPosition().right == other.sprite->GetPosition().right
			&& sprite->GetPosition().bottom == other.sprite->GetPosition().bottom;
	}

	// Virtual
	virtual void Update();

	// Inline
	inline void ClearPath() { while (!path.empty()) path.pop(); };

	// General methods
	void TakeHit(int _damage);
	void Heal(int _healAmount);
	void Move();
	void ResetStatsToDefault();
	void BoostStats(int perc);
	void AddStatusMessage(string _msg, time_t _end, COLORREF _color = NULL);
	void UpdateStatusMessages();


	// Accessor methods
	string GetName() { return name; };
	string GetDescription() { return description; };
	Sprite*	GetSprite() { return sprite; };
	Sprite*	GetMenuSprite() { return menuSprite; };
	int	GetHealthPoint() { return stats.health; };
	int	GetMaxHealthPoint() { return stats.maxHealth; };
	int	GetSpeed() { return stats.speed; };
	void SetSpeed(int _speed) { stats.speed = _speed; };
	int	GetArmor() { return stats.armor; };
	void SetArmor(int _a) { stats.armor = _a; };
	int	GetFireSpeed() { return stats.fireSpeed; };
	void SetFireSpeed(int _fs) { stats.fireSpeed = _fs; };
	POINT GetFireDirection() { return fireDirection; };
	void SetFireDirection(Character *closestEnemy);
	void SetFireDirection(POINT pt) { fireDirection = pt; };
	int	GetCurFireDelay() { return curFireDelay; };
	void SetCurFireDelay(int _d) { curFireDelay = _d; };
	int	GetFireDelay() { return stats.fireDelay; };
	void SetFireDelay(int _fd) { stats.fireDelay = _fd; };
	POINT GetMapPosition() { return mapPosition; };
	void SetMapPosition(POINT _pos) { mapPosition = _pos; };
	BOOL IsRobot() { return isRobot; };
	BOOL IsReady() { return ready; }
	void SetReady(BOOL _ready) { ready = _ready; };
	AITask GetTask() { return task; };
	void SetTask(AITask _at) { task = _at; };
	stack<POINT> GetPath() { return path; };
	Character* GetTarget() { return target; };
	void SetTarget(Character *_ch) { target = _ch; };
	void SetPath(stack<POINT> _path) { path = _path; };
	bool GetFirstCreated() { return firstCreated; };
	void SetFirstCreated(bool _f) { firstCreated = _f; };
	vector<Character*>	GetCurrentTargets() { return currentTargets; };
	void SetCurrentTargets(vector<Character*> _currentTargets) { currentTargets = _currentTargets; };
	int	GetHealth() { return stats.health; };
	vector<Character*> GetNearbyDemons() { return nearbyDemons; };
	vector<Character*> GetNearbyRobots() { return nearbyRobots; };
	void SetNearbyDemons(vector<Character*> _d) { nearbyDemons = _d; };
	void SetNearbyRobots(vector<Character*> _r) { nearbyRobots = _r; };
	string GetStatus() {
		if (statusMessages.empty()) return "";
		else return statusMessages[0].message;
	};
	time_t GetStatusEnd() {
		if (statusMessages.empty()) return 0;
		else return statusMessages[0].end;
	};
	COLORREF GetStatusColor() {
		if (statusMessages.empty()) return NULL;
		else return statusMessages[0].color;
	};
};
