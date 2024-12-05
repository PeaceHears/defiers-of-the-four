//-----------------------------------------------------------------
// Defiers of the Four Application
// C++ Source - DotF.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DotF.h"

#include "client.h"
#include <iostream>

#define ALLY_BULLET_DAMAGE 10
#define ENEMY_BULLET_DAMAGE 15
#define FIRE_RANGE 4
#define MAX_ACTIVE_DEMON_PER_BASE 2

#define UI_TEXT_COLOR RGB(149, 133, 133)
#define UI_BG_COLOR RGB(15, 15, 15)

//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE _hInstance, const string& ipAddress)
{
	serverIPAddress = ipAddress;

	// Create the game engine
	game = new GameEngine(_hInstance, (LPTSTR)TEXT("Defiers of the Four"),
		(LPTSTR)TEXT("Defiers of the Four"), (WORD)IDI_DOTF, (WORD)IDI_DOTF_SM, 1200, 768);
	if (game == NULL)
		return FALSE;
	
	// Set the frame rate
	game->SetFrameRate(60);

	// Store the instance handle
	hInstance = _hInstance;

	// Configure game variables
	currentScene = MENU_MAIN;
	currentMapIdx = 0;
	playerCount = -1;
	currentScore = 0;
	time(&lastHealTime);

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
	nextMapSwitchDelay = 0;
	fowSwitchDelay = 0;
	fow = true;
	ResetFogMap();

	// Load bitmaps and sprites
	bmMenuBackground = new Bitmap(hDC, 1200, 768, RGB(22, 15, 28));
	bmMenuBackground2 = new Bitmap(hDC, 176, 768, UI_BG_COLOR);
	bmMenuTitle = new Bitmap(hDC, IDB_MENU_TITLE, hInstance);
	bmMenuStar = new Bitmap(hDC, IDB_MENU_STAR, hInstance);
	bmBlackRect = new Bitmap(hDC, 32, 32);
	bmDarkRect = new Bitmap(hDC, 32, 32, RGB(12, 5, 18));
	bmBullet = new Bitmap(hDC, IDB_BULLET, hInstance);
	bmDemonBullet = new Bitmap(hDC, IDB_DEMON_BULLET, hInstance);
	
	bmWall1 = new Bitmap(hDC, IDB_WALL_1, hInstance);
	bmWall2 = new Bitmap(hDC, IDB_WALL_2, hInstance);
	bmWall3 = new Bitmap(hDC, IDB_WALL_3, hInstance);
	bmEnemyBase = new Bitmap(hDC, IDB_ENEMY_BASE, hInstance);
	bmExplosion = new Bitmap(hDC, IDB_EXPLOSION, hInstance);

	bmDemon1 = new Bitmap(hDC, IDB_DEMON_1, hInstance);
	bmDemon1Explosion = new Bitmap(hDC, IDB_DEMON_1_EXPLODE, hInstance);
	bmBoss1 = new Bitmap(hDC, IDB_BOSS_1, hInstance);

	bmRobotMenu = new Bitmap(hDC, IDB_MENU_ROBOT, hInstance); // 100x100
	bmMonster = new Bitmap(hDC, IDB_MONSTER, hInstance); // 32x32

	bmRobot1Walk = new Bitmap(hDC, IDB_ROBOT_1_WALK, hInstance);

	// Create initial game elements
	CreateButtons(hDC);
	CreateRobots(hDC);
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
		game->PrintText(_hDC, (LPTSTR)TEXT("Defiers of the Four"), 96, { 0, 150, RES_W, 300 });
		for (auto &Button : menuMainButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_PLAYERS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Game Type"), 64, { 0, 150, RES_W, 300 });
		for (auto &Button : menuPlayersButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_ROBOTS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Select Characters"), 64, { 0, 150, RES_W, 300 });

		// Draw robots
		DrawRobotsOnMenu(_hDC);
		// Select menu buttons
		btnBackToPlayers->Draw(_hDC);
		if (selectedRobotIndexes.size() == playerCount * 2) {
			btnReady->Draw(_hDC);
		}
		break;
	case MATCHMAKING:
		bmMenuBackground->Draw(_hDC, 0, 0);

		game->PrintText(_hDC, (LPTSTR)TEXT("MATCHMAKING \n Your server IP:\n"), 64, { 0, 150, RES_W, 300 });
		game->PrintText(_hDC, ConvertString(serverIPAddress), 64, { 0, 300, RES_W, 450 });

		btnBackToPlayers->Draw(_hDC);
		btnStart->Draw(_hDC);

		break;
	case GAME_PLAY:
	{
		bmMenuBackground->Draw(_hDC, 0, 0);
		bmMenuBackground2->Draw(_hDC, 1024, 0);

		// draw slightly darker bg for discovered but not visible parts
		if (fow) {
			for (int i = 0; i < fogMap.size(); i++) {
				for (int j = 0; j < fogMap[i].size(); j++) {
					if (fogMap[i][j] == FMS_DISCOVERED) {
						bmDarkRect->Draw(_hDC, j * 32, i * 32);
					}
				}
			}
		}

		game->DrawSprites(_hDC);
		DrawRightMenu(_hDC);

		for (auto &Robot : inGameRobots) {
			DrawIndicators(_hDC, Robot);
		}

		for (auto &DemonBase : demonBases) {
			for (auto &Demon : DemonBase->GetCurrentDemons()) {
				DrawIndicators(_hDC, Demon);
			}
		}

		if (demonBoss1 != NULL) {
			DrawIndicators(_hDC, demonBoss1);
		}

		// Hide undiscovered map
		if (fow) {
			for (int i = 0; i < fogMap.size(); i++) {
				for (int j = 0; j < fogMap[i].size(); j++) {
					if (fogMap[i][j] == FMS_UNDISCOVERED) {
						bmBlackRect->Draw(_hDC, j * 32, i * 32);
					}
				}
			}
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

	if (currentScene == GAME_PLAY) 
	{
		UpdateCharacters();	
		UpdateBases();
		UpdateObstacles();

		UpdateClientData();
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
	// Ally bullet vs Character
	if (_spriteHitter->GetSpriteType() == ST_ALLY_BULLET && _spriteHittee->GetSpriteType() == ST_CHARACTER) {
		if (_spriteHittee->GetCharacter() != NULL) {
			_spriteHittee->GetCharacter()->TakeHit(ALLY_BULLET_DAMAGE);

			if (_spriteHittee->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHittee->GetCharacter());
			}
		}
		_spriteHitter->Kill();
	}
	else if (_spriteHitter->GetSpriteType() == ST_CHARACTER && _spriteHittee->GetSpriteType() == ST_ALLY_BULLET) {
		if (_spriteHitter->GetCharacter() != NULL) {
			_spriteHitter->GetCharacter()->TakeHit(ALLY_BULLET_DAMAGE);

			if (_spriteHitter->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHitter->GetCharacter());
			}
		}
		_spriteHittee->Kill();
	}
	// Enemy bullet vs Demon
	else if (_spriteHitter->GetSpriteType() == ST_ENEMY_BULLET && (_spriteHittee->GetSpriteType() == ST_DEMON)) {
		_spriteHitter->Kill();
	}
	else if (_spriteHitter->GetSpriteType() == ST_DEMON && (_spriteHittee->GetSpriteType() == ST_ENEMY_BULLET)) {
		_spriteHittee->Kill();
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
	else if (_spriteHitter->GetSpriteType() == ST_DEMON && _spriteHittee->GetSpriteType() == ST_ALLY_BULLET) {
		if (_spriteHitter->GetCharacter() != NULL) {
			_spriteHitter->GetCharacter()->TakeHit(ALLY_BULLET_DAMAGE);

			if (_spriteHitter->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHitter->GetCharacter());
			}
		}
		_spriteHittee->Kill();
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
	else if (_spriteHitter->GetSpriteType() == ST_CHARACTER && _spriteHittee->GetSpriteType() == ST_ENEMY_BULLET) {
		if (_spriteHitter->GetCharacter() != NULL) {
			_spriteHitter->GetCharacter()->TakeHit(ENEMY_BULLET_DAMAGE);

			if (_spriteHitter->GetCharacter()->GetHealthPoint() <= 0) {
				KillCharacter(_spriteHitter->GetCharacter());
			}
		}
		_spriteHittee->Kill();
	}
	// Bullet vs base
	else if ((_spriteHitter->GetSpriteType() == ST_ALLY_BULLET || _spriteHitter->GetSpriteType() == ST_ENEMY_BULLET) && (_spriteHittee->GetSpriteType() == ST_BASE)) {
		_spriteHitter->Kill();
	}
	else if (_spriteHitter->GetSpriteType() == ST_BASE && (_spriteHittee->GetSpriteType() == ST_ALLY_BULLET || _spriteHittee->GetSpriteType() == ST_ENEMY_BULLET)) {
		_spriteHittee->Kill();
	}
	// Bullet vs wall
	else if ((_spriteHitter->GetSpriteType() == ST_ALLY_BULLET || _spriteHitter->GetSpriteType() == ST_ENEMY_BULLET)  && _spriteHittee->GetSpriteType() == ST_WALL) {
		WallSprite* wallSprite = (WallSprite*)_spriteHittee;
		wallSprite->TakeHit(_spriteHitter->GetSpriteType() == ST_ALLY_BULLET ? ALLY_BULLET_DAMAGE:ENEMY_BULLET_DAMAGE);
		int perc = wallSprite->GetPercHealth();

		if (wallSprite->GetHealth() <= 0) {
			// Destroy wall
			maps[currentMapIdx][wallSprite->GetPosition().top / 32][wallSprite->GetPosition().left / 32] = 0;
			Sprite *explosion = new Sprite(bmExplosion);
			explosion->SetPosition(wallSprite->GetPosition().left, wallSprite->GetPosition().top);
			explosion->SetNumFrames(8, true);
			game->AddSprite(explosion);
			wallSprite->Kill();
		}
		else if (wallSprite->GetPercHealth() < 40) {
			// Replace with damaged wall
			wallSprite->SetBitmap(bmWall3);
		}
		else if (wallSprite->GetPercHealth() < 80) {
			// Replace with damaged wall
			wallSprite->SetBitmap(bmWall2);
		}

		_spriteHitter->Kill();
	}
	else if (_spriteHitter->GetSpriteType() == ST_WALL && (_spriteHittee->GetSpriteType() == ST_ALLY_BULLET || _spriteHittee->GetSpriteType() == ST_ENEMY_BULLET)) {
		WallSprite* wallSprite = (WallSprite*)_spriteHitter;
		bool atStroke = false;
		if (_spriteHittee->GetCharacter()->IsRobot())
		{
			Robot *hitter = (Robot*)(_spriteHittee->GetCharacter());
			if (hitter->GetRobotType() == R_CONSTROBOT) {
				if (hitter->IsAbilityActive(1))
				{
					atStroke = true;
					hitter->SetAbilityActive(1, false);
				}

			}

		}
		wallSprite->TakeHit(_spriteHittee->GetSpriteType() == ST_ALLY_BULLET ? ALLY_BULLET_DAMAGE : ENEMY_BULLET_DAMAGE);
		int perc = wallSprite->GetPercHealth();

		if (wallSprite->GetHealth() <= 0 || atStroke) {
			// Destroy wall
			maps[currentMapIdx][wallSprite->GetPosition().top / 32][wallSprite->GetPosition().left / 32] = 0;
			Sprite *explosion = new Sprite(bmExplosion);
			explosion->SetPosition(wallSprite->GetPosition().left, wallSprite->GetPosition().top);
			explosion->SetNumFrames(8, true);
			game->AddSprite(explosion);
			wallSprite->Kill();
		}
		else if (wallSprite->GetPercHealth() < 40) {
			// Replace with damaged wall
			wallSprite->SetBitmap(bmWall3);
		}
		else if (wallSprite->GetPercHealth() < 80) {
			// Replace with damaged wall
			wallSprite->SetBitmap(bmWall2);
		}

		_spriteHittee->Kill();
	}
	// Bullet vs bullet
	else if ((_spriteHitter->GetSpriteType() == ST_ENEMY_BULLET && _spriteHittee->GetSpriteType() == ST_ALLY_BULLET) ||
			(_spriteHitter->GetSpriteType() == ST_ALLY_BULLET && _spriteHittee->GetSpriteType() == ST_ENEMY_BULLET) ||
			(_spriteHitter->GetSpriteType() == ST_ALLY_BULLET && _spriteHittee->GetSpriteType() == ST_ALLY_BULLET) ||
			(_spriteHitter->GetSpriteType() == ST_ENEMY_BULLET && _spriteHittee->GetSpriteType() == ST_ENEMY_BULLET)) {
			_spriteHittee->Kill();
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
		Player1Controls(); // there will be always player 1
		if (playerCount == 2) {
			Player2Controls(); // if 2 players
		}
		
		// Load next world
		if (++nextMapSwitchDelay > 10 && GetAsyncKeyState('L') < 0) {
			LoadNextLevel();

			nextMapSwitchDelay = 0;
		}

		// Switch fow
		if (++fowSwitchDelay > 10 && GetAsyncKeyState('F') < 0) {
			SwitchFow();

			fowSwitchDelay = 0;
		}
	}
}

void MouseButtonDown(int _x, int _y, BOOL _isLeftClick)
{
}

void MouseButtonUp(int _x, int _y, BOOL _isLeftClick)
{
	if (_isLeftClick) {
		if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || 
			currentScene == MENU_SELECT_ROBOTS || currentScene == MATCHMAKING) {
			HandleMenuButtonClick(_x, _y);
		}

	}
}

void MouseMove(int _x, int _y)
{
	if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || 
		currentScene == MENU_SELECT_ROBOTS || currentScene == MATCHMAKING) {
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
	int range = 3; // TODO; class variable

	currentMap = maps[currentMapIdx];
	if (fow) ClearFogMap();
	CheckAbilities();
	HealWithPoints();

	POINT pos;	

	// flatten demons
	vector<Demon*> demons;
	for (auto &DemonBase : demonBases) {
		for (auto &Demon : DemonBase->GetCurrentDemons()) {
			demons.push_back(Demon);

			// hide sprites initially, then we will show them if they are nearby
			if (fow) {
				Demon->GetSprite()->SetHidden(true);
			}
		}
	}

	// Update robot map vector positions, also fire delay
	for (auto &Robot : inGameRobots) {
		pos = ScreenRectToArrayPoint(Robot->GetSprite()->GetPosition());
		Robot->SetMapPosition(pos);
		Robot->UpdateStatusMessages();
		currentMap[pos.y][pos.x] = 3;
		Robot->SetCurFireDelay(min(Robot->GetCurFireDelay()+1, Robot->GetFireDelay()));
	}

	// Update demon map vector positions, also fire delay
	for (auto &DemonBase : demonBases) {
		for (auto &Demon : DemonBase->GetCurrentDemons()) {
			pos = ScreenRectToArrayPoint(Demon->GetSprite()->GetPosition());
			Demon->SetMapPosition(pos);
			currentMap[pos.y][pos.x] = 4;
			Demon->SetCurFireDelay(min(Demon->GetCurFireDelay() + 1, Demon->GetFireDelay()));
		}
	}

	// Temp values for this scope
	vector<Character*> nearbyDemons;
	vector<Character*> nearbyRobots;
	Robot* r;
	Demon* d;

	// Set nearby targets and allies for robots
	for (auto &Robot : inGameRobots) {
		nearbyDemons.clear(); // reset
		nearbyRobots.clear(); // reset

		pos = ScreenRectToArrayPoint(Robot->GetSprite()->GetPosition());
		Robot->SetMapPosition(pos);
		currentMap[pos.y][pos.x] = 3;

		for (int i = -range; i < range; i++) {
			for (int j = -range; j < range; j++) {
				int checkX = Robot->GetMapPosition().x + j;
				int checkY = Robot->GetMapPosition().y + i;
				if (checkX < 0 || checkY < 0 || checkX >= 32 || checkY >= 24) continue; // bounds
				
				fogMap[checkY][checkX] = FMS_VISIBLE;
				if (i == 0 && j == 0) continue; // self - dont add to nearby

				// Robot
				if (currentMap[checkY][checkX] == 3) {
					r = GetRobotAtMapPosition(checkX, checkY);
					if (r != NULL) nearbyRobots.push_back(r);
				}
				// Demon
				else if (currentMap[checkY][checkX] == 4) {
					d = GetDemonAtMapPosition(checkX, checkY);
					if (d != NULL) nearbyDemons.push_back(d);

					// hide sprites initially, then we will show them if they are nearby
					if (fow) {
						d->GetSprite()->SetHidden(false);
					}
				}

			}
		}

		Robot->SetNearbyDemons(nearbyDemons);
		Robot->SetNearbyRobots(nearbyRobots);

		// reset stats to default for passive aura later on
		if (Robot->GetRobotType() != R_CAPTAIN) Robot->ResetStatsToDefault();

		// Boss 1 fire to these robots
		if (demonBoss1 != NULL && rand() % 2) {
			demonBoss1->SetFireDirection(Robot);
			Fire(demonBoss1);
		}
	}

	// Set nearby targets and allies for demons
	for (auto &Demon : demons) {
		nearbyDemons.clear(); // reset
		nearbyRobots.clear(); // reset

		pos = ScreenRectToArrayPoint(Demon->GetSprite()->GetPosition());
		Demon->SetMapPosition(pos);
		currentMap[pos.y][pos.x] = 3;

		for (int i = -range; i < range; i++) {
			for (int j = -range; j < range; j++) {
				int checkX = Demon->GetMapPosition().x + j;
				int checkY = Demon->GetMapPosition().y + i;

				if (i == 0 && j == 0) continue; // self
				if (checkX < 0 || checkY < 0 || checkX >= 32 || checkY >= 24) continue; // bounds

				// Robot
				if (currentMap[checkY][checkX] == 3) {
					r = GetRobotAtMapPosition(checkX, checkY);
					if (r != NULL) nearbyRobots.push_back(r);
				}
				// Demon
				else if (currentMap[checkY][checkX] == 4) {
					d = GetDemonAtMapPosition(checkX, checkY);
					if (d != NULL) nearbyDemons.push_back(d);
				}
			}
		}

		Demon->SetNearbyDemons(nearbyDemons);
		Demon->SetNearbyRobots(nearbyRobots);
	}

	// Robot actions
	bool robotFlag = false; // set if P1 is followed, so that other AI will follow P2
	Character *target;
	for (auto &robot : inGameRobots) {
		// AI controlled
		if (robot->GetControlStatus() == CS_AI) {
			// no enemy is around
			if (robot->GetNearbyDemons().size() == 0) {
				// TODO; maybe use skill too?
				robot->SetTask(AT_FOLLOW);

				if (!robotFlag) {
					target = GetPlayersRobot(1);
					robotFlag = true;
				}
				else {
					target = GetPlayersRobot(2);				
				}
				robot->SetTarget(target);
				
				if (robot->IsReady()) {
					robot->SetPath(FollowRobot(robot->GetMapPosition(), target->GetMapPosition(), currentMap));
				}

			}
			// there are enemies around
			else {
				robot->SetTask(AT_ATTACK);
				target = robot->GetNearbyDemons().front();

				/*if (robot->GetStatus().empty()) {
					int rnd = rand() % RobotDemonSpottedMessages->length();
					swprintf(textBuffer, 15, L"%d", rnd);
					robot->AddStatusMessage(RobotDemonSpottedMessages[rnd], time(&now) + 2, RGB(255, 255, 255));
				}*/
				

				int roll = rand() % 5;
				// attack
				if (roll == 0) {
					robot->SetFireDirection(target);
					Fire(robot);
				}
				// evade
				else if (roll == 1) {
					
				}	
			}
			robot->Update();
		}
		// player controlled
		else {
		}

		
		if (robot->GetRobotType() == R_CAPTAIN) {
			// Robo cap passive to boost nearby ally stats
			for (auto &nearbyRobot : robot->GetNearbyRobots()) {
				/// if (((Robot*)nearbyRobot)->GetRobotType() != R_CAPTAIN)
					 nearbyRobot->BoostStats(20);
			}

			// Robo cap check self ability 2 expiration
			time(&now);
			if (!robot->IsAbilityReady(1)) {
				if (difftime(now, robot->GetAbilityUsedTime(1)) >= robot->GetAbilityDuration(1)) {
					// expired
					((R_Captain*)robot)->Unbreakable(false);
				}
				else {
					// active
					
				}
				
			}
		}

		

	}

	// Demon actions
	int i = 0;
	for (auto &Demon : demons) {
		// no enemy is around
		/*if (Demon->GetNearbyRobots().size() == 0) {
			// TODO; maybe use skill too?
			Demon->SetTask(AT_IDLE);

			if (Demon->IsReady() && (rand() % 10) == 0) {
				if (Demon->GetFirstCreated()) {
					Demon->SetFirstCreated(false);
					Demon->SetPath(DemonRoam(Demon, Demon->GetBase()->GetMapPosition(), Demon->GetBase()->GetMapPosition(), currentMap));
					
				}
				else {
					Demon->SetPath(DemonRoam(Demon, Demon->GetMapPosition(), Demon->GetBase()->GetMapPosition(), currentMap));
				}
			}
		}
		// there are enemies around
		else {
			Demon->SetTask(AT_ATTACK);
			target = Demon->GetNearbyRobots().front();

			int roll = rand() % 5;
			// attack
			if (roll == 0) {
				Demon->SetFireDirection(target);
				Fire(Demon);
			}
			// evade
			else if (roll == 1) {

			}
		}*/

		Demon->Situations(currentMap, demonBases, game, bmDemonBullet, hInstance);
		Demon->Update();
	}

}

// Update demon bases
void UpdateBases() {
	int numDemons;
	for (auto &DemonBase : demonBases) {
		numDemons = DemonBase->GetCurrentDemons().size();
		if (DemonBase->GetSpawnLimit() == 0) {
			if (numDemons == 0) {
				demonBases.erase(remove(demonBases.begin(), demonBases.end(), DemonBase), demonBases.end());
				//DemonBase->GetSprite()->Kill();
				//delete DemonBase;
				continue;
			}
		}	
		else if (numDemons < MAX_ACTIVE_DEMON_PER_BASE && (rand() % 100 < MAX_ACTIVE_DEMON_PER_BASE - numDemons)) {
			if (DemonBase != NULL) {
				AddDemon(DemonBase);
			}
			
		}
	}
}

// Update obstacles
void UpdateObstacles() {

}

void UpdateClientData()
{
	for (const auto& inGameRobot : inGameRobots) 
	{
		const auto& pos = ScreenRectToArrayPoint(inGameRobot->GetSprite()->GetPosition());

		if (inGameRobot->GetControlStatus() == ControlStatus::CS_AI)
		{
			inGameData.allyPosition = pos;
		}
		else
		{
			inGameData.playerPosition = pos;
		}

		game->GetClient().setInGameData(inGameData);
	}
}

// Creates all robots in the game at the start.
void CreateRobots(HDC _hDC) {
	Sprite *robotSprite, *menuSprite;
	POINT pos;
	pos.x = 250;
	pos.y = 250;

	// 1 
	robotSprite = new Sprite(bmRobot1Walk);
	robotSprite->SetNumFrames(11);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot1 = new Robot({ "Robot 1" }, { "Robot 1 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14, CS_AI);
	robotSprite->SetCharacter(robot1);

	// 2
	robotSprite = new Sprite(bmRobot1Walk);
	robotSprite->SetNumFrames(11);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	R_Constrobot *robot2 = new R_Constrobot({ "Robot 2" }, { "Robot 2 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14);
	robotSprite->SetCharacter(robot2);

	// 3 - Wololo
	robotSprite = new Sprite(bmRobot1Walk);
	robotSprite->SetNumFrames(11);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	R_Wololo *robot3 = new R_Wololo({ "Robot 3" }, { "Robot 3 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14);
	robotSprite->SetCharacter(robot3);

	// 4 - Captain
	robotSprite = new Sprite(bmRobot1Walk);
	robotSprite->SetNumFrames(11);
	menuSprite = new Sprite(bmRobotMenu);
	robotSprite->SetSpriteType(ST_CHARACTER);
	R_Captain *robot4 = new R_Captain({ "Robot 4" }, { "Robot 4 Desc" }, robotSprite, menuSprite, 100, 8, pos, 14);
	robotSprite->SetCharacter(robot4);

	// Add to vector
	robots.push_back((Robot*)robot1);
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
	if (fireDirection.x != 0 || fireDirection.y != 0) {
		robot->SetFireDirection(fireDirection);
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}

	//velocity = CheckCollisionBeforeMoving(robot, velocity);
	robot->GetSprite()->SetVelocity(velocity);

	// Switch robots
	if (++dlPlayer1Switch > 30 && GetAsyncKeyState(VK_LSHIFT) < 0) {
		SwitchRobot(1);
		dlPlayer1Switch = 0;
	}

	// Fire
	//robot->SetCurFireDelay(robot->GetCurFireDelay() + 1);
	if (/*robot->GetCurFireDelay() > robot->GetFireDelay() &&*/ GetAsyncKeyState(VK_LCONTROL)) {
		Fire(robot);
		//robot->SetCurFireDelay(0);
	}

	// Abilities
	if (GetAsyncKeyState('Q') < 0) {
		//robot->UseSuperPower1(inGameRobots);
		if (robot->IsAbilityReady(0)) { 
			switch (robot->GetRobotType()) {
			case R_CONSTROBOT:
			{
				WallSprite *newwall = new WallSprite(bmWall1);
				newwall->SetSpriteType(ST_WALL);
				if (robot->GetFireDirection().x == 0 && robot->GetFireDirection().y < 0) //up
				{
					if (robot->GetMapPosition().y - 1 < 0)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y - 1][robot->GetMapPosition().x] != 0)break;
					newwall->SetPosition(robot->GetMapPosition().x * 32, (robot->GetMapPosition().y - 1) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y - 1][robot->GetMapPosition().x] = 2;
				}
				else if (robot->GetFireDirection().x < 0 && robot->GetFireDirection().y == 0) //left
				{
					if (robot->GetMapPosition().x - 1 < 0)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x - 1] != 0)break;
					newwall->SetPosition((robot->GetMapPosition().x - 1) * 32, (robot->GetMapPosition().y) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x - 1] = 2;
				}
				else if (robot->GetFireDirection().x == 0 && robot->GetFireDirection().y > 0) //down
				{
					if (robot->GetMapPosition().y + 2 >= 32)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y + 2][robot->GetMapPosition().x] != 0)break;
					newwall->SetPosition(robot->GetMapPosition().x * 32, (robot->GetMapPosition().y + 2) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y + 2][robot->GetMapPosition().x] = 2;
				}
				else if (robot->GetFireDirection().x > 0 && robot->GetFireDirection().y == 0) //right
				{
					if (robot->GetMapPosition().x + 2 >= 32)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x + 2] != 0)break;
					newwall->SetPosition((robot->GetMapPosition().x + 2) * 32, (robot->GetMapPosition().y) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x + 2] = 2;
				}
				game->AddSprite(newwall);

				((R_Constrobot*)robot)->UseAbility1();
				break;
			}
			default:
				robot->UseAbility1();
			}
		};
		
	}
	else if (GetAsyncKeyState('E') < 0) {
		//robot->UseSuperPower2(inGameRobots);
		if (robot->IsAbilityReady(1)) {
			switch (robot->GetRobotType()) {
			case R_WOLOLO:
				((R_Wololo*)robot)->UseAbility2(inGameRobots); // override func
				break;
			default:
				robot->UseAbility2();
			}
		}
	}
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
	if (fireDirection.x != 0 || fireDirection.y != 0) {
		robot->SetFireDirection(fireDirection);
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}

	//velocity = CheckCollisionBeforeMoving(robot, velocity);
	robot->GetSprite()->SetVelocity(velocity);

	// Switch robots
	if (++dlPlayer1Switch > 30 && GetAsyncKeyState(VK_NUMPAD0) < 0) {
		SwitchRobot(2);
		dlPlayer1Switch = 0;
	}

	// Fire
	//robot->SetCurFireDelay(robot->GetCurFireDelay() + 1);
	if (/*robot->GetCurFireDelay() > robot->GetFireDelay() &&*/ GetAsyncKeyState(VK_RETURN)) {
		Fire(robot);
		//robot->SetCurFireDelay(0);
	}

	// Abilities
	if (GetAsyncKeyState(GetAsyncKeyState(VK_NUMPAD1)) < 0) {
		//robot->UseSuperPower1(inGameRobots);
		if (robot->IsAbilityReady(0)) {
			switch (robot->GetRobotType()) {
			case R_CONSTROBOT:
			{
				WallSprite *newwall = new WallSprite(bmWall1);
				newwall->SetSpriteType(ST_WALL);
				if (robot->GetFireDirection().x == 0 && robot->GetFireDirection().y < 0) //up
				{
					if (robot->GetMapPosition().y - 1 < 0)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y - 1][robot->GetMapPosition().x] != 0)break;
					newwall->SetPosition(robot->GetMapPosition().x * 32, (robot->GetMapPosition().y - 1) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y - 1][robot->GetMapPosition().x] = 2;
				}
				else if (robot->GetFireDirection().x < 0 && robot->GetFireDirection().y == 0) //left
				{
					if (robot->GetMapPosition().x - 1 < 0)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x - 1] != 0)break;
					newwall->SetPosition((robot->GetMapPosition().x - 1) * 32, (robot->GetMapPosition().y) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x - 1] = 2;
				}
				else if (robot->GetFireDirection().x == 0 && robot->GetFireDirection().y > 0) //down
				{
					if (robot->GetMapPosition().y + 2 >= 32)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y + 2][robot->GetMapPosition().x] != 0)break;
					newwall->SetPosition(robot->GetMapPosition().x * 32, (robot->GetMapPosition().y + 2) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y + 2][robot->GetMapPosition().x] = 2;
				}
				else if (robot->GetFireDirection().x > 0 && robot->GetFireDirection().y == 0) //right
				{
					if (robot->GetMapPosition().x + 2 >= 32)break;
					if (maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x + 2] != 0)break;
					newwall->SetPosition((robot->GetMapPosition().x + 2) * 32, (robot->GetMapPosition().y) * 32);
					maps[currentMapIdx][robot->GetMapPosition().y][robot->GetMapPosition().x + 2] = 2;
				}
				game->AddSprite(newwall);

				((R_Constrobot*)robot)->UseAbility1();
				break;
			}
			default:
				robot->UseAbility1();
			}
		};

	}
	else if (GetAsyncKeyState(VK_NUMPAD2) < 0) {
		//robot->UseSuperPower2(inGameRobots);
		if (robot->IsAbilityReady(1)) {
			switch (robot->GetRobotType()) {
			case R_WOLOLO:
				((R_Wololo*)robot)->UseAbility2(inGameRobots); // override func
				break;
			default:
				robot->UseAbility2();
			}
		}
	}
}

