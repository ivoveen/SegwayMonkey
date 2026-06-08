#include<bullet/btBulletDynamicsCommon.h>

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#ifdef RASPBERRY
#include "bcm_host.h"
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <vector>
// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Graphics.h"
#include "ObjectShader.h"
//imgui
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "WorldManager.h"
#include "GameStateGame.h"
#include "GameStateMainMenu.h"
#include "GameStatePause.h"
#include "GameStateLose.h"


using namespace glm;

//---------------------------------------------------------------------------------------------
//Credit:
// This code is very much based on the "Hello cubes" program made by brian.
// I have restructered almost all of the code and removed some pieces however,
// most of the code is still directly from brian's program.
// In the future I will keep reworking and changing the project to my needs.
//---------------------------------------------------------------------------------------------


int main() {
	////---------------------------------------------------------------------------------------------
	////initial set up code:
	////---------------------------------------------------------------------------------------------
	Graphics mGraphics;  // create a 1 time instance of a graphics class in Global space
	Target_State state;
	Target_State* p_state = &state;

	mGraphics.GetDisplay();
	mGraphics.esInitContext(p_state);
	mGraphics.init_ogl(p_state, 1024, 768, 1024, 768); // this is the window size we set , you can set it to anything, the 3rd and 4th params are for older pis'
	////1024, 768, 1024, 768
	//// 640, 480, 640, 480

	////--------------------------------------------
	//// imgui setup
	const char* glsl_version = "#version 100";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init(glsl_version);

	ObjectShader shaderManager;
	shaderManager.InitShaders("Object", ObjectShader::Object);
	shaderManager.InitShaders("Debug", ObjectShader::Debug);
	shaderManager.InitShaders("Terrain", ObjectShader::Terrain);


	struct timeval t1, t2;
	struct timezone tz;
	float deltaTime = 0;
	std::vector<int>* keys;

	gettimeofday(&t1, &tz);

	enum GameState {
		MainMenu, Pause, Game, Lose
	}mGameState = MainMenu;
	int difficulty = 1;

	GameStateGame* theGame = new GameStateGame();
	theGame->Init(&mGraphics, &shaderManager, p_state, difficulty);

	GameStateMainMenu theMainMenu = GameStateMainMenu(&mGraphics, &shaderManager, p_state);
	GameStatePause thePauseMenu = GameStatePause(&mGraphics, &shaderManager, p_state);
	GameStateLose theloseMenu = GameStateLose(&mGraphics, &shaderManager, p_state);

	int returnValue;
	int score;
	//-------------------------------------------------------------
	//Game loop
	while (1) {
		//get delta time
		gettimeofday(&t2, nullptr);
		deltaTime = ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) * 0.000001f);
		t1 = t2;

		//Input
		mGraphics.updateMousePos(deltaTime, p_state->width, p_state->height);
		keys = mGraphics.UpdateKeyboardInput();

		switch (mGameState)
		{
		case MainMenu:
			returnValue = theMainMenu.Update(deltaTime);
			if (returnValue) {
				mGameState = Game;
				difficulty = returnValue;
				delete theGame;
				theGame = new GameStateGame();
				theGame->Init(&mGraphics, &shaderManager, p_state, difficulty);
			}

			break;
		case Pause:
			returnValue = thePauseMenu.Update(deltaTime);
			switch (returnValue)
			{
			case 1:
				mGameState = MainMenu;
				break;
			case 2:
				mGameState = Game;
			default:
				break;
			}

			break;
		case Game:
			returnValue = theGame->update(deltaTime, keys);
			if (returnValue) {
				score = returnValue;
				mGameState = Lose;
			}

			//when pressing escape the game will restart.
			for (int i = 0; i < keys->size(); i++) {
				if (keys->at(i) == XK_Escape) {

					mGameState = Pause;
				}
			}
			break;
		case Lose:
			returnValue = theloseMenu.Update(deltaTime, score);
			switch (returnValue)
			{
			case 1:
				mGameState = MainMenu;

				break;
			case 2:
				mGameState = Game;
				delete theGame;
				theGame = new GameStateGame();
				theGame->Init(&mGraphics, &shaderManager, p_state, difficulty);
			default:
				break;
			}
			break;
		default:
			break;
		}







	}
	return 0;
}

