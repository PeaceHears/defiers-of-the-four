//-----------------------------------------------------------------
// Defiers of the Four Application
// C++ Source - DotF.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DotF.h"

#define ALLY_BULLET_DAMAGE 10
#define ENEMY_BULLET_DAMAGE 20
#define FIRE_RANGE 3

//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE _hInstance)
{
	// Create the game engine
	game = new GameEngine(_hInstance, (LPTSTR)TEXT("Defiers of the Four"),
		(LPTSTR)TEXT("Defiers of the Four"), (WORD)IDI_DOTF, (WORD)IDI_DOTF_SM, 1024, 768);
	if (game == NULL)
		return FALSE;

	// Set the frame rate
	game->SetFrameRate(60);

	// Store the instance handle
	hInstance = _hInstance;

	// Configure game variables
	currentScene = MENU_MAIN;
	playerCount = -1;

	return TRUE;
}

void GameStart(HWND _hWindow)
{
	// Seed the random number generator
	srand(GetTickCount());

	// Create the offscreen device context and bitmap
	offScreenDC = CreateCompatibleDC(GetDC(_hWindow));
	offScreenBitmap = CreateCompatibleBitmap(GetDC(_hWindow),
		game->GetWidth(), game->GetHeight());
	SelectObject(offScreenDC, offScreenBitmap);

	// Device context
	HDC hDC = GetDC(_hWindow);

	// Set initial values
	numSwitch = 0;
	dlPlayer1Switch = 0;
	dlPlayer2Switch = 0;

	// Load bitmaps and sprites
	bmMenuBackground = new Bitmap(hDC, 1024, 768, RGB(22, 15, 28));
	bmMenuTitle = new Bitmap(hDC, IDB_MENU_TITLE, hInstance);
	bmMenuStar = new Bitmap(hDC, IDB_MENU_STAR, hInstance);
	bmBullet = new Bitmap(hDC, IDB_BULLET, hInstance);

	bmWall = new Bitmap(hDC, IDB_WALL, hInstance);
	bmEnemyBase = new Bitmap(hDC, IDB_ENEMY_BASE, hInstance);

	bmRobotMenu = new Bitmap(hDC, IDB_MENU_ROBOT, hInstance); // 100x100
	bmRobot = new Bitmap(hDC, IDB_MONSTER, hInstance); // 32x32

	// Create initial game elements
	CreateButtons(hDC);
	CreateRobots(hDC);

	/*RECT bounds = { 0, 0, 1024, 768 };
	bmDemon = new Bitmap(hDC, IDB_DEMON, hInstance);
	spDemon = new Sprite(bmDemon, bounds,  BA_WRAP);
	game->AddSprite(spDemon);
	spDemon->SetPosition(200, 200);
	spDemon->SetNumFrames(18);
	spDemon->SetFrameDelay(5);
	spDemon->SetVelocity(2, 0);*/
	

}

void GameEnd()
{
	// Cleanup the offscreen device context and bitmap
	DeleteObject(offScreenBitmap);
	DeleteDC(offScreenDC);

	// Cleanup the sprites
	game->CleanupSprites();

	// Cleanup the game engine
	delete game;
}

void GameActivate(HWND _hWindow) {}

void GameDeactivate(HWND _hWindow) {}