// Returns the robot currently controlled by _player.
Robot* GetPlayersRobot(int _player) {
	switch (_player) {
	case 1:
		for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == CS_P1) {
				return Robot;
			}
		}
		return NULL;
	case 2:
		for (auto &Robot : inGameRobots) {
			
			if (Robot->GetControlStatus() == CS_P2) {
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
		if (inGameRobots.at(numSwitch)->GetControlStatus() == CS_AI) {
			setPlayer = numSwitch;
		}
		else if (_player == 1 && inGameRobots.at(numSwitch)->GetControlStatus() == CS_P1) {
			setAI = numSwitch;
		}
		else if (_player == 2 && inGameRobots.at(numSwitch)->GetControlStatus() == CS_P2) {
			setAI = numSwitch;
		}

		numSwitch++;
		if (numSwitch == inGameRobots.size()) {
			numSwitch = 0;
		}

		if (setAI != -1 && setPlayer != -1) {
			inGameRobots.at(setAI)->SetReady(true);
			inGameRobots.at(setAI)->ClearPath(); // prevent moving to last ai controlled pos
			inGameRobots.at(setAI)->SetControlStatus(CS_AI);
			inGameRobots.at(setAI)->GetSprite()->SetVelocity(0, 0);

			if (_player == 1) {
				inGameRobots.at(setPlayer)->SetControlStatus(CS_P1);
			}
			else {
				inGameRobots.at(setPlayer)->SetControlStatus(CS_P2);
			}
			return;
		}
	}
}

