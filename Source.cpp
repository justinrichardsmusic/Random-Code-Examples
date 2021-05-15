#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int const nScreenSize = 480;
int const nPlayerCount = 8;
int const nAppleCount = 50;

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "PGE_Snake";
	}

private:
	// Initialise Map
	int const nTileSize = 16;
	int const nMapSize = nScreenSize / nTileSize;

	int const nHalfTileSize = (int)(nTileSize * 0.5);

	// Player Colours
	olc::Pixel const playerColour[8] = { olc::WHITE, olc::YELLOW, olc::RED, olc::BLUE, olc::MAGENTA, olc::GREEN, olc::CYAN, olc::DARK_MAGENTA };

	// Player Variables
	std::vector<std::pair<int, int>> playerSegments[nPlayerCount];
	std::pair<int, int> nPlayerPos[nPlayerCount];
	std::pair<int, int> nPlayerVel[nPlayerCount];
	std::pair<int, int> nPlayerNextVel[nPlayerCount];
	int nTailLength[nPlayerCount];
	float fPlayerSpeed;
	int nPlayerScore[nPlayerCount];

	int nCurrentPlayerCount = 1;
	int nCurrentAppleCount = 1;
	std::string strGameMode = "Classic";

	// Apple variables
	int ax[nAppleCount], ay[nAppleCount], at[nAppleCount];  // x, y, tiumer for apple time out
	int const nAppleTimeOut = (int)(nScreenSize / nTileSize) * 2;

	// Generic variables
	float fGameSpeedCounter;
	bool bExitFlag = false;

	// Game States
	enum GAME_STATE
	{
		GS_RESET,
		GS_DRAW_MENU,
		GS_MENU,
		GS_GAME_START,
		GS_GAME_PLAY,
		GS_GAME_OVER

	} nGameState, nNextState;

	void ResetApple(int appleIndex, int playerIndex)
	{
		int nRandomLocation = rand() % (nMapSize * nMapSize);
		ax[appleIndex] = nRandomLocation % nMapSize;
		ay[appleIndex] = (int)(nRandomLocation / nMapSize);
		at[appleIndex] = 0;

		// check to see if apple spawned inside the tail, if so respawn it (possible reccursion but only if apple count exceeds available screen space!)
		for (int i = 0; i < nPlayerCount; i++)
			for (auto const &p : playerSegments[i])
				if (ax[appleIndex] == p.first && ay[appleIndex] == p.second) ResetApple(appleIndex, playerIndex);

		// check to see if our apple spawned inside another apple
		for (int a = 0; a < nAppleCount; a++)
			if (ax[appleIndex] == ax[a] && ay[appleIndex] == ay[a] && appleIndex != a) ResetApple(appleIndex, playerIndex);

		if (playerIndex >= 0) 
			nPlayerScore[playerIndex]++;
	}
	
	void ResetGame()
	{
		// Reset player and game variables
		for (int i = 0; i < nPlayerCount; i++)
		{
			nPlayerPos[i] = { std::floor(nMapSize * 0.2 + i * 2), std::floor(nMapSize * 0.2 + i * 2) };

			//for (int r = 0; r < nPlayerCount; r++)
			//{
			//	if (nPlayerPos[r] == nPlayerPos[i]) ResetGame(); // Make sure the players aren't in same starting location
			//}

			nPlayerVel[i] = { -1, 0 };
			nPlayerNextVel[i] = nPlayerVel[i];
			nTailLength[i] = 5;
			nPlayerScore[i] = 0;
			playerSegments[i].clear();
			playerSegments[i].push_back(nPlayerPos[i]);
		}
		fPlayerSpeed = 4.0;
		fGameSpeedCounter = 0.0f;

		for (int a = 0; a < nAppleCount; a++)
			ResetApple(a, -1); // -1 == don't increase score
	}

	void DrawPlayer(int i)
	{
		for (size_t s = 1; s < playerSegments[i].size() - 1; s++)
		{
			FillCircle((int)playerSegments[i].at(s).first * nTileSize + nHalfTileSize, (int)playerSegments[i].at(s).second * nTileSize + nHalfTileSize, nHalfTileSize, playerColour[i]);

			// North
			if (playerSegments[i].at(s - 1).second < playerSegments[i].at(s).second || playerSegments[i].at(s + 1).second < playerSegments[i].at(s).second)
				FillRect((int)playerSegments[i].at(s).first * nTileSize + nHalfTileSize - 1, (int)playerSegments[i].at(s).second * nTileSize, 2, nHalfTileSize, olc::BLACK);

			// South
			if (playerSegments[i].at(s - 1).second > playerSegments[i].at(s).second || playerSegments[i].at(s + 1).second > playerSegments[i].at(s).second)
				FillRect((int)playerSegments[i].at(s).first * nTileSize + nHalfTileSize - 1, (int)playerSegments[i].at(s).second * nTileSize + nHalfTileSize, 2, nHalfTileSize, olc::BLACK);

			// West
			if (playerSegments[i].at(s - 1).first < playerSegments[i].at(s).first || playerSegments[i].at(s + 1).first < playerSegments[i].at(s).first)
				FillRect((int)playerSegments[i].at(s).first * nTileSize, (int)playerSegments[i].at(s).second * nTileSize + nHalfTileSize - 1, nHalfTileSize, 2, olc::BLACK);

			// East
			if (playerSegments[i].at(s - 1).first > playerSegments[i].at(s).first || playerSegments[i].at(s + 1).first > playerSegments[i].at(s).first)
				FillRect((int)playerSegments[i].at(s).first * nTileSize + nHalfTileSize, (int)playerSegments[i].at(s).second * nTileSize + nHalfTileSize - 1, nHalfTileSize, 2, olc::BLACK);
		}

		// Draw Tail End and Head
		FillCircle((int)playerSegments[i].at(0).first * nTileSize + nHalfTileSize, (int)playerSegments[i].at(0).second * nTileSize + nHalfTileSize, nHalfTileSize / 2, playerColour[i]);

		FillRect((int)playerSegments[i].at(0).first * nTileSize, (int)playerSegments[i].at(0).second * nTileSize + nHalfTileSize - 1, nTileSize, 2, olc::BLACK);
		FillRect((int)playerSegments[i].at(0).first * nTileSize + nHalfTileSize - 1, (int)playerSegments[i].at(0).second * nTileSize, 2, nTileSize, olc::BLACK);

		FillCircle((int)playerSegments[i].at(playerSegments[i].size() - 1).first * nTileSize + nHalfTileSize, (int)playerSegments[i].at(playerSegments[i].size() - 1).second * nTileSize + nHalfTileSize, nHalfTileSize + 1, playerColour[i]);

		// Work out where the head of the snake is in world space so we can more easily place the eyes...
		int nSnakeHeadXInWorldSpace = (playerSegments[i].back().first * nTileSize) + (nTileSize * 0.5);
		int nSnakeHeadYInWorldSpace = (playerSegments[i].back().second * nTileSize) + (nTileSize * 0.5);

		if (nPlayerVel[i].first != 0)
		{
			// We are facing left or right so draw the eyes where they should be
			Draw(nSnakeHeadXInWorldSpace + 2 * nPlayerVel[i].first, nSnakeHeadYInWorldSpace - 2, olc::BLACK);
			Draw(nSnakeHeadXInWorldSpace + 2 * nPlayerVel[i].first, nSnakeHeadYInWorldSpace + 1, olc::BLACK);
		}
		else
		{
			// Facing up or down instead...
			Draw(nSnakeHeadXInWorldSpace - 2, nSnakeHeadYInWorldSpace + 2 * nPlayerVel[i].second, olc::BLACK);
			Draw(nSnakeHeadXInWorldSpace + 1, nSnakeHeadYInWorldSpace + 2 * nPlayerVel[i].second, olc::BLACK);
		}

		return;
	}

	void DrawMenu()
	{
		Clear(olc::VERY_DARK_GREY);

		playerSegments[0].clear();

		// S
		nPlayerPos[0] = { 1, 5 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);
		
		nPlayerVel[0] = { -1, 0 };
		DrawPlayer(0);
		playerSegments[0].clear();

		// N
		nPlayerPos[0] = { 5, 5 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);
		
		nPlayerVel[0] = { 0, 1 };
		DrawPlayer(0);
		playerSegments[0].clear();

		// A
		nPlayerPos[0] = { 9, 5 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);
		
		nPlayerVel[0] = { 0, 1 };
		DrawPlayer(0);
		playerSegments[0].clear();

		nPlayerPos[0] = { 10, 3 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);

		nPlayerVel[0] = { 0, -1 };
		DrawPlayer(0);
		playerSegments[0].clear();

		// K
		nPlayerPos[0] = { 13, 5 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);

		nPlayerVel[0] = { 0, 1 };
		DrawPlayer(0);
		playerSegments[0].clear();

		nPlayerPos[0] = { 15, 5 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--; nPlayerPos[0].first--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--; nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);

		DrawPlayer(0);
		playerSegments[0].clear();

		// E
		nPlayerPos[0] = { 19, 5 };
		
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].second--;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]); nPlayerPos[0].first++;
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);

		nPlayerVel[0] = { 1, 0 };
		DrawPlayer(0);
		playerSegments[0].clear();

		nPlayerPos[0] = { 18, 3 };
		playerSegments[0].emplace(playerSegments[0].begin(), nPlayerPos[0]);

		DrawPlayer(0);
		playerSegments[0].clear();

		DrawMenuText();

		return;
	}

	void DrawMenuText()
	{
		FillRect(0, nTileSize * 10, ScreenWidth(), nTileSize * 6, olc::VERY_DARK_GREY);

		DrawString(nTileSize * 5, nTileSize * 10, "-- M E N U --", olc::YELLOW);

		olc::Pixel pMenuColour = olc::GREEN;

		if (ax[0] == 13 || ax[0] == 14)
		{
			DrawString(nTileSize * (ax[0] + 1), nTileSize * 12, "-> Players   " + std::to_string(nCurrentPlayerCount), pMenuColour);
			DrawString(nTileSize * (ax[0] + 1), nTileSize * 13, "-> Apples    " + std::to_string(nCurrentAppleCount), pMenuColour);
			DrawString(nTileSize * (ax[0] + 1), nTileSize * 14, "-> Start Game", pMenuColour);

			pMenuColour = olc::DARK_GREEN;
		}

		DrawString(nTileSize * 5, nTileSize * 12, "-> Classic", pMenuColour);
		DrawString(nTileSize * 5, nTileSize * 13, "-> Arcade", pMenuColour);
		DrawString(nTileSize * 5, nTileSize * 14, "-> Settings", pMenuColour);
		DrawString(nTileSize * 5, nTileSize * 15, "-> Exit", pMenuColour);

		return;
	}

