#include <glad\glad.h>
#include <iostream>
#include <fstream>
#include <numeric>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <glfw3.h>
#include <time.h> 
#include "GLFWWindow.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

int windowWidth = 800;
int windowHeight = 600;

std::vector<CModel*> vecpTriangle;
CShader* pShader;
CTexture* pTexture;
CCamera* pCamera;

CGLFWWindow* pWindow;

void loadContent() //load all objects and fill them
{
	srand(time(NULL));
	//create 100 cubes
	vecpTriangle.push_back(new CModel(glm::vec3(rand() % 30 - 15, rand() % 30 - 15, rand() % 30 - 15), glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50)));
	for (int i = 1; i < 100; ++i)
	{
		vecpTriangle.push_back(new CModel(vecpTriangle[0], glm::vec3(rand() % 30 - 15, rand() % 30 - 15, rand() % 30 - 15), glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50)));
	}
	//create camera
	pCamera = new CCamera(90, 800.0f / 600.0f, 0.1f, 1000.0f, glm::vec3(0, 0, -20), glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
	//create shader program
	pShader = CShader::createShaderProgram("../shaders/vertex.glsl", nullptr, nullptr, nullptr, "../shaders/fragment.glsl");
	//create and load texture
	pTexture = new CTexture("../textures/texture1.bmp");
	
	//bind keys to the objects operations
	for (int i = 0; i < vecpTriangle.size(); ++i)
	{
		vecpTriangle[i]->AddKeyBinding(GLFW_KEY_1, std::bind(&CModel::EnableAnimation, vecpTriangle[i]));
		vecpTriangle[i]->AddKeyBinding(GLFW_KEY_2, std::bind(&CModel::DisableAnimation, vecpTriangle[i]));
		vecpTriangle[i]->AddKeyBinding(GLFW_KEY_E, std::bind(&CModel::IncreaseAnimationSpeed, vecpTriangle[i]));
		vecpTriangle[i]->AddKeyBinding(GLFW_KEY_Q, std::bind(&CModel::DecreaseAnimationSpeed, vecpTriangle[i]));
	}
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::XAxis, std::bind(&CCamera::TurnX, pCamera, std::placeholders::_1));
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::YAxis, std::bind(&CCamera::TurnY, pCamera, std::placeholders::_1));
	
	pCamera->AddKeyBinding(GLFW_KEY_W, std::bind(&CCamera::Forward, pCamera));
	pCamera->AddKeyBinding(GLFW_KEY_S, std::bind(&CCamera::Backward, pCamera));
	
	std::cout << "Content loaded" << std::endl;
}

void programLoop(CGLFWWindow* window)
{
	auto start = std::chrono::steady_clock::now();
	glClearColor(0.1f, 0.1f, 0.02f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	bool bClose = false;
	do //render loop
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color and the depth buffer


		bClose = window->IO(); //check key inputs
		
		pTexture->Bind(pShader, 0, "texture1");
		//draw all cubes
		for (int i = 0; i < vecpTriangle.size(); ++i)
		{
			vecpTriangle[i]->draw(pShader, pCamera); //draws the triangle with the given shader
		}

		//swaps the backbuffer with the frontbuffer to show the result
		glfwSwapBuffers(window->getWindowPointer());

		//time measurement
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> diff = end - start;
		start = std::chrono::steady_clock::now();
		double dTimeDiff = diff.count();

	} while (bClose == false);
}

void deleteContent() //delete the objects
{
	for (int i = 0; i < vecpTriangle.size(); ++i)
	{
		delete vecpTriangle[i];
	}
	delete pShader;
	delete pTexture;
	delete pCamera;
}

int main()
{
	std::cout << "Program started" << std::endl;
	pWindow = new CGLFWWindow(1024,768); //create the window
	loadContent();
	programLoop(pWindow);
	deleteContent();
	delete pWindow;
	return 0;
}