// Fire from a character
void Fire(Character *character) {
	if (character->GetCurFireDelay() < character->GetFireDelay()) return;
	character->SetCurFireDelay(0);

	if (character->GetFireDirection().x == 0 && character->GetFireDirection().y == 0) return;

	Sprite *bullet;
	if (character->IsRobot()) {
		PlaySound((LPCWSTR)IDW_ROBOT_FIRE, hInstance, SND_ASYNC | SND_RESOURCE);
		bullet = new Sprite(bmBullet);
		bullet->SetSpriteType(ST_ALLY_BULLET);
	}
	else {
		PlaySound((LPCWSTR)IDW_DEMON_FIRE, hInstance, SND_ASYNC | SND_RESOURCE);
		bullet = new Sprite(bmDemonBullet);
		bullet->SetSpriteType(ST_ENEMY_BULLET);
	}		

	bullet->SetBoundsAction(BA_DIE);
	bullet->SetPosition(character->GetSprite()->GetPosition().left + character->GetSprite()->GetWidth() / 2, character->GetSprite()->GetPosition().top + character->GetSprite()->GetHeight() / 2);
	bullet->SetVelocity(character->GetFireDirection());
	bullet->SetCharacter(character);

	// Get bullet out of the character first
	while (bullet->TestCollision(character->GetSprite())) {
		if (bullet->GetVelocity().x==0&&bullet->GetVelocity().y==0)
		{
			break;
		}
		bullet->SetPosition(bullet->GetPosition().left + character->GetFireDirection().x, bullet->GetPosition().top + character->GetFireDirection().y);
	}
	game->AddSprite(bullet);
}