void GamePaint(HDC _hDC)
{
	switch (currentScene) {
	case MENU_MAIN:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Defiers of the Four"), 96, { 0, 150, 1024, 300 });
		for (auto &Button : menuMainButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_PLAYERS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Game Type"), 64, { 0, 150, 1024, 300 });
		for (auto &Button : menuPlayersButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_ROBOTS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Select Characters"), 64, { 0, 150, 1024, 300 });

		// Draw robots
		DrawRobotsOnMenu(_hDC);
		// Select menu buttons
		btnBackToPlayers->Draw(_hDC);
		if (selectedRobotIndexes.size() == playerCount * 2) {
			btnReady->Draw(_hDC);
		}
		break;
	case GAME_PLAY:
		bmMenuBackground->Draw(_hDC, 0, 0);

		// Draw sprites
		game->DrawSprites(_hDC);

		// Draw map values
		/*for (int i = 0; i < currentMap.size(); i++) {
			for (int  j = 0; j < currentMap[i].size(); j++) {
				RECT bounds = { j * 32, i * 32, 16 + (j * 32),  16 + (i * 32) };
				game->PrintText(_hDC, ConvertString(std::to_string(currentMap[i][j])), 16, bounds);
			}
		}*/

		// Draw paths
		/*for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == AI) {
				PrintPathOnScreen(_hDC, Robot->GetPath());
				POINT curPos = { Robot->GetSprite()->GetPosition().left, Robot->GetSprite()->GetPosition().top };
				RECT bounds = bounds = { curPos.x, curPos.y, 16 + (curPos.x), 16 + (curPos.y) };
				game->PrintText(_hDC, ConvertString({ 'X' }), 16, bounds);
			}
		}*/
		
		for (auto &Robot : inGameRobots) {
			DrawIndicators(_hDC, Robot);
		}

		for (auto &DemonBase : demonBases) {
			for (auto &Demon : DemonBase->GetCurrentDemons()) {
				DrawIndicators(_hDC, Demon);
			}
		}



		break;
	case GAME_PAUSE:
		break;
	default:
		break;
	}

}

void GameCycle()
{
	// Update the sprites
	game->UpdateSprites();

	if (currentScene == GAME_PLAY) {
		UpdateCharacters();	
	}

	// Obtain a device context for repainting the game
	HWND  hWindow = game->GetWindow();
	HDC   hDC = GetDC(hWindow);

	// Paint the game to the offscreen device context
	GamePaint(offScreenDC);

	// Blit the offscreen bitmap to the game screen
	BitBlt(hDC, 0, 0, game->GetWidth(), game->GetHeight(),
		offScreenDC, 0, 0, SRCCOPY);

	// Cleanup
	ReleaseDC(hWindow, hDC);
}

BOOL SpriteCollision(Sprite* _spriteHitter, Sprite* _spriteHittee)
{
	// Character vs Obstacle
	if (_spriteHitter->GetSpriteType() == ST_CHARACTER && _spriteHittee->GetSpriteType() == ST_OBSTACLE) {
	}
	// Ally bullet vs Character
	else if (_spriteHitter->GetSpriteType() == ST_ALLY_BULLET && (_spriteHittee->GetSpriteType() == ST_CHARACTER)) {
		if (_spriteHittee->GetCharacter() != NULL) {
			_spriteHittee->GetCharacter()->TakeHit(ALLY_BULLET_DAMAGE);

			if (_spriteHittee->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHittee->GetCharacter());
			}
		}
		_spriteHitter->Kill();
	}
	// Enemy bullet vs Demon
	else if (_spriteHitter->GetSpriteType() == ST_ENEMY_BULLET && (_spriteHittee->GetSpriteType() == ST_DEMON)) {
		_spriteHitter->Kill();
	}
	// Demon vs Obstacle
	else if (_spriteHitter->GetSpriteType() == ST_DEMON && _spriteHittee->GetSpriteType() == ST_OBSTACLE) {
		_spriteHitter->SetVelocity(-(_spriteHitter->GetVelocity().x), -(_spriteHitter->GetVelocity().y));
	}
	// Ally bullet vs Demon
	else if (_spriteHitter->GetSpriteType() == ST_ALLY_BULLET && _spriteHittee->GetSpriteType() == ST_DEMON) {
		if (_spriteHittee->GetCharacter() != NULL) {
			_spriteHittee->GetCharacter()->TakeHit(ALLY_BULLET_DAMAGE);

			if (_spriteHittee->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHittee->GetCharacter());
			}
		}
		_spriteHitter->Kill();
	} 
	// Enemy bullet vs Robot
	else if (_spriteHitter->GetSpriteType() == ST_ENEMY_BULLET && _spriteHittee->GetSpriteType() == ST_CHARACTER) {
		if (_spriteHittee->GetCharacter() != NULL) {
			_spriteHittee->GetCharacter()->TakeHit(ENEMY_BULLET_DAMAGE);

			if (_spriteHittee->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHittee->GetCharacter());
			}
		}
		_spriteHitter->Kill();
	}

	else if ((_spriteHitter->GetSpriteType() == ST_ALLY_BULLET || _spriteHitter->GetSpriteType() == ST_ENEMY_BULLET) && (_spriteHittee->GetSpriteType() != ST_CHARACTER)) {
		_spriteHitter->Kill();
	}

	return TRUE;
}

void SpriteDying(Sprite* _sprite)
{

}


//-----------------------------------------------------------------
// Input
//-----------------------------------------------------------------
void HandleKeys()
{
	if (currentScene == GAME_PLAY) {
		Player1Controls();
		if (playerCount == 2) {
			Player2Controls();
		}
	}
}

void MouseButtonDown(int _x, int _y, BOOL _isLeftClick)
{
}

void MouseButtonUp(int _x, int _y, BOOL _isLeftClick)
{
	if (_isLeftClick) {
		if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || currentScene == MENU_SELECT_ROBOTS) {
			HandleMenuButtonClick(_x, _y);
		}

	}
}

void MouseMove(int _x, int _y)
{
	if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || currentScene == MENU_SELECT_ROBOTS) {
		// Hover effect for menu
		HandleMenuButtonHover(_x, _y);
	}
}

void HandleJoystick(JOYSTATE _joystickState) {}

//-----------------------------------------------------------------
// Game
//-----------------------------------------------------------------

// Update character actions.
void UpdateCharacters() {
	currentMap = maps[0];
	POINT pos;
	// Update positions
	for (auto &Robot : inGameRobots) {
		pos = ScreenRectToArrayPoint(Robot->GetSprite()->GetPosition());
		Robot->SetMapPosition(pos);
		currentMap[pos.y][pos.x] = 3;
	}

	for (auto &DemonBase : demonBases) {
		for (auto &Demon : DemonBase->GetCurrentDemons()) {
			pos = ScreenRectToArrayPoint(Demon->GetSprite()->GetPosition());
			Demon->SetMapPosition(pos);
			currentMap[pos.y][pos.x] = 4;

			/* TEMP FIRE AI */
			POINT pos = Demon->GetMapPosition();
			Demon->SetTask(AT_IDLE);
			for (int i = -FIRE_RANGE; i < FIRE_RANGE; i++) {
				for (int j = -FIRE_RANGE; j < FIRE_RANGE; j++) {
					int checkX = Demon->GetMapPosition().x + j;
					int checkY = Demon->GetMapPosition().y + i;

					if (checkX < 0 || checkY < 0) continue;
					if (checkX >= 32) continue;
					if (checkY >= 24) continue;

					if (currentMap[checkY][checkX] == 3) {
						Demon->SetTask(AT_ATTACK);
						POINT target = { checkY * 32, checkX * 32 };
						POINT self = RectToPoint(Demon->GetSprite()->GetPosition());
						target.x = self.x - target.x;
						target.y = self.y - target.y;

						/*int mult = sqrt(pow(target.x, 2) + pow(target.y,2));
						target.x *= Demon->GetFireSpeed() / mult;
						target.y *= Demon->GetFireSpeed() / mult;*/

						Demon->SetFireDirection({ rand() % 7 - 4,rand()%7-4 });
						Fire(Demon);
						break;
					}
				}
			}
		}
	}

	Robot* target;
	for (auto &Robot : inGameRobots) {
		// If not player controlled, then call update for that robot
		if (Robot->GetControlStatus() == AI) {
			Robot->SetTask(AT_FOLLOW);
			Robot->SetSpeed(4);
			target = GetPlayersRobot(1);
			POINT curPos = Robot->GetMapPosition();
			POINT targetPos = target->GetMapPosition();
			if (Robot->GetPath().empty()) {
				stack<POINT> path = FindPathBFS(curPos, targetPos, currentMap);
				Robot->SetTarget(GetPlayersRobot(1));
				Robot->SetPath(path);
				stack<POINT> curPath = Robot->GetPath();
			}

			/* TEMP FIRE AI*/
			if (rand() % 5 == 0) {
			
				for (int i = -FIRE_RANGE; i < FIRE_RANGE; i++) {
					for (int j = -FIRE_RANGE; j < FIRE_RANGE; j++) {
						int checkX = Robot->GetMapPosition().x + j;
						int checkY = Robot->GetMapPosition().y + i;

						if (checkX < 0 || checkY < 0) continue;
						if (checkX >= 32) continue;
						if (checkY >= 24) continue;

						if (currentMap[checkY][checkX] == 4) {
							Robot->SetTask(AT_ATTACK);
							POINT target = { checkY * 32, checkX * 32 };
							POINT self = RectToPoint(Robot->GetSprite()->GetPosition());
							target.x = self.x - target.x;
							target.y = self.y - target.y;

							Robot->SetFireDirection({ rand() % 7 - 4,rand() % 7 - 4 });
							Fire(Robot);
							break;
						}
					}
				}
			}


			Robot->Update();
		}
	}

	for (auto &DemonBase : demonBases) {
		for (auto &Demon : DemonBase->GetCurrentDemons()) {

			/*if (Demon->GetPath().empty()){
				stack<POINT> path = DemonRoam(Demon->GetMapPosition(), Demon->GetBase()->GetMapPosition(), currentMap);
				Demon->SetPath(path);
				PrintPath(path);
			}*/

			
			Demon->Update();
		}
	}
}

