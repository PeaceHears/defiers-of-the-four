//-----------------------------------------------------------------
// Character Object
// C++ Source - Character.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Character.h"

//-----------------------------------------------------------------
// Character Constructor(s)/Destructor
//-----------------------------------------------------------------
Character::Character(string _name, string _description, Sprite* _sprite, Sprite* _menuSprite, 
	int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed) {
	name = _name;
	description = _description;
	sprite = _sprite;
	menuSprite = _menuSprite;
	mapPosition = _mapPosition;
	curFireDelay = 7;
	task = AT_IDLE;
	ready = true;

	CharacterStats pStats;
	pStats.fireDelay = 7;
	pStats.fireSpeed = _fireSpeed;
	pStats.health = pStats.maxHealth = _healthPoint;
	pStats.speed = _speed;
	pStats.armor = 10;
	stats = baseStats = pStats;
	fireDirection = { stats.fireSpeed, 0 };
}

Character::~Character()
{
}


//-----------------------------------------------------------------
// Character General Methods
//-----------------------------------------------------------------
void Character::Heal(int _healAmount) {
	int absHeal = _healAmount;
	stats.health += _healAmount;
	if (stats.health > stats.maxHealth) {
		absHeal = stats.maxHealth - stats.health;
		stats.health = stats.maxHealth;
	}
	time_t now;
	if (absHeal > 0) {
		AddStatusMessage({ "+" + to_string(absHeal) }, time(&now) + 2, RGB(15, 200, 15));
		PlaySound((LPCWSTR)IDW_HEAL, hInstance, SND_ASYNC | SND_RESOURCE);
	}
}

void Character::TakeHit(int _damage) { 
	int absDamage = max(_damage-stats.armor, 0);
	stats.health -= absDamage;
	time_t now;
	
	if (absDamage > 0) AddStatusMessage({"-" + to_string(absDamage)}, time(&now) + 2, RGB(200, 15, 15));
};

void Character::Move() {
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

void Character::SetFireDirection(Character *closestEnemy) {
	POINT enemypoint;
	enemypoint.x = closestEnemy->GetMapPosition().x;
	enemypoint.y = closestEnemy->GetMapPosition().y;
	fireDirection.x = enemypoint.x - mapPosition.x;
	fireDirection.y = enemypoint.y - mapPosition.y;
};

void Character::BoostStats(int perc) {
	stats.armor = baseStats.armor + round((baseStats.armor / 100.0) * perc);
	stats.maxHealth = baseStats.maxHealth + round((baseStats.maxHealth / 100.0) * perc);
	stats.speed = baseStats.speed + round((baseStats.speed / 100.0) * perc);
}

void Character::ResetStatsToDefault() {
	CharacterStats temp = baseStats;
	temp.health = stats.health;
	stats = temp;
}

void Character::AddStatusMessage(string _msg, time_t _end, COLORREF _color) {
	// add new message
	StatusMessage sm;
	sm.message = _msg;
	sm.end = _end;
	sm.color = _color;
	statusMessages.push_back(sm);
	// sort, so that the soonest to expire will be on the top
	sort(statusMessages.begin(), statusMessages.end());
}

void Character::UpdateStatusMessages() {
	// delete expired messages
	time_t now;
	time(&now);
	auto it = statusMessages.begin();
	while (it != statusMessages.end()) {
		if (difftime(now, ((StatusMessage)(*it)).end) >= 0) {
			it = statusMessages.erase(it);
		}
		else {
			++it;
		}
	}
}