Demon* AddDemon(DemonBase* _base) {
	// debug
	swprintf(textBuffer, 20, L"Adddemon start \n");
	OutputDebugString(textBuffer);

	POINT basePos = RectToPoint(_base->GetSprite()->GetPosition());
	POINT arrayPos = { basePos.x / 32, basePos.y / 32 };
	RECT bounds = { basePos.x - (32 * 3), basePos.y - (32 * 3), basePos.x + (32 * 4), basePos.y + (32 * 4) };
	basePos.x += 32;
	
	Sprite* demonSprite = new Sprite(bmDemon1);
	demonSprite->SetSpriteType(ST_DEMON);
	demonSprite->SetPosition(basePos);
	demonSprite->SetBounds(bounds);
	demonSprite->SetBoundsAction(BA_BOUNCE);
	game->AddSprite(demonSprite);
	Demon* demon = new Demon({ "Demon" }, { "Demon description" }, demonSprite, 45, 10, { arrayPos.x+1, arrayPos.y }, _base, 10);
	demon->SetBase(_base);
	demon->SetSpeed(3);
	demon->SetFireDelay(30);
	demonSprite->SetCharacter(demon);
	_base->AddDemonToVector(demon);
	_base->ReduceSpawnLimit(1);

	// debug
	swprintf(textBuffer, 20, L"Adddemon end \n");
	OutputDebugString(textBuffer);
	return demon;
}

