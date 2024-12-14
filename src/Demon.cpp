//-----------------------------------------------------------------
// Demon Object
// C++ Source - Demon.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Demon.h"
#include "DemonBase.h"

//-----------------------------------------------------------------
// Demon Constructor(s)/Destructor
//-----------------------------------------------------------------
Demon::Demon(const int _id, std::string _name, std::string _description, 
	Sprite* _sprite, int _healthPoint, int _speed, POINT _mapPosition,
	DemonBase* base, int _fireSpeed)
	:Character(_name, _description, _sprite, _sprite, _healthPoint, _speed, _mapPosition, _fireSpeed)
{
	id = _id;
	name = _name;
	description = _description;
	sprite = _sprite;
	mapPosition = _mapPosition;
	isRobot = false;

	CharacterStats pStats;
	pStats.fireDelay = 7;
	pStats.fireSpeed = _fireSpeed;
	pStats.health = pStats.maxHealth = _healthPoint;
	pStats.speed = _speed;
	stats = baseStats = pStats;
	fireDirection = { stats.fireSpeed, 0 };
}

Demon::~Demon()
{
}


//-----------------------------------------------------------------
// Demon General Methods
//-----------------------------------------------------------------
void Demon::Update()
{
	Character::Update();
}

void Demon::Situations(Map currentMap, vector<DemonBase*> demonBaseArray, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance) {

	/*seekTheRobots(demon, currentmap);

	POINT demonBasePosition;
	demonBasePosition.x = demon->GetBase()->GetMapPosition().x;
	demonBasePosition.y = demon->GetBase()->GetMapPosition().y;

	int robot_amount_in_the_base = demon->GetCurrentTargets().size();
	int demon_amount_in_the_base = demon->GetBase()->GetCurrentDemons().size();
	*/

	// no enemy is around
	if (this->GetNearbyRobots().size() == 0) {
		// TODO; maybe use skill too?
		this->SetTask(AT_IDLE);

		if (this->IsReady()) {
			if (this->GetFirstCreated()) {
				this->SetFirstCreated(false);
				this->SetPath(DemonRoam(this, this->GetBase()->GetMapPosition(), this->GetBase()->GetMapPosition(), currentMap));

			}
			else {
				this->SetPath(DemonRoam(this, this->GetMapPosition(), this->GetBase()->GetMapPosition(), currentMap));
			}
		}

	}
	else {

		int robot_amount_around_the_demon = this->GetNearbyRobots().size();
		int demon_amount_in_the_base = this->GetBase()->GetCurrentDemons().size();
		vector<Robot*> robots = reinterpret_cast<vector<Robot*> const&>(nearbyRobots);

		if (robot_amount_around_the_demon <= 2) { //TASK 2
			for (size_t i = 0; i < robot_amount_around_the_demon; i++)
				AttackByMaintainingTheDistance(2, this, currentMap, robots[i], game, bmDemonBullet, hInstance);
		}
		else if (robot_amount_around_the_demon > 2) { // TASK 3

			for (size_t i = 0; i < demon_amount_in_the_base; i++)
				WarnTheBaseDemons(3, this->GetBase()->GetCurrentDemons()[i], currentMap, this, game, bmDemonBullet, hInstance);
		
			if (demon_amount_in_the_base < 2 * robot_amount_around_the_demon) { // TASK 5

				DemonBase* the_closest_base = FindTheClosestBase(this, this->GetBase(), demonBaseArray);
				int demon_amount_in_the_closest_base = the_closest_base->GetCurrentDemons().size();

				for (size_t i = 0; i < demon_amount_in_the_closest_base; i++)
					WarnTheBaseDemons(5, the_closest_base->GetCurrentDemons()[i], currentMap, this, game, bmDemonBullet, hInstance);

				/*
				if (the_closest_base->GetCurrentDemons().size() > 2 * robot_amount_around_the_demon && demon_amount_in_the_base > 2) { // TASK 6

					for (size_t i = 0; i < demon_amount_in_the_base / 2; i++)
					{
						// move to closest base
						GoToTheClosestBase(6, this->GetBase()->GetCurrentDemons()[i], the_closest_base, currentMap, game, bmDemonBullet, hInstance); //en yakin base'e yardima gider.
					}
				}
				*/
			}

		}
		
		if (demon_amount_in_the_base >= 2 * robot_amount_around_the_demon) { //TASK 4

			for (size_t i = 0; i < robot_amount_around_the_demon; i++)
				AttackByMaintainingTheDistance(4, this, currentMap, robots[i], game, bmDemonBullet, hInstance);
		}

	}

}