// Creates all robots in the game at the start.
void CreateRobots(HDC _hDC) {
	Sprite *robotSprite, *menuSprite;
	POINT pos;
	pos.x = 250;
	pos.y = 250;

	// 1 
	robotSprite = new Sprite(bmRobot);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot1 = new Robot({ "Robot 1" }, { "Robot 1 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14, AI, 10, 30);
	robotSprite->SetCharacter(robot1);

	// 2
	robotSprite = new Sprite(bmRobot);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot2 = new Robot({ "Robot 2" }, { "Robot 2 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14, AI, 10, 30);
	robotSprite->SetCharacter(robot2);

	// 3
	robotSprite = new Sprite(bmRobot);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot3 = new Robot({ "Robot 3" }, { "Robot 3 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14, AI, 10, 30);
	robotSprite->SetCharacter(robot3);

	// 4 
	robotSprite = new Sprite(bmRobot);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot4 = new Robot({ "Robot 4" }, { "Robot 4 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14, AI, 10, 30);
	robotSprite->SetCharacter(robot4);

	// Add to vector
	robots.push_back(robot1);
	robots.push_back(robot2);
	robots.push_back(robot3);
	robots.push_back(robot4);
}

// Handles Player 1's key controls, only in game play scene.
void Player1Controls() {
	Robot *robot = GetPlayersRobot(1);
	// Robot movement
	POINT velocity = { 0,0 };
	POINT fireDirection = { 0, 0 };
	if (GetAsyncKeyState('W') < 0) {
		velocity.y = -robot->GetSpeed();
		fireDirection.y = -robot->GetFireSpeed();
	}
	else if (GetAsyncKeyState('S') < 0) {
		velocity.y = robot->GetSpeed();
		fireDirection.y = robot->GetFireSpeed();
	}

	if (GetAsyncKeyState('A') < 0) {
		velocity.x = -robot->GetSpeed();
		fireDirection.x = -robot->GetFireSpeed();
	}
	else if (GetAsyncKeyState('D') < 0) {
		velocity.x = robot->GetSpeed();
		fireDirection.x = robot->GetFireSpeed();
	}

	// Set fire direction
	if (velocity.x != 0 || velocity.y != 0) {
		robot->SetFireDirection(fireDirection);
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}

	velocity = CheckCollisionBeforeMoving(robot, velocity);
	robot->GetSprite()->SetVelocity(velocity);

	// Switch robots
	if (++dlPlayer1Switch > 30 && GetAsyncKeyState(VK_LSHIFT) < 0) {
		SwitchRobot(1);
		dlPlayer1Switch = 0;
	}

	// Fire
	robot->SetCurFireDelay(robot->GetCurFireDelay() + 1);
	if (robot->GetCurFireDelay() > robot->GetFireDelay() && GetAsyncKeyState(VK_LCONTROL)) {
		Fire(robot);
		robot->SetCurFireDelay(0);
	}

	// Abilities

	// Pause
}

// Handles Player 2's key controls, only in game play scene.
void Player2Controls() {
	Robot *robot = GetPlayersRobot(2);
	// Robot movement
	POINT velocity = { 0,0 };
	POINT fireDirection = { 0, 0 };
	if (GetAsyncKeyState(VK_UP) < 0) {
		velocity.y = -robot->GetSpeed();
		fireDirection.y = -robot->GetFireSpeed();
	}
	else if (GetAsyncKeyState(VK_DOWN) < 0) {
		velocity.y = robot->GetSpeed();
		fireDirection.y = robot->GetFireSpeed();
	}

	if (GetAsyncKeyState(VK_LEFT) < 0) {
		velocity.x = -robot->GetSpeed();
		fireDirection.x = -robot->GetFireSpeed();
	}
	else if (GetAsyncKeyState(VK_RIGHT) < 0) {
		velocity.x = robot->GetSpeed();
		fireDirection.x = robot->GetFireSpeed();
	}

	// Set fire direction
	if (velocity.x != 0 || velocity.y != 0) {
		robot->SetFireDirection(fireDirection);
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}

	velocity = CheckCollisionBeforeMoving(robot, velocity);
	robot->GetSprite()->SetVelocity(velocity);

	// Switch robots
	if (++dlPlayer2Switch > 30 && GetAsyncKeyState(VK_NUMPAD0) < 0) {
		SwitchRobot(2);
		dlPlayer2Switch = 0;
	}

	// Fire
	robot->SetCurFireDelay(robot->GetCurFireDelay() + 1);
	if (robot->GetCurFireDelay() > robot->GetFireDelay() && GetAsyncKeyState(VK_NUMPAD1)) {
		Fire(robot);
		robot->SetCurFireDelay(0);
	}

	// Abilities

	// Pause
}

// Returns the robot currently controlled by _player.
Robot* GetPlayersRobot(int _player) {
	switch (_player) {
	case 1:
		for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == PLAYER_1) {
				return Robot;
			}
		}
		return NULL;
	case 2:
		for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == PLAYER_2) {
				return Robot;
			}
		}
		return NULL;
	default:
		// This should not happen
		return NULL;
	}
}

// Switch to next AI robot for _player.
void SwitchRobot(int _player) {
	if (inGameRobots.size() == playerCount) return;
	int setAI = -1;
	int setPlayer = -1;
	while (1) {
		if (inGameRobots.at(numSwitch)->GetControlStatus() == AI) {
			setPlayer = numSwitch;
		}
		else if (_player == 1 && inGameRobots.at(numSwitch)->GetControlStatus() == PLAYER_1) {
			setAI = numSwitch;
		}
		else if (_player == 2 && inGameRobots.at(numSwitch)->GetControlStatus() == PLAYER_2) {
			setAI = numSwitch;
		}

		numSwitch++;
		if (numSwitch == inGameRobots.size()) {
			numSwitch = 0;
		}

		if (setAI != -1 && setPlayer != -1) {
			inGameRobots.at(setAI)->SetControlStatus(AI);
			inGameRobots.at(setAI)->GetSprite()->SetVelocity(0, 0);

			if (_player == 1) {
				inGameRobots.at(setPlayer)->SetControlStatus(PLAYER_1);
			}
			else {
				inGameRobots.at(setPlayer)->SetControlStatus(PLAYER_2);
			}
			return;
		}
	}
}

// Fire from a character
void Fire(Character *character) {
	Sprite *bullet;
	bullet = new Sprite(bmBullet);
	bullet->SetBoundsAction(BA_DIE);
	bullet->SetPosition(character->GetSprite()->GetPosition().left + character->GetSprite()->GetWidth() / 2, character->GetSprite()->GetPosition().top + character->GetSprite()->GetHeight() / 2);
	bullet->SetVelocity(character->GetFireDirection());
	
	bullet->SetSpriteType(ST_ALLY_BULLET);
	if (!character->IsRobot() || ((Robot*)character)->GetControlStatus() == AI) {
		character->SetCurFireDelay(character->GetCurFireDelay()+1);
		if (character->GetCurFireDelay() < character->GetFireDelay()) {
			delete bullet;
			return;
		}

		if (!character->IsRobot()) {
			bullet->SetSpriteType(ST_ENEMY_BULLET);
		}
		
		character->SetCurFireDelay(0);
	}

	while (bullet->TestCollision(character->GetSprite())) {
		bullet->SetPosition(bullet->GetPosition().left + character->GetFireDirection().x, bullet->GetPosition().top + character->GetFireDirection().y);
	}
	game->AddSprite(bullet);
	PlaySound((LPCWSTR)IDW_GUN_SHOT, hInstance, SND_ASYNC | SND_RESOURCE);
}

Demon* AddDemon(DemonBase* _base) {
	
	POINT basePos = RectToPoint(_base->GetSprite()->GetPosition());
	POINT arrayPos = { basePos.x / 32, basePos.y / 32 };
	RECT bounds = { basePos.x - (32 * 3), basePos.y - (32 * 3), basePos.x + (32 * 4), basePos.y + (32 * 4) };
	basePos.x += 32;
	
	Sprite* demonSprite = new Sprite(bmRobot);
	demonSprite->SetSpriteType(ST_DEMON);
	demonSprite->SetPosition(basePos);
	demonSprite->SetBounds(bounds);
	demonSprite->SetVelocity(rand() % 3 + 1, 0);
	demonSprite->SetBoundsAction(BA_BOUNCE);
	game->AddSprite(demonSprite);
	Demon* demon = new Demon({ "Demon" }, { "Demon description" }, demonSprite, 20, 10, { arrayPos.x+1, arrayPos.y }, _base, 10);
	demon->SetBase(_base);
	//demon->SetFireDelay(30);
	demonSprite->SetCharacter(demon);
	_base->AddDemonToVector(demon);
	return demon;
}

void KillCharacter(Character* _character) {
	if (_character->IsRobot()) {
		Robot *robot = (Robot*)_character;

		// Switch before deleting, if controlled by character
		if (robot->GetControlStatus() == PLAYER_1) {
			if (inGameRobots.size() == 1) {
				currentScene = MENU_SELECT_PLAYERS;
				return;
			}
			SwitchRobot(1);
		}

		inGameRobots.erase(std::remove(inGameRobots.begin(), inGameRobots.end(), robot), inGameRobots.end());
		robot->GetSprite()->Kill();

		delete robot;
	}
	else {
		Demon *demon = (Demon*)_character;
		demon->GetBase()->RemoveDemon(demon);
		demon->GetSprite()->Kill();
		delete demon;
	}
}

// Passes selected robot indexes to Robot instances.
// Assigns robots to players.
// Creates map.
void InitializeLevel() {
	// Get selected robots IDs, then find the instance
	// of these robots, and push it to vector which
	// will be used during gameplay to control robots.
	// Also add sprites to sprite manager.
	for (auto i : selectedRobotIndexes) {
		inGameRobots.push_back(robots[i]);
		robots[i]->GetSprite()->SetPosition((14+i) * 32, 23*32);
		robots[i]->GetSprite()->SetZOrder(10);
		game->AddSprite(robots[i]->GetSprite());
	}

	// Initially add robot control to each player.
	// Default value of control status is AI.
	inGameRobots[0]->SetControlStatus(PLAYER_1);
	if (playerCount == 2) {
		inGameRobots[1]->SetControlStatus(PLAYER_2);
	}

	// Create map
	for (size_t i = 0; i < 8; i++)
	{
		if (i == 0) {
			Map newmap = CreateMap(true);
			maps[i] = newmap;
		}
		else
		{
			Map newmap = CreateMap(false);
			maps[i] = newmap;
		}
	}

	int enemyBaseCount = 0;
	int wallCount = 0;

	// 0 -> Empty
	// 1 -> Enemy base
	// 2 -> Wall
	for (int i = 0; i < maps[0].size(); i++)
	{
		for (int j = 0; j < maps[0][0].size(); j++)
		{
			// left to right; which map, which row, which col.
			if (maps[0][i][j] == 1)
			{
				
				enemyBases[enemyBaseCount] = new Sprite(bmEnemyBase);
				DemonBase* base = new DemonBase({ j, i }, enemyBaseCount, enemyBases[enemyBaseCount]);
				base->SetMapPosition(ScreenRectToArrayPoint(enemyBases[enemyBaseCount]->GetPosition()));
				demonBases.push_back(base);
				enemyBases[enemyBaseCount]->SetSpriteType(ST_OBSTACLE);
				game->AddSprite(enemyBases[enemyBaseCount]);
				enemyBases[enemyBaseCount]->SetPosition(j * 32, i * 32);
				numEnemyBases++;
				enemyBaseCount++;

				AddDemon(base);
			}
			if (maps[0][i][j] == 2)
			{
				walls[wallCount] = new Sprite(bmWall);
			
				walls[wallCount]->SetSpriteType(ST_OBSTACLE);
				game->AddSprite(walls[wallCount]);
				walls[wallCount]->SetPosition(j * 32, i * 32);
				wallCount++;
			}
		}
	}
}

// Handles map generation.
// Returns a 2D int vector, numbers representing game objects.
Map CreateMap(bool _firstLevel)
{
	// Draw the sprites in the sprite vector
	int number_of_wall = 100;
	int wall_lenght;// yan yana gelebilecek duvar sayısı
	int number_of_enemy_center;
	int wall_width = 32;
	int wall_height = 32;
	int enemy_center_width = 32;
	int enemy_center_height = 32;
	const int screen_width = 1024; // column =x  
	const int screen_height = 700; // row =y
	srand(time(NULL));
	number_of_enemy_center = rand() % 10 + 3;     //range 1 to 20
	int map_row = 24;
	int map_col = 32;
	vector< vector<int> > map(map_row, vector<int>(map_col, 0));

	if (_firstLevel)
	{
		for (size_t i = 20; i < map_row; i++)
		{
			for (size_t j = 0; j < map_col; j++)
			{
				map[i][j] = 0;
			}
		}
		map_row = 20;
	}
	else
	{
		map_row = 24;
	}

	vector<vector<int>> wall_positions(number_of_wall, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 
	vector<vector<int>> enemy_center_positions(number_of_enemy_center, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 

	// 0: empty
	// 1: enemy base
	// 2: wall
	int array_counter = 0;
	while (number_of_enemy_center > 0) {
		int row_a[] = { 4,11,18 };
		int col_a[] = { 4,11,18,25 };
		srand(time(NULL));
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng3(rd()); // seed the generator
		std::uniform_int_distribution<> distr3(0, 3);
		int col = distr3(eng3); //rand() % 5; //4,11,17,23,29
		col = col_a[col];
		srand(time(NULL));
		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 eng4(rd2()); // seed the generator
		std::uniform_int_distribution<> distr4(0, 2);
		int row = distr4(eng4); //4,11,17,23
		row = row_a[row];
		bool avaliable_for_enemyC = false;

		while (!avaliable_for_enemyC)
		{
			if (map[row][col] != 0)
			{
				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng3(rd()); // seed the generator
				std::uniform_int_distribution<> distr3(0, 3);
				col = distr3(eng3); //rand() % 5; //4,11,17,23,29
				col = col_a[col];
				srand(time(NULL));
				std::random_device rd2; // obtain a random number from hardware
				std::mt19937 eng4(rd2()); // seed the generator
				std::uniform_int_distribution<> distr4(0, 2);
				row = distr4(eng4);
				row = row_a[row];
				break;
			}
			else {
				avaliable_for_enemyC = true;
				enemy_center_positions[array_counter][0] = row * 32;
				enemy_center_positions[array_counter][1] = col * enemy_center_height;
				array_counter++;
				number_of_enemy_center--;
				map[row][col] = 1;
			}
		}
	}

	array_counter = 0;
	while (number_of_wall > 0) {
		wall_lenght = rand() % 5 + 1;//range 1 to 5
		int wall_position;// = rand() % 4;
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng3(rd()); // seed the generator
		std::uniform_int_distribution<> distr3(0, 100);
		wall_position = distr3(eng3);
		if (number_of_wall - wall_lenght < 0) {
			wall_lenght = number_of_wall;
			number_of_wall -= wall_lenght;
		}
		else {
			number_of_wall = number_of_wall - wall_lenght;
		}
		if (wall_position < 65) //right
		{
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 eng3(rd()); // seed the generator
			std::uniform_int_distribution<> distr3(0, map_col - 1);
			int col1 = distr3(eng3);

			srand(time(NULL));
			std::random_device rd2; // obtain a random number from hardware
			std::mt19937 eng4(rd2());
			std::uniform_int_distribution<> distr4(0, map_row - 1);
			int row1 = distr4(eng4);

			bool avaliable_for_wall = false;

			while (!avaliable_for_wall)
			{
				if ((wall_lenght + col1) < map_col)
				{
					avaliable_for_wall = true;
					for (int j = col1; j < (col1 + wall_lenght) && avaliable_for_wall; j++)
					{
						if (map[row1][j] != 0)
						{
							std::mt19937 eng3(rd()); // seed the generator
							std::uniform_int_distribution<> distr3(0, map_col - 1);
							col1 = distr3(eng3);
							srand(time(NULL));
							std::random_device rd2; // obtain a random number from hardware
							std::mt19937 eng4(rd2());
							std::uniform_int_distribution<> distr4(0, map_row - 1);
							row1 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					std::mt19937 eng3(rd()); // seed the generator
					std::uniform_int_distribution<> distr3(0, map_col - 1);
					col1 = distr3(eng3);
					srand(time(NULL));
					std::random_device rd2; // obtain a random number from hardware
					std::mt19937 eng4(rd2());
					std::uniform_int_distribution<> distr4(0, map_row - 1);
					row1 = distr4(eng4);
				}
			}
			for (size_t i = 0; i < wall_lenght; i++)
			{
				wall_positions[array_counter][0] = row1 * wall_width;
				wall_positions[array_counter][1] = (col1 + i)*wall_height;
				array_counter++;
			}
			for (int j = col1; j < (col1 + wall_lenght); j++)
			{
				map[row1][j] = 2;
			}
		}
		else if (wall_position >= 65) // up
		{
			std::mt19937 eng3(rd()); // seed the generator
			std::uniform_int_distribution<> distr3(0, map_col - 1);
			int col2 = distr3(eng3);
			srand(time(NULL));
			std::random_device rd2; // obtain a random number from hardware
			std::mt19937 eng4(rd2());
			std::uniform_int_distribution<> distr4(0, 20);
			int row2 = distr4(eng4);
			bool avaliable_for_wall = false;

			while (!avaliable_for_wall)
			{
				if ((wall_lenght + row2) < map_row)
				{
					avaliable_for_wall = true;
					for (int j = row2; j < (row2 + wall_lenght) && avaliable_for_wall; j++)
					{
						if (map[j][col2] != 0)
						{
							std::mt19937 eng3(rd()); // seed the generator
							std::uniform_int_distribution<> distr3(0, map_col - 1);
							col2 = distr3(eng3);
							srand(time(NULL));
							std::random_device rd2; // obtain a random number from hardware
							std::mt19937 eng4(rd2());
							std::uniform_int_distribution<> distr4(0, map_row - 1);
							row2 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					std::mt19937 eng3(rd()); // seed the generator
					std::uniform_int_distribution<> distr3(0, map_col - 1);
					col2 = distr3(eng3);
					srand(time(NULL));
					std::random_device rd2; // obtain a random number from hardware
					std::mt19937 eng4(rd2());
					std::uniform_int_distribution<> distr4(0, map_row - 1);
					row2 = distr4(eng4);
				}
			}
			for (size_t i = 0; i < wall_lenght; i++)
			{
				wall_positions[array_counter][0] = (row2 + i) * wall_width;
				wall_positions[array_counter][1] = col2 * wall_height;
				array_counter++;
			}
			for (int j = row2; j < (row2 + wall_lenght); j++)
			{
				map[j][col2] = 2;
			}
		}
	}

	return map;
}

//-----------------------------------------------------------------
// Draw functions
//-----------------------------------------------------------------
// Draws indicatiors in game play scene.
void DrawIndicators(HDC _hDC, Character* _character) {

	// Draw controller's name if robot
	if (_character->IsRobot()) {
		RECT bounds = _character->GetSprite()->GetPosition();
		bounds.bottom = bounds.top;
		bounds.top -= 25;
		std::string identifier;
		if (((Robot *)_character)->GetControlStatus() == AI) identifier = "AI";
		else if (((Robot *)_character)->GetControlStatus() == PLAYER_2) identifier = "P2";
		else identifier = "P1";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(identifier), 16, bounds);
	}

	// Draw health bar
	HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
	int totalWidth = 50;
	int totalHeight = 5;
	int greenWidth = round(_character->GetHealthPoint() * totalWidth / _character->GetMaxHealthPoint());
	int redWith = totalWidth - greenWidth;
	RECT pos = _character->GetSprite()->GetPosition();
	pos.top -= 40;
	pos.left -= totalWidth / 4;

	SelectObject(_hDC, greenBrush);
	Rectangle(_hDC, pos.left, pos.top, pos.left + greenWidth, pos.top + totalHeight);
	DeleteObject(greenBrush);

	SelectObject(_hDC, redBrush);
	Rectangle(_hDC, pos.left + greenWidth, pos.top, pos.left + greenWidth + redWith, pos.top + totalHeight);
	DeleteObject(redBrush);
	
}

// Draw robots on the select menu with their menu sprites.
// Displays name and descriptions on hover.
void DrawRobotsOnMenu(HDC _hDC)
{
	// 100 x 100 robot icons
	int iconWidth = 100;
	int offset = iconWidth + iconWidth;

	// Calculate to center robot icons
	POINT start;
	start.x = (RES_W / 2) - (iconWidth / 2) - ((robots.size() - 1) * iconWidth);
	start.y = 300;

	int i = 0; // to see which robot is clicked
	for (auto &Robot : robots) {
		Robot->GetMenuSprite()->SetPosition(start.x + (i*offset), start.y); // set new position
		bool robotIsSelected = std::find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end(); // check if robot is selected

		/*
		// If robot is hovered:
		//		not selected -> white focus
		//		selected	 -> red focus
		// If not hovered:
		//		not selected -> nothing
		//		selected	 -> green focus
		*/
		if (Robot->GetMenuHover()) {
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetName()), 36, { 0, 500, 1024, 550 });
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetDescription()), 36, { 224, 550, 800, 700 });

			HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
			if (robotIsSelected) {
				SelectObject(_hDC, hRedBrush);
			}

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - 5, Robot->GetMenuSprite()->GetPosition().top - 5,
				Robot->GetMenuSprite()->GetPosition().right + 20, Robot->GetMenuSprite()->GetPosition().bottom + 20);
			DeleteObject(hRedBrush);
		}
		else if (robotIsSelected) {
			HBRUSH hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(_hDC, hGreenBrush);

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - 5, Robot->GetMenuSprite()->GetPosition().top - 5,
				Robot->GetMenuSprite()->GetPosition().right + 20, Robot->GetMenuSprite()->GetPosition().bottom + 20);
			DeleteObject(hGreenBrush);
		}
		Robot->GetMenuSprite()->Draw(_hDC);
		i++;
	}

	// Display how many robots more should be selected
	if (playerCount * 2 > selectedRobotIndexes.size()) {
		std::string remaining = std::to_string(playerCount * 2 - selectedRobotIndexes.size()) + " remaining";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(remaining), 24, { 0, 250, 1024, 300 });
	}
}

void CreateButtons(HDC _hDC)
{
	// Main menu buttons 
	btnPlay = new Button(_hDC, (LPTSTR)TEXT("Play"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuMainButtons.push_back(btnPlay);
	btnExit = new Button(_hDC, (LPTSTR)TEXT("Exit"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuMainButtons.push_back(btnExit);

	// Play menu buttons 
	btn1Player = new Button(_hDC, (LPTSTR)TEXT("1 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuPlayersButtons.push_back(btn1Player);
	btn2Player = new Button(_hDC, (LPTSTR)TEXT("2 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuPlayersButtons.push_back(btn2Player);
	btnBackToMain = new Button(_hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 500);
	menuPlayersButtons.push_back(btnBackToMain);

	// Character select buttons
	btnReady = new Button(_hDC, (LPTSTR)TEXT("Begin"), (RES_W / 2) - (BTN_WIDTH / 2), 650);
	menuRobotsButtons.push_back(btnReady);
	btnBackToPlayers = new Button(_hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 700);
	menuRobotsButtons.push_back(btnBackToPlayers);
}

//-----------------------------------------------------------------
// Menu functions
//-----------------------------------------------------------------

// Handles click on buttons in menu.
// Scene changes and character selections are also here.
void HandleMenuButtonClick(int _x, int _y)
{
	int i = 0; // to retrieve which robot is clicked

	switch (currentScene) {
	case MENU_MAIN:
		// Play clicked
		if (btnPlay->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
		}

		// Exit clicked
		if (btnExit->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			GameEnd();
		}
		break;
	case MENU_SELECT_PLAYERS:
		// Back to main menu clicked
		if (btnBackToMain->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_MAIN;
		}

		// 1 - Player selected
		if (btn1Player->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			playerCount = 1;
			currentScene = MENU_SELECT_ROBOTS;
		}

		// 2 - Player selected
		if (btn2Player->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			playerCount = 2;
			currentScene = MENU_SELECT_ROBOTS;
		}
		break;
	case MENU_SELECT_ROBOTS:
		// Button click - Ready
		if (btnReady->GetSprite()->IsPointInside(_x, _y) && playerCount * 2 == selectedRobotIndexes.size()) {
			InitializeLevel();
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = GAME_PLAY;
		}

		// Button click - Back
		if (btnBackToPlayers->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
		}

		// Robot select
		for (auto &Robot : robots) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				bool robotIsSelected = std::find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end();

				// Add to selected if not selected, erase otherwise
				if (!robotIsSelected) {
					PlaySound((LPCWSTR)IDW_MENU_SELECT, hInstance, SND_ASYNC | SND_RESOURCE);
					selectedRobotIndexes.push_back(i);
				}
				else {
					selectedRobotIndexes.erase(std::remove(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i), selectedRobotIndexes.end());
				}
			}
			i++;
		}
		break;
	default:
		break;
	}
}

// Handles hovers on buttons in menu.
// It is visual rather than functional.
void HandleMenuButtonHover(int _x, int _y)
{
	switch (currentScene) {
	case MENU_MAIN:
		for (auto &Button : menuMainButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}
		break;
	case MENU_SELECT_PLAYERS:
		for (auto &Button : menuPlayersButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}
		break;
	case MENU_SELECT_ROBOTS:
		for (auto &Button : menuRobotsButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}

		for (auto &Robot : robots) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				Robot->SetMenuHover(true);
			}
			else {
				Robot->SetMenuHover(false);
			}
		}
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------
// Utility
//-----------------------------------------------------------------

POINT ScreenRectToArrayPoint(RECT _rect) {
	POINT pt;
	pt.x = floor(_rect.left / 32);
	pt.y = floor(_rect.top / 32);
	return pt;
}

POINT ScreenToArrayPoint(POINT _pt) {
	_pt.x = floor(_pt.x / 32);
	_pt.y = floor(_pt.y / 32);
	return _pt;
}

POINT RectToPoint(RECT rect) {
	return { rect.left, rect.top };
}
// Converts std::string to LPWSTR.
// Neccessary for DRAW TEXT.
LPWSTR ConvertString(const std::string& instr)
{
	// Assumes std::string is encoded in the current Windows ANSI codepage
	int bufferlen = ::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), NULL, 0);

	if (bufferlen == 0)
	{
		// Something went wrong. Perhaps, check GetLastError() and log.
		return 0;
	}

	// Allocate new LPWSTR - must deallocate it later
	LPWSTR widestr = new WCHAR[bufferlen + 1];

	::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), widestr, bufferlen);

	// Ensure wide string is null terminated
	widestr[bufferlen] = 0;

	// Do something with widestr
	return widestr;
	//delete[] widestr;
}

// Move the character to see if it will collide with an obstacle.
// Separate check for x and y.
POINT CheckCollisionBeforeMoving(Character *robot, POINT velocity) {
	robot->GetSprite()->SetPosition(robot->GetSprite()->GetPosition().left + velocity.x, robot->GetSprite()->GetPosition().top);
	BOOL collisionX = game->CheckMoveCollision(robot->GetSprite());
	// Revert
	robot->GetSprite()->SetPosition(robot->GetSprite()->GetPosition().left - velocity.x, robot->GetSprite()->GetPosition().top);

	// Test for Y
	robot->GetSprite()->SetPosition(robot->GetSprite()->GetPosition().left, robot->GetSprite()->GetPosition().top + velocity.y);
	BOOL collisionY = game->CheckMoveCollision(robot->GetSprite());
	// Revert
	robot->GetSprite()->SetPosition(robot->GetSprite()->GetPosition().left, robot->GetSprite()->GetPosition().top - velocity.y);

	if (collisionX) {
		velocity.x = 0;
	}
	if (collisionY) {
		velocity.y = 0;
	}
	return velocity;
}

// Compare POINT structs, true if equal
BOOL CmpPoint(POINT p1, POINT p2) {
	if (p1.x == p2.x && p1.y == p2.y) {
		return true;
	}
	return false;
}

// Get Manhattan distance between two points
int GetDistance(POINT p1, POINT p2) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	return dx + dy;
}

// Print path
void PrintPath(stack<POINT> _path) {
	stack<POINT> path = _path;
	while (!path.empty()) {
		POINT p = (POINT)path.top();
		wchar_t textBuffer[20] = { 0 };
		swprintf(textBuffer, 20, L"%d,%d\n", p.x, p.y);
		OutputDebugString(textBuffer);
		path.pop();
	}
}

// Print path on screen
void PrintPathOnScreen(HDC _hDC, stack<POINT> _path) {
	POINT curPos;
	RECT bounds;
	while (!_path.empty()) {
		curPos = _path.top();
		_path.pop();
		bounds = {curPos.x, curPos.y, 16+(curPos.x), 16+(curPos.y)};
		game->PrintText(_hDC, ConvertString({ 'P' }), 16, bounds);
	}
}

// Breadth first path finding
stack<POINT> FindPathBFS(POINT _src, POINT _dst, Map _currentMap) {
	int cols = 32;
	int rows = 24;
	Map map = currentMap;
	queue<Coordinate*> alt;
	Coordinate *c, *ct;
	stack<POINT> path;

	alt.push(new Coordinate(_src.x, _src.y));

	while (!alt.empty()) {
		c = alt.front();
		alt.pop();

		if (map[c->y][c->x] != 0 && map[c->y][c->x] != 3) continue;

		map[c->y][c->x] = 99; // mark as visited

		if (CoordinateDistance(c, new Coordinate(_dst.x, _dst.y)) <= 1) {
			int i = 0;
			while (c->prev != NULL) {
				path.push(c->screenPos);
				c = c->prev;
				i++;
			}
			return path;
		}

		if (c->x + 1 < cols && map[c->y][c->x + 1] == 0) alt.push(new Coordinate(c->x + 1, c->y, c->dist + 1, c));
		if (c->x - 1 >= 0 && map[c->y][c->x - 1] == 0) alt.push(new Coordinate(c->x - 1, c->y, c->dist + 1, c));
		if (c->y + 1 < rows && map[c->y + 1][c->x] == 0) alt.push(new Coordinate(c->x, c->y + 1, c->dist + 1, c));
		if (c->y - 1 >= 0 && map[c->y - 1][c->x] == 0) alt.push(new Coordinate(c->x, c->y - 1, c->dist + 1, c));
	}

	return path; // no path;
};

// Manhattan distance of Coordinate class objects
int CoordinateDistance(Coordinate *c1, Coordinate *c2) {
	int dx, dy;

	dx = abs(c1->x - c2->x);
	dy = abs(c1->y - c2->y);
	return dx + dy;
}

stack<POINT> DemonRoam(POINT robotposition, POINT baselocation, Map currentmap) {
	/*int robotx= robot.GetMapPosition().x;
	int roboty = robot.GetMapPosition().y;*/
	const int range = 3;
	int size = 2 * range + 1;
	//int arr_x[2 * range + 1];
	//int arr_y[2 * range + 1];
	vector<int> arr_x;
	vector<int> arr_y;
	int x[] = { -3,-2,-1,0,1,2,3 };
	for (size_t i = 0; i < 7; i++)
	{
		arr_x.push_back(x[i]);
		arr_y.push_back(x[i]);
	}
	bool finded_avaliable = false;
	int target_y;
	int target_x;

	while (!finded_avaliable)
	{
		std::random_device rd1; // obtain a random number from hardware
		std::mt19937 eng1(rd1()); // seed the generator
		std::uniform_int_distribution<> distr1(0, size - 1);
		target_x = distr1(eng1);
		target_x = arr_x[target_x] + robotposition.x;
		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 eng2(rd2()); // seed the generator
		std::uniform_int_distribution<> distr2(0, size - 1);
		target_y = distr2(eng2);
		target_y = arr_y[target_y] + robotposition.y;
		if (currentmap[target_y][target_x] == 0)
		{
			finded_avaliable = true;
		}
	}
	POINT targetposition;
	targetposition.x = target_x;
	targetposition.y = target_y;

	stack<POINT> pf = FindPathBFS(robotposition, targetposition, currentmap);
	return pf;
}