void KillCharacter(Character* _character) {
	if (_character->IsRobot()) {
		Robot *robot = (Robot*)_character;

		// Switch before deleting, if controlled by character
		if (robot->GetControlStatus() == CS_P1) {
			if (inGameRobots.size() == 1) {
				currentScene = MENU_SELECT_PLAYERS;
				return;
			}
			SwitchRobot(1);
		}

		inGameRobots.erase(remove(inGameRobots.begin(), inGameRobots.end(), robot), inGameRobots.end());
		robot->GetSprite()->Kill();

		delete robot;
	}
	else {
		Demon *demon = (Demon*)_character;
		if (demon->GetDemonType() == D_DEMON) {
			Sprite *demonExplode = new Sprite(bmDemon1Explosion);
			demonExplode->SetNumFrames(9, true);
			demonExplode->SetPosition(demon->GetSprite()->GetPosition());
			demonExplode->SetZOrder(5);
			game->AddSprite(demonExplode);

			demon->GetBase()->RemoveDemon(demon);
			demon->GetSprite()->Kill();
			delete demon;
			currentScore += 5;
		}
		else {
			demon->GetSprite()->Kill();
			delete demonBoss1;
			currentScore += 100;
		}
		

		if (rand() % 2) {
			PlaySound((LPCWSTR)IDW_DEMON_DIE_1, hInstance, SND_ASYNC | SND_RESOURCE);
		}
		else {
			PlaySound((LPCWSTR)IDW_DEMON_DIE_2, hInstance, SND_ASYNC | SND_RESOURCE);
		}
		
	}
}

stack<POINT> DemonRoam(Demon *_demon, POINT robotposition, POINT baselocation, Map currentmap) {
	/*int robotx= robot.GetMapPosition().x;
	int roboty = robot.GetMapPosition().y;*/
	const int range = 3;
	vector<int> arr_x;
	vector<int> arr_y;

	swprintf(textBuffer, 20, L"Demon roam start\n");
	OutputDebugString(textBuffer);
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
		random_device rd1; // obtain a random number from hardware
		mt19937 eng1(rd1()); // seed the generator
		uniform_int_distribution<> distr1(0, arr_x.size() - 2);
		target_x = distr1(eng1);
		target_x = arr_x[target_x] + baselocation.x;
		random_device rd2; // obtain a random number from hardware
		mt19937 eng2(rd2()); // seed the generator
		uniform_int_distribution<> distr2(0, arr_x.size() - 2);
		target_y = distr2(eng2);
		target_y = arr_y[target_y] + baselocation.y;

		swprintf(textBuffer, 20, L"Found %d, %d and sizes are %d, %d\n", target_y, target_x, currentMap.size(), currentMap[0].size());
		OutputDebugString(textBuffer);
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


	swprintf(textBuffer, 20, L"Demon roam end\n");
	OutputDebugString(textBuffer);
	return FindPathBFS(robotposition, targetposition, currentmap);
}