/* void Demon::seekTheRobots(Demon* demon, Map currentmap) {

   POINT demonposition;
   demonposition.x = demon->GetMapPosition().x;
   demonposition.y = demon->GetMapPosition().y;
   const int range = 2;

   vector<int> arr_x;
   vector<int> arr_y;
   int i = 0;
   int value = 0;
   while (value < 2 * range + 1) {
	   arr_y.push_back(value);
	   arr_x.push_back(value);
	   if (i > 0) {
		   i++;
		   arr_y.push_back(-value);
		   arr_x.push_back(-value);

	   }

	   value++;
   }


   vector<Character*> robots_in_range; // Demon tipinde olacak

								   // Range içindeki düþmanlarý tespit eder ve bunlarý ateþ edilecek düþman arrayine ekler.
   for (size_t i = 0; i < sizeof(arr_x); i++)
   {
	   for (size_t j = 0; j < sizeof(arr_y); j++)
	   {
		   if (!(i == 0 && j == 0)) {

			   if (demonposition.x + arr_x[i] < 32 && demonposition.x + arr_x[i] >= 0 && demonposition.y + arr_y[j] < 24 && demonposition.y + arr_y[j] >= 0)
			   {
				   if (currentmap[demonposition.y + arr_y[j]][demonposition.x + arr_x[i]] == 3) // enemy 3
				   {
					   // get instance of the robot at position
					   //robots_in_range.push_back(GetPlayer(demonposition.y + arr_y[j], demonposition.x + arr_x[i]));
					   break;
				   }

			   }

		   }

	   }
   }

   demon->SetCurrentTargets(robots_in_range);
}
*/
/*
void Demon::GoToTheClosestBase(int taskNumber, Demon* demon, DemonBase* the_closest_base, Map currentMap, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance) {

	int robot_amount_around_the_demon = demon->GetNearbyRobots().size();
	vector<Robot*> robots = reinterpret_cast<vector<Robot*> const&>(nearbyRobots);

	for (size_t i = 0; i < robot_amount_around_the_demon; i++)
		AttackByMaintainingTheDistance(taskNumber, demon, currentMap, robots[i], game, bmDemonBullet, hInstance);

}*/

void Demon::WarnTheBaseDemons(int taskNumber, Demon* demon, Map currentMap, Demon* helplessDemon, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE hInstance) {

	POINT startingposition;
	startingposition.x = demon->GetMapPosition().x;
	startingposition.y = demon->GetMapPosition().y;

	POINT targetposition;
	targetposition.x = helplessDemon->GetMapPosition().x;
	targetposition.y = helplessDemon->GetMapPosition().y;

	int robot_amount_around_the_demon = demon->GetNearbyRobots().size();
	vector<Robot*> robots = reinterpret_cast<vector<Robot*> const&>(nearbyRobots);

	for (size_t i = 0; i < robot_amount_around_the_demon; i++)
		AttackByMaintainingTheDistance(taskNumber, demon, currentMap, robots[i], game, bmDemonBullet, hInstance);

}

void Demon::AttackByMaintainingTheDistance(int taskNumber, Demon* demon, Map currentMap, Robot* robot, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE   hInstance) {

	int distance = EuclideanDistance(demon, robot, NULL);

	if (distance > 4) {
		Chase(taskNumber, demon, robot, currentMap);
	}
	else if (distance < 4) {
		Evade(taskNumber, demon, robot, currentMap);
	}

	if (distance <= 5) { //başka base'den gelenlere ve attack için distance'ı dengeli tutmaya yönelik

		demon->SetTask(AT_ATTACK);

		if (taskNumber == 2 || taskNumber == 3)
			target = demon->GetNearbyRobots().front();
		else if (taskNumber == 4) {

			Robot* the_closest_robot = FindTheClosestRobot(demon, demon->GetNearbyRobots());

			target = the_closest_robot;
		}

		int roll = rand() % 2;
		// attack
		if (roll == 0) {
			demon->SetFireDirection(target);
			Attack(demon, game, bmDemonBullet, hInstance);
		}
	}


}

