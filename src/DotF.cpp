//-----------------------------------------------------------------
// Defiers of the Four Application
// C++ Source - DotF.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DotF.h"


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
	game->SetFrameRate(30);

	// Store the instance handle
	hInstance = _hInstance;

	// Configure game variables
	currentScene = GAME_PLAY;
	playerNum = -1;

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

	//-----------------------------------------------------------------
	// Characters
	//-----------------------------------------------------------------
	Bitmap *robotBitmap = new Bitmap(hDC, IDB_MENU_ROBOT, hInstance);
	Sprite *robotSprite;
	POINT pos;
	pos.x = 250;
	pos.y = 250;
	robotSprite = new Sprite(robotBitmap);
	Robot *robot1 = new Robot({ "Robot 1" }, { "Robot 1 Desc" }, robotSprite, 100, 7, pos, AI, 10, 30);
	robotSprite = new Sprite(robotBitmap);
	Robot *robot2 = new Robot({ "Robot 2" }, { "Robot 2 Desc" }, robotSprite, 100, 7, pos, AI, 10, 30);
	robotSprite = new Sprite(robotBitmap);
	Robot *robot3 = new Robot({ "Robot 3" }, { "Robot 3 Desc" }, robotSprite, 100, 7, pos, AI, 10, 30);
	robotSprite = new Sprite(robotBitmap);
	Robot *robot4 = new Robot({ "Robot 4" }, { "Robot 4 Desc" }, robotSprite, 100, 7, pos, AI, 10, 30);
	robots.push_back(robot1);
	robots.push_back(robot2);
	robots.push_back(robot3);
	robots.push_back(robot4);

	Bitmap *avatarBitmap = new Bitmap(hDC, IDB_AVATAR, hInstance);
	bmSheep = new Bitmap(hDC, IDB_AVATAR, hInstance);
	Sprite *avatarSprite = new Sprite(avatarBitmap);
	Robot *robotPlay = new Robot({ "Robot 4" }, { "Robot 4 Desc" }, avatarSprite, 100, 6, pos, PLAYER_1, 10, 30);
	robots.push_back(robotPlay);
	robotPlay->GetSprite()->SetPosition(500, 600);


	// Menu bitmaps and sprites
	bmMenuBackground = new Bitmap(hDC, 1024, 768, RGB(22, 15, 28));
	bmMenuTitle = new Bitmap(hDC, IDB_MENU_TITLE, hInstance);
	bmMenuStar = new Bitmap(hDC, IDB_MENU_STAR, hInstance);

	//-----------------------------------------------------------------
	// Buttons
	//-----------------------------------------------------------------
	// Main menu buttons 
	btnPlay = new Button(hDC, (LPTSTR)TEXT("Play"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuMainButtons.push_back(btnPlay);
	btnExit = new Button(hDC, (LPTSTR)TEXT("Exit"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuMainButtons.push_back(btnExit);

	// Play menu buttons 
	btn1Player = new Button(hDC, (LPTSTR)TEXT("1 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuPlayersButtons.push_back(btn1Player);
	btn2Player = new Button(hDC, (LPTSTR)TEXT("2 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuPlayersButtons.push_back(btn2Player);
	btnBackToMain = new Button(hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 500);
	menuPlayersButtons.push_back(btnBackToMain);

	// Character select buttons
	btnReady = new Button(hDC, (LPTSTR)TEXT("Begin"), (RES_W / 2) - (BTN_WIDTH / 2), 650);
	menuRobotsButtons.push_back(btnReady);
	btnBackToPlayers = new Button(hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 700);
	menuRobotsButtons.push_back(btnBackToPlayers);

	// Create map
	bmTile1 = new Bitmap(hDC, IDB_TILE_1, hInstance);
	for (size_t i = 0; i < 8; i++)
	{
		if (i == 0) {

			map  newmap = CreateMap(true);
			maps[i] = newmap;
		}
		else
		{
			map  newmap = CreateMap(false);
			maps[i] = newmap;
		}
	}

	_pWallBitmap = new Bitmap(hDC, IDB_WALL, hInstance);
	_pEnemyBaseBitmap = new Bitmap(hDC, IDB_ENEMY_BASE, hInstance);
	// Create the golf ball sprites
	path = { -1, -1, -1, -1 };
	int _pEnemyBase_c = 0;
	int _pWalls_c = 0;

	for (size_t i = 0; i < maps[0].size(); i++)
	{
		for (size_t j = 0; j < maps[0][0].size(); j++)
		{
			if (maps[0][i][j] == 1) // enemy center   // soldan sağa indexler 0: hangi ekranda olduğu ,2 :row , 3 : col
			{
				_pEnemyBase[_pEnemyBase_c] = new Sprite(_pEnemyBaseBitmap);

				_pEnemyBase[_pEnemyBase_c]->SetPosition(j * 32, i * 32);
				number_of_enemyCenter++;
				_pEnemyBase_c++;
			}
			if (maps[0][i][j] == 2) // wall
			{
				_pWalls[_pWalls_c] = new Sprite(_pWallBitmap);

				_pWalls[_pWalls_c]->SetPosition(j * 32, i * 32);
				_pWalls_c++;

			}
		}
	}

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

void GameActivate(HWND _hWindow)
{
}

void GameDeactivate(HWND _hWindow)
{
}

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
		if (selectedRobots.size() == playerNum * 2) {
			btnReady->Draw(_hDC);
		}
		break;
	case GAME_PLAY:
		/*for (int i = 0; i < RES_W / 32; i++) {
			for (int j = 0; j < RES_H / 32; j++) {
				bmTile1->Draw(_hDC, 32 * i, 32 * j);
			}
		}*/
		for (int i = 0; i < 100; i++)
			_pWalls[i]->Draw(_hDC);
		for (int i = 0; i < number_of_enemyCenter; i++)
			_pEnemyBase[i]->Draw(_hDC);

		if (pathQ.size() > 0) {
			bmSheep->Draw(_hDC, pathQ.front().x * 32, pathQ.front().y * 32);
			pathQ.pop();
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

void HandleKeys()
{
	if (currentScene == GAME_PLAY) {
		POINT velocity = { 0,0 };

		if (GetAsyncKeyState(VK_LEFT) < 0) {
			velocity.x = -robots[4]->GetSpeed();
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0) {
			velocity.x = robots[4]->GetSpeed();
		}

		if (GetAsyncKeyState(VK_UP) < 0) {
			velocity.y = -robots[4]->GetSpeed();
		}
		else if (GetAsyncKeyState(VK_DOWN) < 0) {
			velocity.y = robots[4]->GetSpeed();
		}

		if (velocity.y != 0 && velocity.x != 0) {
			velocity = { ((velocity.x > 0) ? 1 : -1) * 4, ((velocity.y > 0) ? 1 : -1) * 4 };
		}
		robots[4]->GetSprite()->Update();
		robots[4]->GetSprite()->SetVelocity(velocity);

		bulletRotation = velocity;

		/*if (GetAsyncKeyState(VK_SPACE) < 0 && !(bulletRotation.x == 0 && bulletRotation.y == 0)) {
			RECT bounds = { 0, 0, 1024, 768 };
			Bitmap *bmSheep = new Bitmap();
			Sprite *bullet = new Sprite(bmSheep, bounds, BA_DIE);
			bullet->SetPosition(robots[4]->GetSprite()->GetPosition());
			bullet->SetVelocity(bulletRotation);
			game->AddSprite(bullet);
		}*/
	}
}

void MouseButtonDown(int _x, int _y, BOOL _isLeftClick)
{
	if (_isLeftClick) {
		if (path.top == -1) {
			path.left = round(_x / 32);
			path.top = round(_y / 32);
		}
		else {
			path.right = round(_x / 32);
			path.bottom = round(_y / 32);

			//POINT src = {0, 0 };
			POINT src = { path.left, path.top };
			POINT dst = { path.right, path.bottom };
			pathQ = pathFinding(dst, src, maps[0]);

		}
	}
}

void MouseButtonUp(int _x, int _y, BOOL _isLeftClick)
{
	if (_isLeftClick) {
		HandleMenuButtonClick(_x, _y);
	}
}

void MouseMove(int _x, int _y)
{
	// Hover effect for menu
	HandleMenuButtonHover(_x, _y);

}

void HandleJoystick(JOYSTATE _joystickState)
{
}

BOOL SpriteCollision(Sprite* _spriteHitter, Sprite* _spriteHittee)
{
	// Swap the sprite velocities so that they appear to bounce
	POINT ptSwapVelocity = _spriteHitter->GetVelocity();
	_spriteHitter->SetVelocity(_spriteHittee->GetVelocity());
	_spriteHittee->SetVelocity(ptSwapVelocity);
	return TRUE;
}

//-----------------------------------------------------------------
// Menu functions
//-----------------------------------------------------------------
void HandleMenuButtonClick(int _x, int _y) {
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
			playerNum = 1;
			currentScene = MENU_SELECT_ROBOTS;
		}

		// 2 - Player selected
		if (btn2Player->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			playerNum = 2;
			currentScene = MENU_SELECT_ROBOTS;
		}
		break;
	case MENU_SELECT_ROBOTS:
		// Button click - Ready
		if (btnReady->GetSprite()->IsPointInside(_x, _y) && playerNum * 2 == selectedRobots.size()) {
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
			if (Robot->GetSprite()->IsPointInside(_x, _y)) {
				bool robotIsSelected = std::find(selectedRobots.begin(), selectedRobots.end(), i) != selectedRobots.end();

				// Add to selected if not selected, erase otherwise
				if (!robotIsSelected) {
					PlaySound((LPCWSTR)IDW_MENU_SELECT, hInstance, SND_ASYNC | SND_RESOURCE);
					selectedRobots.push_back(i);
				}
				else {
					selectedRobots.erase(std::remove(selectedRobots.begin(), selectedRobots.end(), i), selectedRobots.end());
				}
			}
			i++;
		}
		break;
	default:
		break;
	}
}

void HandleMenuButtonHover(int _x, int _y) {
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
			if (Robot->GetSprite()->IsPointInside(_x, _y)) {
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

void DrawRobotsOnMenu(HDC _hDC) {
	// 100 x 100 robot icons
	int iconWidth = 100;
	int offset = iconWidth + iconWidth;

	// Calculate to center robot icons
	POINT start;
	start.x = (RES_W / 2) - (iconWidth / 2) - ((robots.size() - 1) * iconWidth);
	start.y = 300;

	int i = 0; // to see which robot is clicked
	for (auto &Robot : robots) {
		Robot->GetSprite()->SetPosition(start.x + (i*offset), start.y); // set new position
		bool robotIsSelected = std::find(selectedRobots.begin(), selectedRobots.end(), i) != selectedRobots.end(); // check if robot is selected

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

			Rectangle(_hDC, Robot->GetSprite()->GetPosition().left - 5, Robot->GetSprite()->GetPosition().top - 5,
				Robot->GetSprite()->GetPosition().right + 20, Robot->GetSprite()->GetPosition().bottom + 20);
			DeleteObject(hRedBrush);
		}
		else if (robotIsSelected) {
			HBRUSH hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(_hDC, hGreenBrush);

			Rectangle(_hDC, Robot->GetSprite()->GetPosition().left - 5, Robot->GetSprite()->GetPosition().top - 5,
				Robot->GetSprite()->GetPosition().right + 20, Robot->GetSprite()->GetPosition().bottom + 20);
			DeleteObject(hGreenBrush);
		}
		Robot->GetSprite()->Draw(_hDC);
		i++;
	}

	// Display how many robots more should be selected
	if (playerNum * 2 > selectedRobots.size()) {
		std::string remaining = std::to_string(playerNum * 2 - selectedRobots.size()) + " remaining";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(remaining), 24, { 0, 250, 1024, 300 });
	}
}

//-----------------------------------------------------------------
// Game
//-----------------------------------------------------------------
map CreateMap(bool _firstLevel)
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
	else {
		map_row = 24;

	}




	vector< vector<int> > wall_positions(number_of_wall, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 
	vector< vector<int> > enemy_center_positions(number_of_enemy_center, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 
	// 0 empty
	//1 enemy center
	// 2 wall
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

queue <POINT> pathFinding(POINT targetposition, POINT startingposition, map currentmap) { // map_index hangi harita olduğu 1,2..8

	bool reach_starting_point = false;
	queue <POINT> adjacents;
	queue <int> weights;
	adjacents.push(targetposition);
	weights.push(0);
	int current_num = -1;
	int row;

	row = 24;

	int arr_x[3] = { -1,0,1 };
	int arr_y[3] = { -1,0,1 };
	while (!reach_starting_point) {

		if (adjacents.size() < 1)
			break;
		if (adjacents.front().x == startingposition.x&&adjacents.front().y == startingposition.y)
		{
			reach_starting_point = true;
			break;
		}

		for (size_t i = 0; i < 3 && !reach_starting_point; i++)
		{
			for (size_t j = 0; j < 3 && !reach_starting_point; j++)
			{
				if (!(i == 0 && j == 0)) {
					if (adjacents.size() > 0)
					{

						if (adjacents.front().x + arr_x[i] < 32 && adjacents.front().x + arr_x[i] >= 0 && adjacents.front().y + arr_y[j] < row && adjacents.front().y + arr_y[j] >= 0)
						{
							if (adjacents.front().x + arr_x[i] == startingposition.x&&adjacents.front().y + arr_y[j] == startingposition.y) {
								currentmap[adjacents.front().y + arr_y[j]][adjacents.front().x + arr_x[i]] = current_num;
								POINT np;
								np.x = adjacents.front().x + arr_x[i];
								np.y = adjacents.front().y + arr_y[j];
								adjacents.push(np);
								weights.push(current_num);
								reach_starting_point = true;
								break;
							}
							if (currentmap[adjacents.front().y + arr_y[j]][adjacents.front().x + arr_x[i]] == 0)
							{
								currentmap[adjacents.front().y + arr_y[j]][adjacents.front().x + arr_x[i]] = current_num;
								POINT np;
								np.x = adjacents.front().x + arr_x[i];
								np.y = adjacents.front().y + arr_y[j];
								adjacents.push(np);
								weights.push(current_num);
							}


						}
					}
				}

			}
		}
		int temp1 = weights.front();
		weights.pop();
		adjacents.pop();
		int temp2;
		if (weights.size() > 0)
		{
			temp2 = weights.front();
			if (temp1 != temp2)
			{
				current_num--;
			}
		}

	}

	bool reach_target_point = false;
	POINT currentpoint;
	queue <POINT> finalroute;
	currentpoint = startingposition;
	finalroute.push(startingposition);
	while (!reach_target_point) {

		if (currentpoint.x == targetposition.x&&currentpoint.y == targetposition.y)
		{
			reach_target_point = true;
			break;
		}
		int temp_x = 0;
		int temp_y = 0;
		int minpoint = -10000;
		for (size_t i = 0; i < 3 && !reach_target_point; i++)
		{
			for (size_t j = 0; j < 3 && !reach_target_point; j++)
			{
				if (!(i == 0 && j == 0)) {
					if (currentpoint.x + arr_x[i] < 32 && currentpoint.x + arr_x[i] >= 0 && currentpoint.y + arr_y[j] < row && currentpoint.y + arr_y[j] >= 0)
					{
						if (currentmap[currentpoint.y + arr_y[j]][currentpoint.x + arr_x[i]] < 0) // eksi değerlere bakılır .
						{

							if (currentmap[currentpoint.y + arr_y[j]][currentpoint.x + arr_x[i]] > minpoint)
							{
								minpoint = currentmap[currentpoint.y + arr_y[j]][currentpoint.x + arr_x[i]];
								temp_x = currentpoint.x + arr_x[i];
								temp_y = currentpoint.y + arr_y[j];
							}

						}
						if (currentpoint.x + arr_x[i] == targetposition.x&&currentpoint.y + arr_y[j] == targetposition.y) {
							temp_x = currentpoint.x + arr_x[i];
							temp_y = currentpoint.y + arr_y[j];
							reach_target_point = true;
							break;
						}


					}
				}

			}
		}

		POINT np;
		np.x = temp_x;
		np.y = temp_y;
		currentpoint = np;
		finalroute.push(np);


	}
	return finalroute;
}
//-----------------------------------------------------------------
// Utility
//-----------------------------------------------------------------
LPWSTR ConvertString(const std::string& instr) {
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