stack <POINT> FollowRobot(POINT src, POINT dst, Map _map) {
	vector<int> arr_x;
	vector<int> arr_y;
	int x[] = { 2,1,0 };
	for (size_t i = 0; i < 3; i++)
	{
		arr_x.push_back(x[i]);
		arr_y.push_back(x[i]);
	}


	bool isFound = false;
	while (!isFound)
	{
		for (size_t i = 0; i < arr_x.size() && !isFound; i++)
		{
			for (size_t j = 0; j < arr_y.size() && !isFound; j++)
			{
				if (!(i == 0 && j == 0)) {

					if (dst.x + arr_x[i] < 32 && dst.x + arr_x[i] >= 0 && dst.y + arr_y[j] < 24 && dst.y + arr_y[j] >= 0) {
						if (_map[arr_y[j] + dst.y][arr_x[i] + dst.x] == 0)
						{
							dst.x += arr_x[i];
							dst.y += arr_y[j];
							isFound = true;
							break;
						}
					}
				}
			}
		}

	}
	return FindPathBFS(src, dst, maps[currentMapIdx]);
}

void CheckAbilities() {
	for (auto &Robot : inGameRobots) {
		time(&now);

		if (!Robot->IsAbilityReady(0))
		{
			if (difftime(now, Robot->GetAbilityUsedTime(0)) >= Robot->GetAbilityCooldown(0))
			{
				Robot->SetAbilityReady(0, true);
			}
		}
		if (!Robot->IsAbilityReady(1))
		{
			if (difftime(now, Robot->GetAbilityUsedTime(1)) >= Robot->GetAbilityCooldown(1))
			{
				Robot->SetAbilityReady(1, true);
			}
		}
	}

}

void HealWithPoints() { // her 10 sn bir healt pointi 50 den düşük olanlara total point üzerinden can verir.
	time(&now);
	int diff = (int)difftime(now, lastHealTime);
	if (diff >= 10)
	{
		if (currentScore > 0) {

			for (auto &Robot : robots) {
				if (Robot->GetHealth() < 50)
				{
					if (currentScore - 3 >= 0) {
						Robot->Heal(3);
						time(&lastHealTime);
						currentScore -= 3;
					}
					else {
						break;
					}
				}
			}

		}

	}
}

// Loads next level from Maps array
// Arranges sprites and positions
void LoadNextLevel() {
	if (currentMapIdx == NUM_MAP - 1)
		return;

	currentMapIdx++;
	ResetFogMap();
	// Destroy demons and demon bases
	for (auto &demonBase : demonBases) {
		for (auto &demon : demonBase->GetCurrentDemons()) {
			demon->GetSprite()->Kill();
			delete demon;
		}
		demonBase->GetSprite()->Kill();
		delete demonBase;
	}
	demonBases.clear();

	// Destroy walls
	for (auto &sprite : game->GetSprites()) {
		if (sprite->GetSpriteType() == ST_WALL) sprite->Kill();
	}

	// Reposition robots to the bottom of the new map
	int i = 0;
	for (auto &robot : inGameRobots) {
		robot->GetSprite()->SetPosition((14 + 2 * i) * 32, 23 * 32);
		robot->GetSprite()->SetZOrder(10);
		i++;
	}

	// Load new obstacles from the map
	Sprite *sprite;
	for (int i = 0; i < maps[currentMapIdx].size(); i++)
	{
		for (int j = 0; j < maps[currentMapIdx][0].size(); j++)
		{
			// left to right; which map, which row, which col.
			if (maps[currentMapIdx][i][j] == 1)
			{
				sprite = new Sprite(bmEnemyBase);
				sprite->SetPosition(j * 32, i * 32);
				DemonBase* base = new DemonBase({ j, i }, 0, sprite);
				base->SetMapPosition(ScreenRectToArrayPoint(sprite->GetPosition()));
				demonBases.push_back(base);
				sprite->SetSpriteType(ST_BASE);
				game->AddSprite(sprite);
			}
			else if (maps[currentMapIdx][i][j] == 2)
			{
				sprite = new WallSprite(bmWall1);
				sprite->SetPosition(j * 32, i * 32);
				sprite->SetSpriteType(ST_WALL);
				game->AddSprite(sprite);
			}
		}
	}

	// Create boss if cur level boss level
	if (currentMapIdx == 2) {
		POINT mp = { 15, 6 };
		POINT wp = { mp.x * 32, mp.y * 32 };
		Sprite* spr = new Sprite(bmBoss1);
		spr->SetPosition(wp);
		spr->SetNumFrames(18);
		spr->SetFrameDelay(2);
		demonBoss1 = new DemonBoss1(spr, mp);
		spr->SetCharacter(demonBoss1);
		spr->SetSpriteType(ST_DEMON);
		game->AddSprite(spr);
	}
}

