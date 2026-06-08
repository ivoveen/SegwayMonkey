#include "GameStatePause.h"
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
#include "Camera.h"
#include "Player.h"
#include "ObjectShader.h"
//imgui
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include"Obstacle.h"

GameStatePause::GameStatePause(Graphics* graphics, ObjectShader* shaderManager, Target_State* pState) {
	mGraphics = graphics;
	p_state = pState;
	mpShaderManager = shaderManager;


	mpCamera = new Camera();

	//player setup
	glm::vec3 Pos = glm::vec3(77.5f, 3.0f, 10);
	mPlayer = new Player(Pos, mpShaderManager, (char*)"./Resources/Models/pitfallMonkeySegway.obj", "Object");
	mPlayer->scales = glm::vec3(1.0f, 1.0f, 1.0f);
	mPlayer->SetYRotation(PI);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(p_state->width, p_state->height);
	io.FontGlobalScale = 4.0f;

}
int GameStatePause::Update(float deltaTime) {
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 3.0f;

	//imgui init
	io.DeltaTime = deltaTime;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
	// Setup the viewport
	glViewport(0, 0, p_state->width, p_state->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bullet simulate
	//-------------------------------------------------------
	// ImGUI window
		// Set the window to be transparent
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBackground;

	// Set the window position and size
	ImVec2 window_pos(0, 0);
	ImVec2 window_size(p_state->width, p_state->height);

	ImGui::SetNextWindowPos(window_pos);
	ImGui::SetNextWindowSize(window_size);

	// Begin the ImGui window
	ImGui::Begin("Main Menu", nullptr, window_flags);

	// Center the play button in the window
	ImVec2 buttonSize(200, 80);
	ImVec2 textPos((window_size.x - ImGui::CalcTextSize("Monkey   Segway").x) * 0.5f, 9.0f);
	ImGui::SetCursorPos(textPos);
	ImGui::Text("Monkey   Segway");
	//easy button
	ImVec2 buttonPos((window_size.x - buttonSize.x) * 0.5f, (window_size.y - (buttonSize.y * 3.5f)) * 0.5f);
	// Draw the play button
	if (ImGui::IsMouseHoveringRect(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y))) {
		ImGui::GetWindowDrawList()->AddRectFilled(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y), IM_COL32(0, 70, 200, 255));
	}
	else {
		ImGui::GetWindowDrawList()->AddRectFilled(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y), IM_COL32(0, 120, 255, 255));
	}
	ImGui::GetWindowDrawList()->AddText(buttonPos, IM_COL32(255, 255, 255, 255), "Main menu", nullptr);
	// Check for button click
	if (ImGui::IsMouseReleased(0) && ImGui::IsMouseHoveringRect(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y))) {
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glFlush();
		eglSwapBuffers(p_state->display, p_state->surface);
		return 1;
	}


	//normal button
	buttonPos = ImVec2((window_size.x - buttonSize.x) * 0.5f, (window_size.y - buttonSize.y) * 0.5f);
	// Draw the play button
	if (ImGui::IsMouseHoveringRect(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y))) {
		ImGui::GetWindowDrawList()->AddRectFilled(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y), IM_COL32(0, 70, 200, 255));
	}
	else {
		ImGui::GetWindowDrawList()->AddRectFilled(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y), IM_COL32(0, 120, 255, 255));
	}
	ImGui::GetWindowDrawList()->AddText(buttonPos, IM_COL32(255, 255, 255, 255), "Continue", nullptr);
	// Check for button click
	if (ImGui::IsMouseReleased(0) && ImGui::IsMouseHoveringRect(buttonPos, ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y))) {
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glFlush();
		eglSwapBuffers(p_state->display, p_state->surface);
		return 2;
	}

	// End the ImGui window
	ImGui::End();
	//-----------------------------------------------------------
	// Game loop
	//
	mpCamera->Update(keys);
	glm::mat4 VP = mpCamera->MakeVPMatrix();
	mpShaderManager->use(ObjectShader::Object);
	mpShaderManager->setVec3("cameraPosition", mpCamera->worldPosition);
	mpShaderManager->setVec3("lightPosition", vec3(400, 30, 400));
	mpShaderManager->setFloat("ambientPercent", ambientPercent);
	mpShaderManager->setMat4("VP", VP);
	mpShaderManager->setFloat("radius", radius);
	mpShaderManager->setFloat("fogDistance", fogDistance);
	mPlayer->Update();
	mPlayer->Draw();

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
	return 0;
}