#include "GameStateGame.h"
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
#include "GameObject.h"
#include "Camera.h"
#include "BulletManager.h"
#include "Player.h"
#include "ObjectShader.h"
//imgui
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "PhysicsObj.h"
#include "WorldManager.h"
#include "Zone.h"
#include "ContactCallback.h"

#include"Obstacle.h"
using namespace glm;


void GameStateGame::Init(Graphics* graphics, ObjectShader* shaderManager, Target_State* pState, int difficulty) {
	mGraphics = graphics;
	p_state = pState;
	mpShaderManager = shaderManager;
	//--------------------------------------------
	// bullet setup
	mpBulletManager = new BulletManager();
	mpBulletManager->MyPhysicsDraw->setObjectShader(mpShaderManager);


	//std::vector<GameObject*> MyObjects;
	mpCamera = new Camera();

	//player setup
	glm::vec3 Pos = glm::vec3(77.5f, -2.0f, 10.0f);
	mPlayer = new Player(Pos, mpBulletManager, mpShaderManager, (char*)"./Resources/Models/pitfallMonkeySegway.obj", "Object");
	mPlayer->scales = glm::vec3(1.0f, 1.0f, 1.0f);
	mPlayer->CreatePhysicsObj(mpBulletManager, mPlayer->CreateMyShape(ObjectModel::CONVEXHULL), 1);
	mPlayer->MyPhysObj->GetRigidBody()->setAngularFactor(btVector3(0, 1, 0));
	mPlayer->MyPhysObj->GetRigidBody()->setLinearFactor(btVector3(1, 1, 0));

	//----------------------------------------
	// WorldManager setup;
	mWorldManager = new WorldManager(mpShaderManager, mpBulletManager, difficulty, mPlayer);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(p_state->width, p_state->height);
	io.FontGlobalScale = 1.0f;

}

GameStateGame::~GameStateGame() {
	delete mWorldManager;
	delete mpCamera;
	delete mPlayer;
	delete mpBulletManager;

}


int GameStateGame::update(float deltaTime, std::vector<int>* keys) {
	ImGuiIO& io = ImGui::GetIO();

	//imgui init
	io.DeltaTime = deltaTime;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
	// Setup the viewport
	glViewport(0, 0, static_cast<GLuint>(p_state->width), static_cast<GLuint>(p_state->height));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//bullet simulate
	if (simulation_running)mpBulletManager->DynamicPhysicsWorld->stepSimulation(deltaTime);





#ifdef DEBUG
	io.FontGlobalScale = 1.0f;

	//-------------------------------------------------------
	// ImGUI window
	ImGui::Begin("General Info");
	ImGui::SliderFloat("difficulty", &mWorldManager->difficultyFactor, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::Text("Chunk: %i", mWorldManager->mSelectedChunkID);
	ImGui::Text("Zone: %i", mWorldManager->mSelectedZoneID);
	vec3 color = mWorldManager->mWorldZones[mWorldManager->mSelectedZoneID]->mColor;
	ImGui::Text("Zone Color: %f, %f, %f", color.x, color.y, color.z);

	if (ImGui::CollapsingHeader("General & Input"))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("General"))
		{
			ImGui::Text("DeltaTime: %f", deltaTime);
			ImGui::Text("FPS: %f", fps);
			ImGui::Checkbox("Simulate", &simulation_running);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Debug Draw", &drawDebugLines);      // Edit bools storing our window open/close state
			ImGui::TreePop();
		}
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Shader"))
		{
			ImGui::SliderFloat("radius", &radius, 500.0f, 5000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("fog distance", &fogDistance, 0.0f, 1000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Ambient Percent", &ambientPercent, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Inputs"))
		{
			ImGui::Text("player rotatY: %f", mPlayer->rotations.y);
			vec3 pos = mPlayer->GetPosition();
			ImGui::Text("player pos: %f, %f, %f ", pos.x, pos.y, pos.z);
			ImGui::Text("player grounded: %i", mPlayer->grounded);
			ImGui::Text("player goingDown: %i", mPlayer->goingDown);

			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Mouse pos: <INVALID>");
			ImGui::Text("Mouse down:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
			ImGui::TreePop();
		}
	}
	ImGui::End();
#else
	io.FontGlobalScale = 2.0f;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBackground;

	ImGuiWindowFlags window_flagsDebug = ImGuiWindowFlags_None;
	// Set the window position and size
	ImVec2 window_pos(0, 0);
	ImVec2 window_size(p_state->width, p_state->height);

	ImGui::SetNextWindowPos(window_pos);
	ImGui::SetNextWindowSize(window_size);

	ImGui::Begin("General Info", nullptr, window_flags);
	ImVec2 textPos = ImVec2((2), (0));
	ImGui::SetCursorPos(textPos);
	ImGui::Text("Current Zone: %s", mWorldManager->mWorldZones[mWorldManager->mCurrentZoneID]->mZoneName.c_str());

	textPos = ImVec2((window_size.x - ImGui::CalcTextSize("Score: 100000").x), (0));
	ImGui::SetCursorPos(textPos);
	ImGui::Text("Score: %i", static_cast<int>(mWorldManager->mScore));

	ImGui::End();
#endif

	//-----------------------------------------------------------
	// Game loop
	//
	mpCamera->Update(keys);
	if (simulation_running)mPlayer->UpdateMovement(deltaTime, keys);
	glm::mat4 VP = mpCamera->MakeVPMatrix();

	mpShaderManager->use(ObjectShader::Terrain);
	mpShaderManager->setVec3("cameraPosition", mpCamera->worldPosition);
	mpShaderManager->setVec3("lightPosition", vec3(400, 30, 400));
	mpShaderManager->setFloat("ambientPercent", ambientPercent);
	mpShaderManager->setMat4("VP", VP);
	mpShaderManager->setFloat("radius", radius);
	mpShaderManager->setFloat("fogDistance", fogDistance);
	mWorldManager->Update(deltaTime);
	mWorldManager->Draw();


	mpShaderManager->use(ObjectShader::Object);
	mpShaderManager->setVec3("cameraPosition", mpCamera->worldPosition);
	mpShaderManager->setVec3("lightPosition", vec3(400, 30, 400));
	mpShaderManager->setFloat("ambientPercent", ambientPercent);
	mpShaderManager->setMat4("VP", VP);
	mpShaderManager->setFloat("radius", radius);
	mpShaderManager->setFloat("fogDistance", fogDistance);
	mWorldManager->DrawObjects();

	mPlayer->Update();
	mPlayer->Draw();

	//mObstacle->Update();
	//mObstacle->Draw();
	//--------------------------------------------------------------------------
	//bullet
	//
	if (drawDebugLines) {
		mpBulletManager->DynamicPhysicsWorld->debugDrawWorld();
		mpShaderManager->use(ObjectShader::Debug);
		mpShaderManager->setMat4("Matrix", VP);
		mpBulletManager->MyPhysicsDraw->DoDebugDraw(); // useful but very slow, take note! 
	}
	// --------------------------------------------------------------------------
	// imgui
	//
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glFlush();
	eglSwapBuffers(p_state->display, p_state->surface);
	// report frame rate
	totalTime += deltaTime;
	frames++;
	if (totalTime > 1.0f)
	{
		fps = frames / totalTime;
		//printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totalTime, frames / totalTime);  // uncomment to see frames
		totalTime -= 1.0f;
		frames = 0;
	}

	//check if gameOver
	if (mWorldManager->collisionCallback.gameOver) {
		return mWorldManager->mScore;
	}
	else {
		return 0;
	}
}