// Passes selected robot indexes to Robot instances.
// Assigns robots to players.
// Creates map.
void InitializeGameWorld() {
	// Get selected robots IDs, then find the instance
	// of these robots, and push it to vector which
	// will be used during gameplay to control robots.
	// Also add sprites to sprite manager.
	for (auto i : selectedRobotIndexes) {
		inGameRobots.push_back(robots[i]);
		robots[i]->GetSprite()->SetPosition((14+2*i) * 32, 23*32);
		robots[i]->GetSprite()->SetZOrder(10);
		game->AddSprite(robots[i]->GetSprite());
	}

	// Initially add robot control to each player.
	// Default value of control status is AI.
	inGameRobots[0]->SetControlStatus(CS_P1);
	if (playerCount == 2) {
		inGameRobots[1]->SetControlStatus(CS_P2);
	}

	// Create map
	for (size_t i = 0; i < NUM_MAP; i++)
	{
		if (i == 2) {
			maps[i] = CreateBossMap();
			continue;
		}

		Map newmap = CreateMap(true);
		maps[i] = newmap;
	}

	//TODO: change later
	SwitchFow();

	//int enemyBaseCount = 0;
	//int wallCount = 0;

	//// 0 -> Empty
	//// 1 -> Enemy base
	//// 2 -> Wall
	//Sprite* spr;
	//WallSprite* wspr;
	//for (int i = 0; i < maps[0].size(); i++)
	//{
	//	for (int j = 0; j < maps[0][0].size(); j++)
	//	{
	//		// left to right; which map, which row, which col.
	//		if (maps[0][i][j] == 1)
	//		{
	//			
	//			spr = new Sprite(bmEnemyBase);
	//			spr->SetPosition(j * 32, i * 32);
	//			DemonBase* base = new DemonBase({ j, i }, enemyBaseCount, spr);
	//			base->SetMapPosition(ScreenRectToArrayPoint(spr->GetPosition()));
	//			demonBases.push_back(base);
	//			spr->SetSpriteType(ST_BASE);
	//			game->AddSprite(spr);
	//			spr->SetPosition(j * 32, i * 32);
	//			numEnemyBases++;
	//			enemyBaseCount++;

	//			//AddDemon(base); // add demon on create
	//		}
	//		if (maps[0][i][j] == 2)
	//		{
	//			wspr = new WallSprite(bmWall1);
	//			wspr->SetSpriteType(ST_WALL);
	//			game->AddSprite(wspr);
	//			wspr->SetPosition(j * 32, i * 32);
	//			wallCount++;
	//		}
	//	}
	//}
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
		random_device rd; // obtain a random number from hardware
		mt19937 eng3(rd()); // seed the generator
		uniform_int_distribution<> distr3(0, 3);
		int col = distr3(eng3); //rand() % 5; //4,11,17,23,29
		col = col_a[col];
		srand(time(NULL));
		random_device rd2; // obtain a random number from hardware
		mt19937 eng4(rd2()); // seed the generator
		uniform_int_distribution<> distr4(0, 2);
		int row = distr4(eng4); //4,11,17,23
		row = row_a[row];
		bool avaliable_for_enemyC = false;

		while (!avaliable_for_enemyC)
		{
			if (map[row][col] != 0)
			{
				random_device rd; // obtain a random number from hardware
				mt19937 eng3(rd()); // seed the generator
				uniform_int_distribution<> distr3(0, 3);
				col = distr3(eng3); //rand() % 5; //4,11,17,23,29
				col = col_a[col];
				srand(time(NULL));
				random_device rd2; // obtain a random number from hardware
				mt19937 eng4(rd2()); // seed the generator
				uniform_int_distribution<> distr4(0, 2);
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
		random_device rd; // obtain a random number from hardware
		mt19937 eng3(rd()); // seed the generator
		uniform_int_distribution<> distr3(0, 100);
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
			random_device rd; // obtain a random number from hardware
			mt19937 eng3(rd()); // seed the generator
			uniform_int_distribution<> distr3(0, map_col - 1);
			int col1 = distr3(eng3);

			srand(time(NULL));
			random_device rd2; // obtain a random number from hardware
			mt19937 eng4(rd2());
			uniform_int_distribution<> distr4(0, map_row - 1);
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
							mt19937 eng3(rd()); // seed the generator
							uniform_int_distribution<> distr3(0, map_col - 1);
							col1 = distr3(eng3);
							srand(time(NULL));
							random_device rd2; // obtain a random number from hardware
							mt19937 eng4(rd2());
							uniform_int_distribution<> distr4(0, map_row - 1);
							row1 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					mt19937 eng3(rd()); // seed the generator
					uniform_int_distribution<> distr3(0, map_col - 1);
					col1 = distr3(eng3);
					srand(time(NULL));
					random_device rd2; // obtain a random number from hardware
					mt19937 eng4(rd2());
					uniform_int_distribution<> distr4(0, map_row - 1);
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
			mt19937 eng3(rd()); // seed the generator
			uniform_int_distribution<> distr3(0, map_col - 1);
			int col2 = distr3(eng3);
			srand(time(NULL));
			random_device rd2; // obtain a random number from hardware
			mt19937 eng4(rd2());
			uniform_int_distribution<> distr4(0, 20);
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
							mt19937 eng3(rd()); // seed the generator
							uniform_int_distribution<> distr3(0, map_col - 1);
							col2 = distr3(eng3);
							srand(time(NULL));
							random_device rd2; // obtain a random number from hardware
							mt19937 eng4(rd2());
							uniform_int_distribution<> distr4(0, map_row - 1);
							row2 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					mt19937 eng3(rd()); // seed the generator
					uniform_int_distribution<> distr3(0, map_col - 1);
					col2 = distr3(eng3);
					srand(time(NULL));
					random_device rd2; // obtain a random number from hardware
					mt19937 eng4(rd2());
					uniform_int_distribution<> distr4(0, map_row - 1);
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

// Creates a boss map with a specific pattern
Map CreateBossMap() {
	std::vector<std::vector<int>> bossMap(24, std::vector<int>(32));
	for (int y = 0; y < bossMap.size(); y++) {
		for (int x = 0; x < bossMap[y].size(); x++)
		{
			// Place walls
			if (x == 0 || y == 0 || x == 31 ||((y == 15 || y == 16) && (x >= 3 || x <= 28))) {
				bossMap[y][x] = 2;
			}
		}
	}

	// bases
	bossMap[4][12] = 1;
	bossMap[4][19] = 1;
	bossMap[9][12] = 1;
	bossMap[9][19] = 1;
	return bossMap;
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

	btnStart = new Button(_hDC, (LPTSTR)TEXT("Start"), (RES_W / 2) - (BTN_WIDTH / 2), 650);
	menuMatchmakingButtons.push_back(btnStart);
	btnBackToPlayers2 = new Button(_hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 700);
	menuMatchmakingButtons.push_back(btnBackToPlayers2);
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
			exit(0);
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
			//InitializeGameWorld();
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			//currentScene = GAME_PLAY;
			currentScene = MATCHMAKING;
		}

		// Button click - Back
		if (btnBackToPlayers->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
		}

		// Robot select
		for (auto &Robot : robots) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				bool robotIsSelected = find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end();

				// Add to selected if not selected, erase otherwise
				if (!robotIsSelected && selectedRobotIndexes.size() < 2*playerCount) {
					PlaySound((LPCWSTR)IDW_MENU_SELECT, hInstance, SND_ASYNC | SND_RESOURCE);
					selectedRobotIndexes.push_back(i);
				}
				else {
					selectedRobotIndexes.erase(remove(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i), selectedRobotIndexes.end());
				}
			}
			i++;
		}
		break;
	case MATCHMAKING:
		// Button click - Start
		if (btnStart->GetSprite()->IsPointInside(_x, _y)) {
			InitializeGameWorld();
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = GAME_PLAY;
		}

		if (btnBackToPlayers2->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
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

		for (auto &Robot :  robots ) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				Robot->SetMenuHover(true);
			}
			else {
				Robot->SetMenuHover(false);
			}
		}
		break;
	case MATCHMAKING:
		for (auto& Button : menuMatchmakingButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------
// UI
//-----------------------------------------------------------------

// Draws indicatiors in game play scene.
void DrawIndicators(HDC _hDC, Character* _character) 
{
	if (_character->GetSprite()->IsHidden()) return; // won't draw indicators for hidden sprites

	// Draw health bar
	HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

	int totalWidth = _character->GetSprite()->GetWidth()*1.5;
	int totalHeight = 5;
	int greenWidth = round(_character->GetHealthPoint() * totalWidth / _character->GetMaxHealthPoint());
	int redWith = totalWidth - greenWidth;

	RECT pos = _character->GetSprite()->GetPosition();
	pos.top -= 15;
	pos.left -= totalWidth / 5;

	SelectObject(_hDC, greenBrush);
	Rectangle(_hDC, pos.left, pos.top, pos.left + greenWidth, pos.top + totalHeight);
	DeleteObject(greenBrush);

	SelectObject(_hDC, redBrush);
	Rectangle(_hDC, pos.left + greenWidth, pos.top, pos.left + greenWidth + redWith, pos.top + totalHeight);
	DeleteObject(redBrush);

	// Draw controller's name if robot
	RECT bounds;
	bounds = _character->GetSprite()->GetPosition();
	bounds.bottom = bounds.top;
	bounds.top -= 40;

	if (_character->IsRobot()) {
		string identifier;
		if (((Robot *)_character)->GetControlStatus() == CS_AI) identifier = "AI";
		else if (((Robot *)_character)->GetControlStatus() == CS_P2) identifier = "P2";
		else identifier = "P1";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(identifier), 16, bounds);
	}

	if (!_character->GetStatus().empty() && difftime(time(&now), _character->GetStatusEnd()) < 0) {
		bounds.top -= 20;
		bounds.bottom -= 20;
		bounds.left -= 40;
		bounds.right += 40;

		if (_character->GetStatusColor() == NULL) {
			SetTextColor(_hDC, RGB(rand() % 255, rand() % 255, rand() % 255)); // blinking all colors
		}
		else {
			SetTextColor(_hDC, _character->GetStatusColor());
		}

		game->PrintText(_hDC, ConvertString(_character->GetStatus()), 14, bounds);
	}
}