public:
	bool OnUserCreate() override
	{
		// Initialise the map and player variables
		srand((unsigned int)time(0));
		ResetGame();

		nGameState = GS_RESET;
		nNextState = GS_RESET;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// User input (store players next move as a temporary velocity so we only modify our real velocity when the snake actually moves - stops player from reversing direction)
		// Player 1 (index 0)
		if (GetKey(olc::Key::UP).bPressed && nPlayerVel[0].second != 1)
			nPlayerNextVel[0] = { 0, -1 };

		if (GetKey(olc::Key::DOWN).bPressed && nPlayerVel[0].second != -1)
			nPlayerNextVel[0] = { 0, 1 };

		if (GetKey(olc::Key::LEFT).bPressed && nPlayerVel[0].first != 1)
			nPlayerNextVel[0] = { -1, 0 };

		if (GetKey(olc::Key::RIGHT).bPressed && nPlayerVel[0].first != -1)
			nPlayerNextVel[0] = { 1, 0 };

		// Player 2 (index 1)
		if (GetKey(olc::Key::W).bPressed && nPlayerVel[1].second != 1)
			nPlayerNextVel[1] = { 0, -1 };

		if (GetKey(olc::Key::S).bPressed && nPlayerVel[1].second != -1)
			nPlayerNextVel[1] = { 0, 1 };

		if (GetKey(olc::Key::A).bPressed && nPlayerVel[1].first != 1)
			nPlayerNextVel[1] = { -1, 0 };

		if (GetKey(olc::Key::D).bPressed && nPlayerVel[1].first != -1)
			nPlayerNextVel[1] = { 1, 0 };

		if (GetKey(olc::Key::ENTER).bPressed)
		{
			switch (ay[0])
			{
			case 12: // Classic
			{
				if (ax[0] == 4)
				{
					ax[0] = 14;
					nNextState = GS_DRAW_MENU;
				}
			}
			break;

			case 13: // Arcade
			{
				if (ax[0] == 4)
				{
					ax[0] = 13;
					ay[0] = 12;
					nNextState = GS_DRAW_MENU;
				}
			}
			break;

			case 14: // Settings
			{
				if (ax[0] == 14) nNextState = GS_GAME_START;
			}
			break;

			case 15: // Exit
			{
				if (ax[0] == 4) bExitFlag = true;			
			}
			break;

			}
		}

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			if (nGameState == GS_GAME_PLAY)
				nNextState = GS_RESET;

			if (nGameState == GS_MENU)
			{
				if (ax[0] > 4)
					nNextState = GS_RESET;
				else
					bExitFlag = true;
			}
		}


		switch (nGameState)
		{
		case GS_RESET:
		{
			ax[0] = 4; ay[0] = 12;
			nNextState = GS_DRAW_MENU;
		}
		break;

		case GS_DRAW_MENU:
		{
			DrawMenu();
			nNextState = GS_MENU;
		}
		break;

		case GS_MENU:
		{
			FillCircle(ax[0] * nTileSize + 2, ay[0] * nTileSize + 2, 8, olc::VERY_DARK_GREY);

			ay[0] += nPlayerNextVel[0].second;
			if (ay[0] < 12) ay[0]++;
			if ((ay[0] > 15 && ax[0] == 4) || (ay[0] > 14 && (ax[0] == 13 || ax[0] == 14))) ay[0]--;

			if (ax[0] > 4 && ay[0] == 12)
			{
				nCurrentPlayerCount += nPlayerNextVel[0].first;
				if (nCurrentPlayerCount > nPlayerCount) nCurrentPlayerCount = nPlayerCount;
				if (nCurrentPlayerCount < 1) nCurrentPlayerCount = 1;
				DrawMenuText();
			}

			if (ax[0] > 4 && ay[0] == 13)
			{
				nCurrentAppleCount += nPlayerNextVel[0].first;
				if (nCurrentAppleCount > nAppleCount) nCurrentAppleCount = nAppleCount;
				if (nCurrentAppleCount < 1) nCurrentAppleCount = 1;
				DrawMenuText();
			}

			FillCircle(ax[0] * nTileSize + 2, ay[0] * nTileSize + 2, 8, olc::GREEN);

			nPlayerNextVel[0] = { 0, 0 };
			nPlayerVel[0] = { 0, 0 };

			nGameState = nNextState;
		}
		break;

		case GS_GAME_START:
		{
			ResetGame();
			nNextState = GS_GAME_PLAY;
		}
		break;

		case GS_GAME_PLAY:
		{
		// Move Player, draw things and process game logic (only needs to be done once we reach our next frame - saves drawing over the same screen for no reason!)
		fGameSpeedCounter += fElapsedTime * fPlayerSpeed;
		if (fGameSpeedCounter >= 1.0)
		{
			for (int i = 0; i < nPlayerCount; i++)
			{
				// break if we reach our current player limit
				if (i == nCurrentPlayerCount) break;

				// Allow velocity to effect the snakes location
				nPlayerVel[i] = nPlayerNextVel[i];

				// increment the head of the snakes position
				nPlayerPos[i].first += nPlayerVel[i].first;
				nPlayerPos[i].second += nPlayerVel[i].second;

				// wrap around the screen
				if (nPlayerPos[i].first < 0) nPlayerPos[i].first = nMapSize - 1;
				if (nPlayerPos[i].first > nMapSize - 1) nPlayerPos[i].first = 0;
				if (nPlayerPos[i].second < 0) nPlayerPos[i].second = nMapSize - 1;
				if (nPlayerPos[i].second > nMapSize - 1) nPlayerPos[i].second = 0;

				// Test to see if we collided with our tail
				for (int c = 0; c < nPlayerCount; c++)
				{
					if (c == nCurrentPlayerCount) break;

					for (auto const &p : playerSegments[i])
						if (nPlayerPos[c].first == p.first && nPlayerPos[c].second == p.second)
							ResetGame();
				}


				// Our head is at the end of our array, so we push the new player location to the back of our snake array
				// Then we erase the first element in the array (which is the end of our tail).  This effectively shifts
				// the snake forward while maintaining its body shape.
				playerSegments[i].push_back(nPlayerPos[i]);

				// Only erase the tail if it is fully visible (useful for when player spawns)
				if (playerSegments[i].size() > nTailLength[i]) playerSegments[i].erase(playerSegments[i].begin());

				// Test to see if our head has eaten an apple
				for (int a = 0; a < nAppleCount; a++)
				{
					if (a == nCurrentAppleCount) break;

					if (ax[a] == nPlayerPos[i].first && ay[a] == nPlayerPos[i].second)
					{
						// Reset our apple, increase our tail size, and speed up a little bit
						if (at[a] < nAppleTimeOut)
							nPlayerScore[i]++;

						ResetApple(a, i);
						playerSegments[i].emplace(playerSegments[i].begin(), nPlayerPos[i]);
						fPlayerSpeed += 0.05f;
					}
				}

			}

			// Apple Stuff
			for (int a = 0; a < nAppleCount; a++)
			{
				// break if we reach our current apple count
				if (a == nCurrentAppleCount) break;

				if (at[a] >= nAppleTimeOut * 3)
					ResetApple(a, -1);

				at[a]++;
			}

			// Clear Screen
			Clear(olc::VERY_DARK_GREY);

			// Draw Apples
			for (int a = 0; a < nAppleCount; a++)
			{
				if (a == nCurrentAppleCount) break;

				if (at[a] < nAppleTimeOut)
					FillCircle(ax[a] * nTileSize + nTileSize * 0.5, ay[a] * nTileSize + nTileSize * 0.5, nTileSize * 0.5, olc::GREEN);
				else
					FillCircle(ax[a] * nTileSize + nTileSize * 0.5, ay[a] * nTileSize + nTileSize * 0.5, nTileSize * 0.4, olc::YELLOW);

			}

			// Draw Player
			for (int i = 0; i < nPlayerCount; i++)
			{
				if (i == nCurrentPlayerCount) break;

				DrawPlayer(i);
					
				// Draw Score
				DrawString(nTileSize, nTileSize * (i + 1), std::to_string(nPlayerScore[i]), playerColour[i]);
			}
			// reset our counter so we can wait for the next movement frame
			fGameSpeedCounter = 0.0f;
		}

		}
		break;

		case GS_GAME_OVER:
		{

		}
		break;

		}

		
		// Exit the game
		if (bExitFlag) return false;

		// Update state machine
		nGameState = nNextState;
		

		return true;
	}
};

int main()
{
	Game snake;
	if (snake.Construct(nScreenSize, nScreenSize, 2, 2))
		snake.Start();

	return 0;
}