int Demon::EuclideanDistance(Demon* demon, Robot* robot, DemonBase* demonBase) {

	//HER KARAKTERÝN UPDATE FONKU ÝÇÝNDE DE ÇAÐRILICAK
	POINT startingposition;
	startingposition.x = demon->GetMapPosition().x;
	startingposition.y = demon->GetMapPosition().y;

	POINT targetposition;

	if (robot != NULL) {
		targetposition.x = robot->GetMapPosition().x;
		targetposition.y = robot->GetMapPosition().y;
	}
	else if (demonBase != NULL) {
		targetposition.x = demonBase->GetMapPosition().x;
		targetposition.y = demonBase->GetMapPosition().y;
	}

	int x = startingposition.x - targetposition.x;
	int y = startingposition.y - targetposition.y;
	int distance = pow(x, 2) + pow(y, 2);

	int EuclideanDistance = sqrt(distance);
	return EuclideanDistance;
}

void Demon::Chase(int taskNumber, Demon* demon, Robot* robot, Map currentMap) {

	POINT demonPosition;
	demonPosition.x = demon->GetMapPosition().x;
	demonPosition.y = demon->GetMapPosition().y;

	POINT robotPosition;
	robotPosition.x = robot->GetMapPosition().x;
	robotPosition.y = robot->GetMapPosition().y;

	POINT targetPosition = { demon->GetMapPosition().x , demon->GetMapPosition().y };

	if (demonPosition.x > robotPosition.x) {
		if (targetPosition.x - 1 >= 0) {
			if (currentMap[targetPosition.y][targetPosition.x - 1] == 0) {
				targetPosition.x = demon->GetMapPosition().x - 1;

			}
		}
	}

	else if (demonPosition.x < robotPosition.x) {
		if (targetPosition.x + 1 < 32) {
			if (currentMap[targetPosition.y][targetPosition.x + 1] == 0) {
				targetPosition.x = demon->GetMapPosition().x + 1;

			}
		}
	}

	if (demonPosition.y >= robotPosition.y) {
		if (targetPosition.y - 1 >= 0) {
			if (currentMap[targetPosition.y - 1][targetPosition.x] == 0) {
				targetPosition.y = demon->GetMapPosition().y - 1;

			}
		}
	}

	else if (demonPosition.y < robotPosition.y) {
		if (targetPosition.y + 1 < 24) {
			if (currentMap[targetPosition.y + 1][targetPosition.x] == 0) {
				targetPosition.y = demon->GetMapPosition().y + 1;
			}
		}
	}

	if (demon->IsReady()) {

		BOOL outOFbound = RestrictForTheBaseBoundaries(demon, targetPosition, currentMap);

		if (taskNumber != 5 && taskNumber != 6 && !outOFbound) {

			stack<POINT> path = FindPathBFS(demon->GetMapPosition(), targetPosition, currentMap);

			if (path.size() != 0)
				demon->SetPath(path);
			else {
				demon->ClearPath();
				demon->SetReady(true);
			}

		}
		else if (taskNumber == 5 || taskNumber == 6) {

			stack<POINT> path = FindPathBFS(demon->GetMapPosition(), targetPosition, currentMap);

			if (path.size() != 0)
				demon->SetPath(path);
			else {
				demon->ClearPath();
				demon->SetReady(true);
			}

		}
	}


}