// Draw robots on the select menu with their menu sprites.
// Displays name and descriptions on hover.
void DrawRobotsOnMenu(HDC _hDC)
{
	// 100 x 100 robot icons
	int iconWidth = 100;
	int offset = iconWidth + iconWidth;

	// highlight thickness
	int thickness = 10;

	// Calculate to center robot icons
	POINT start;
	start.x = (RES_W / 2) - (iconWidth / 2) - ((robots.size() - 1) * iconWidth);
	start.y = 300;

	int i = 0; // to see which robot is clicked
	for (auto &Robot : robots) {
		Robot->GetMenuSprite()->SetPosition(start.x + (i*offset), start.y); // set new position
		bool robotIsSelected = find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end(); // check if robot is selected

		/*
		// If robot is hovered:
		//		not selected -> white focus
		//		selected	 -> red focus
		// If not hovered:
		//		not selected -> nothing
		//		selected	 -> green focus
		*/
		if (Robot->IsMenuHover()) {
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetName()), 36, { 0, 500, RES_W, 550 });
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetDescription()), 18, { RES_W/6, 550, 5*RES_W/6, 700 });

			HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
			if (robotIsSelected) {
				SelectObject(_hDC, hRedBrush);
			}

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - thickness, Robot->GetMenuSprite()->GetPosition().top - thickness,
				Robot->GetMenuSprite()->GetPosition().right + thickness, Robot->GetMenuSprite()->GetPosition().bottom + thickness);
			DeleteObject(hRedBrush);
		}
		else if (robotIsSelected) {
			HBRUSH hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(_hDC, hGreenBrush);

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - thickness, Robot->GetMenuSprite()->GetPosition().top - thickness,
				Robot->GetMenuSprite()->GetPosition().right + thickness, Robot->GetMenuSprite()->GetPosition().bottom + thickness);
			DeleteObject(hGreenBrush);
		}
		Robot->GetMenuSprite()->Draw(_hDC);
		i++;
	}

	// Display how many robots more should be selected
	if (playerCount * 2 > selectedRobotIndexes.size()) {
		string remaining = to_string(playerCount * 2 - selectedRobotIndexes.size()) + " remaining";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(remaining), 24, { 0, 250, RES_W, 300 });
	}
	else if (playerCount * 2 == selectedRobotIndexes.size()) {
		string remaining = "Ready to go!";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(remaining), 24, { 0, 250, RES_W, 300 });
	}
}

void DrawRightMenu(HDC _hDC) {
	int offsetV = 10;
	int leftPos = 1023;

	// Set colors
	SetTextColor(_hDC, UI_TEXT_COLOR);
	SetBkColor(_hDC, UI_BG_COLOR);

	int scoreSize = 20;
	RECT scoreBounds = { leftPos, offsetV, RES_W, offsetV + scoreSize };
	swprintf(textBuffer, 150, L"L:%d Score:%d", currentMapIdx + 1, currentScore);
	game->PrintText(_hDC, textBuffer, scoreSize, scoreBounds);

	int robotOffset = 2 * offsetV + scoreSize;

	RECT bounds;

	for (int i = 0; i < inGameRobots.size(); i++) {
		robotOffset = (i * 150) + 30;
		int robotTitleSize = 24;
		int lastPos;
		bounds = { leftPos, robotOffset+offsetV, RES_W, robotOffset+offsetV+robotTitleSize };
		lastPos = robotOffset + offsetV + offsetV + robotTitleSize;
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		game->PrintText(_hDC, ConvertString(inGameRobots[i]->GetName()), robotTitleSize, bounds);

		int secondarySize = 14;

		// Control status
		bounds = { leftPos,lastPos, RES_W, robotOffset + lastPos + offsetV };
		lastPos = robotOffset + (2 * offsetV) + robotTitleSize + secondarySize;
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		
		if (inGameRobots[i]->GetControlStatus() == CS_AI) {
			string aiStatus = { "AI" };
			if (inGameRobots[i]->GetTarget() != NULL) {
				aiStatus = { "AI => " + inGameRobots[i]->GetTarget()->GetName() };
			}
			game->PrintText(_hDC, ConvertString(aiStatus), secondarySize, bounds);
		}
		else if (inGameRobots[i]->GetControlStatus() == CS_P1) {
			game->PrintText(_hDC, ConvertString({ "P1" }), secondarySize, bounds);
		}
		else if (inGameRobots[i]->GetControlStatus() == CS_P2){
			game->PrintText(_hDC, ConvertString({ "P2" }), secondarySize, bounds);
		}

		// Stats
		bounds = { leftPos, lastPos + offsetV, RES_W, lastPos + offsetV + secondarySize };
		lastPos = lastPos + offsetV + offsetV +  secondarySize;
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		game->PrintText(_hDC, 
			ConvertString({ 
				"HP:" + to_string(inGameRobots[i]->GetHealthPoint()) + "/" + to_string(inGameRobots[i]->GetMaxHealthPoint()) + " | "
				"SP:" + to_string(inGameRobots[i]->GetSpeed())
				}), 
			secondarySize, bounds);	

		/*bounds = { leftPos, lastPos + offsetV, RES_W, lastPos + offsetV + secondarySize };
		lastPos = lastPos + offsetV + offsetV + secondarySize;
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		game->PrintText(_hDC,
			ConvertString({
				"[AR:" + to_string(inGameRobots[i]->GetArmor()) + "] "
				"[FS:" + to_string(inGameRobots[i]->GetFireSpeed()) + "] " 
				}),
			secondarySize, bounds);*/

		// Abilities
		string a1 = { 0 }, a2 = { 0 };
		if (inGameRobots[i]->IsAbilityReady(0)) a1 = { "R" };
		else a1 = { to_string(max(0, int(difftime(inGameRobots[i]->GetAbilityUsedTime(0) + inGameRobots[i]->GetAbilityCooldown(0), now)))) + "s" };
		if (inGameRobots[i]->IsAbilityReady(1)) a2 = { "R" };
		else a2 = { to_string(max(0, int(difftime(inGameRobots[i]->GetAbilityUsedTime(1) + inGameRobots[i]->GetAbilityCooldown(1), now)))) + "s" };

		bounds = { leftPos, lastPos + offsetV, RES_W, lastPos + offsetV + secondarySize };
		lastPos = lastPos + offsetV + offsetV + secondarySize;
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		game->PrintText(_hDC,
			ConvertString({
				"[A1:" + a1 + "] "+
				"[A2:" + a2 +"]"
				}),
			secondarySize, bounds);

		bounds = { leftPos, lastPos + offsetV, RES_W, lastPos + offsetV + secondarySize };
		SetTextColor(_hDC, UI_TEXT_COLOR);
		SetBkColor(_hDC, UI_BG_COLOR);
		game->PrintText(_hDC, ConvertString({ "-----------------------" }), secondarySize, bounds);
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

// Converts string to LPWSTR.
// Neccessary for DRAW TEXT.
LPWSTR ConvertString(const string& instr)
{
	// Assumes string is encoded in the current Windows ANSI codepage
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

		if (map[c->y][c->x] != 0 && map[c->y][c->x] != 3&&map[c->y][c->x] != 4) continue;

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

Robot* GetRobotAtMapPosition(int _x, int _y) {
	for (auto &Robot : inGameRobots) {
		if (Robot->GetMapPosition().x == _x && Robot->GetMapPosition().y == _y) return Robot;
	}
	return NULL;
}

Demon* GetDemonAtMapPosition(int _x, int _y) {
	for (auto &DemonBase : demonBases) {
		for (auto &Demon : DemonBase->GetCurrentDemons()) {
			if (Demon->GetMapPosition().x == _x && Demon->GetMapPosition().y == _y) return Demon;
		}
	}
	return NULL;
}

// Sets fog map all to 0
void ResetFogMap() {
	std::vector<std::vector<int>> fogTemp(24, std::vector<int>(32));
	fogMap = fogTemp;
}

// Set active positions to discovered to check again, storing discovered positions
void ClearFogMap() {
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 32; j++)
		{
			if (fogMap[i][j] == FMS_VISIBLE) {
				fogMap[i][j] = FMS_DISCOVERED;
			}	
		}
	}
}

// Switch between fow enabled and disabled
void SwitchFow() {
	fow = !fow;

	if (!fow) {
		// we have to set hidden false for all sprites
		for (auto &spr : game->GetSprites()) {
			spr->SetHidden(false);
		}
	}
}