void Demon::Evade(int taskNumber, Demon* demon, Robot* robot, Map currentMap) {

	POINT demonPosition;
	demonPosition.x = demon->GetMapPosition().x;
	demonPosition.y = demon->GetMapPosition().y;

	POINT robotPosition;
	robotPosition.x = robot->GetMapPosition().x;
	robotPosition.y = robot->GetMapPosition().y;

	POINT targetPosition = { demon->GetMapPosition().x , demon->GetMapPosition().y };

	if (demonPosition.x >= robotPosition.x) {
		if (targetPosition.x + 1 < 32) {
			if (currentMap[targetPosition.y][targetPosition.x + 1] == 0) {
				targetPosition.x = demon->GetMapPosition().x + 1;

			}
		}
	}

	else if (demonPosition.x < robotPosition.x) {
		if (targetPosition.x - 1 >= 0) {
			if (currentMap[targetPosition.y][targetPosition.x - 1] == 0) {
				targetPosition.x = demon->GetMapPosition().x - 1;

			}
		}
	}

	if (demonPosition.y >= robotPosition.y) {
		if (targetPosition.y + 1 < 24) {
			if (currentMap[targetPosition.y + 1][targetPosition.x] == 0) {
				targetPosition.y = demon->GetMapPosition().y + 1;

			}
		}
	}

	else if (demonPosition.y < robotPosition.y) {
		if (targetPosition.y - 1 >= 0) {
			if (currentMap[targetPosition.y - 1][targetPosition.x] == 0) {
				targetPosition.y = demon->GetMapPosition().y - 1;
			}
		}
	}

	if (demon->IsReady()) {

		BOOL outOFbound = RestrictForTheBaseBoundaries(demon, targetPosition, currentMap);

		if (taskNumber != 5 && taskNumber != 6 && !outOFbound) {

			stack<POINT> path = FindPathBFS(demon->GetMapPosition(), targetPosition, currentMap);

			if (path.size() != 0)
				demon->SetPath(path);
			else {
				demon->ClearPath();
				demon->SetReady(true);
			}

		}
		else if (taskNumber == 5 || taskNumber == 6) {

			stack<POINT> path = FindPathBFS(demon->GetMapPosition(), targetPosition, currentMap);

			if (path.size() != 0)
				demon->SetPath(path);
			else {
				demon->ClearPath();
				demon->SetReady(true);
			}

		}
	}

}

Robot* Demon::FindTheClosestRobot(Demon* demon, vector<Character*> robots_around_the_demon) {

	vector<int> arr_distance;

	for (size_t i = 0; i < robots_around_the_demon.size(); i++)
	{
		int distance = EuclideanDistance(demon, (Robot*)robots_around_the_demon[i], NULL);
		arr_distance.push_back(distance);
	}

	int min_distance = arr_distance[0];
	int index_of_the_closest_robot = 0;

	for (size_t i = 0; i < arr_distance.size(); i++)
	{

		if (arr_distance[i] < min_distance) {
			min_distance = arr_distance[i];
			index_of_the_closest_robot = i;
		}

	}

	return (Robot*)robots_around_the_demon[index_of_the_closest_robot];
}

DemonBase* Demon::FindTheClosestBase(Demon* demon, DemonBase* _pEnemyBase, vector<DemonBase*> demonBaseArray) {

	vector<int> arr_distance;
	int index_of_own_base = 0;

	for (size_t i = 0; i < demonBaseArray.size(); i++)
	{
		if (demonBaseArray[i] != demon->GetBase()) { // kendi base'i disindakilere bak.

			int distance = EuclideanDistance(demon, NULL, (DemonBase*)demonBaseArray[i]);
			arr_distance.push_back(distance);
		}
		else {
			index_of_own_base = i;
		}
	}

	int min_distance = arr_distance[0];
	int index_of_the_closest_base = 0;

	for (size_t i = 0; i < arr_distance.size(); i++)
	{
		if (arr_distance[i] < min_distance) {

			min_distance = arr_distance[i];
			index_of_the_closest_base = i;

		}
	}

	if(index_of_the_closest_base < index_of_own_base)  // kendi base'imizi katmadık.
		return demonBaseArray[index_of_the_closest_base];
	else {
		return demonBaseArray[index_of_the_closest_base + 1];
	}
}

BOOL Demon::RestrictForTheBaseBoundaries(Demon* demon, POINT dp, Map currentMap) {

	POINT demonPosition = dp;

	POINT demonBasePosition;
	demonBasePosition.x = demon->GetBase()->GetMapPosition().x;
	demonBasePosition.y = demon->GetBase()->GetMapPosition().y;

	int differenceBetweenX_Axis = abs(demonPosition.x - demonBasePosition.x);
	int differenceBetweenY_Axis = abs(demonPosition.y - demonBasePosition.y);

	if (differenceBetweenX_Axis > 3 || differenceBetweenY_Axis > 3) {
		return true;
	}
	else return false;

}

// Breadth first path finding
stack<POINT> Demon::FindPathBFS(POINT _src, POINT _dst, Map _currentMap) {
	int cols = 32;
	int rows = 24;
	Map map = _currentMap;
	queue<Coordinate2*> alt;
	Coordinate2 *c, *ct;
	stack<POINT> path;

	alt.push(new Coordinate2(_src.x, _src.y));

	while (!alt.empty()) {
		c = alt.front();
		alt.pop();

		if (map[c->y][c->x] != 0 && map[c->y][c->x] != 3 && map[c->y][c->x] != 4) continue;

		map[c->y][c->x] = 99; // mark as visited

		if (CoordinateDistance(c, new Coordinate2(_dst.x, _dst.y)) <= 1) {
			int i = 0;
			while (c->prev != NULL) {
				path.push(c->screenPos);
				c = c->prev;
				i++;
			}
			return path;
		}

		if (c->x + 1 < cols && map[c->y][c->x + 1] == 0) alt.push(new Coordinate2(c->x + 1, c->y, c->dist + 1, c));
		if (c->x - 1 >= 0 && map[c->y][c->x - 1] == 0) alt.push(new Coordinate2(c->x - 1, c->y, c->dist + 1, c));
		if (c->y + 1 < rows && map[c->y + 1][c->x] == 0) alt.push(new Coordinate2(c->x, c->y + 1, c->dist + 1, c));
		if (c->y - 1 >= 0 && map[c->y - 1][c->x] == 0) alt.push(new Coordinate2(c->x, c->y - 1, c->dist + 1, c));
	}

	return path; // no path;
};

// Manhattan distance of Coordinate class objects
int Demon::CoordinateDistance(Coordinate2 *c1, Coordinate2 *c2) {
	int dx, dy;

	dx = abs(c1->x - c2->x);
	dy = abs(c1->y - c2->y);
	return dx + dy;
}

// Fire from a character
void Demon::Attack(Demon* demon, GameEngine* game, Bitmap* bmDemonBullet, HINSTANCE  hInstance) {

	if (demon->GetCurFireDelay() < demon->GetFireDelay()) return;
	demon->SetCurFireDelay(0);

	if (demon->GetFireDirection().x == 0 && demon->GetFireDirection().y == 0) return;

	Sprite *bullet;

	PlaySound((LPCWSTR)IDW_DEMON_FIRE, hInstance, SND_ASYNC | SND_RESOURCE);
	bullet = new Sprite(bmDemonBullet);
	bullet->SetSpriteType(ST_ENEMY_BULLET);

	bullet->SetBoundsAction(BA_DIE);
	bullet->SetPosition(demon->GetSprite()->GetPosition().left + demon->GetSprite()->GetWidth() / 2, demon->GetSprite()->GetPosition().top + demon->GetSprite()->GetHeight() / 2);
	bullet->SetVelocity(demon->GetFireDirection());
	bullet->SetCharacter(demon);

	// Get bullet out of the character first
	while (bullet->TestCollision(demon->GetSprite())) {
		if (bullet->GetVelocity().x == 0 && bullet->GetVelocity().y == 0)
		{
			break;
		}
		bullet->SetPosition(bullet->GetPosition().left + demon->GetFireDirection().x, bullet->GetPosition().top + demon->GetFireDirection().y);
	}
	game->AddSprite(bullet);
}

stack<POINT> Demon::DemonRoam(Demon *_demon, POINT robotposition, POINT baselocation, Map currentmap) {
	/*int robotx= robot.GetMapPosition().x;
	int roboty = robot.GetMapPosition().y;*/
	const int range = 3;
	vector<int> arr_x;
	vector<int> arr_y;

	int x[] = { -3,-2,-1,0,1,2,3 };
	for (size_t i = 0; i < 7; i++)
	{
		arr_x.push_back(x[i]);
		arr_y.push_back(x[i]);
	}
	int isFound = 0;
	int target_y;
	int target_x;
	POINT targetposition;
	int border = 2;
	if (!_demon->GetFirstCreated()) {
		border = 1;

	}
	while (isFound < border)
	{
		std::random_device rd1; // obtain a random number from hardware
		std::mt19937 eng1(rd1()); // seed the generator
		std::uniform_int_distribution<> distr1(0, arr_x.size() - 2);
		target_x = distr1(eng1);
		target_x = arr_x[target_x] + baselocation.x;
		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 eng2(rd2()); // seed the generator
		std::uniform_int_distribution<> distr2(0, arr_x.size() - 2);
		target_y = distr2(eng2);
		target_y = arr_y[target_y] + baselocation.y;

		if (currentmap[target_y][target_x] == 0)
		{
			if (isFound == 0) {
				targetposition.x = target_x;
				targetposition.y = target_y;
			}
			else if (isFound == 1) {
				robotposition.x = target_x;
				robotposition.y = target_y;
			}
			isFound++;

		}
	}

	return FindPathBFS(robotposition, targetposition, currentmap);
}
