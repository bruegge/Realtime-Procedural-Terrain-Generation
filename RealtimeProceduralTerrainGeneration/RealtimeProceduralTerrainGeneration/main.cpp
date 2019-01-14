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
#include "TerrainGenerator.h"

int windowWidth = 800;
int windowHeight = 600;

CModel* pTerrain;
CShader* pShader;
CTexture* pTextureTerrain;
CTexture* pTextureGrass;
CTexture* pTextureSnow;
CTexture* pTextureCliff;
CTexture* pTextureMud;

CCamera* pCamera;
CTerrainGenerator* pTerrainGenerator;
CGLFWWindow* pWindow;

void loadContent() //load all objects and fill them
{
	srand(time(NULL));
	//create 100 cubes
	pTerrain = new CModel();
	pTerrainGenerator = new CTerrainGenerator(224, 224);
	std::pair<std::vector<CModel::SDataVBO>, std::vector<GLuint>> meshData = pTerrainGenerator->GenerateMeshData();

	pTerrain->SetVBOandIBOData(&(meshData.first), &(meshData.second));
	//create camera
	pCamera = new CCamera(90, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f, glm::vec3(0, 0, 7), glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
	//create shader program
	pShader = CShader::createShaderProgram("../shaders/vertex.glsl", nullptr, nullptr, "../shaders/geometry.glsl", "../shaders/fragment.glsl");
	//create and load texture
	pTextureTerrain = new CTexture("../textures/testTerrain.bmp");
	pTextureCliff = new CTexture("../textures/cliff.bmp");
	pTextureGrass = new CTexture("../textures/grass.bmp");
	pTextureMud = new CTexture("../textures/mud.bmp");
	pTextureSnow = new CTexture("../textures/snow.bmp");

	//bind keys to the objects operations
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::XAxis, std::bind(&CCamera::TurnX, pCamera, std::placeholders::_1));
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::YAxis, std::bind(&CCamera::TurnY, pCamera, std::placeholders::_1));
	
	pCamera->AddKeyBinding(GLFW_KEY_W, std::bind(&CCamera::Forward, pCamera, std::placeholders::_1));
	pCamera->AddKeyBinding(GLFW_KEY_S, std::bind(&CCamera::Backward, pCamera, std::placeholders::_1));
	
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
		
		pShader->bind();
		pTextureTerrain->Link(pShader, 0, "texture1");
		pTextureCliff->Link(pShader, 1, "textureCliff");
		pTextureGrass->Link(pShader, 2, "textureGrass");
		pTextureMud->Link(pShader, 3, "textureMud");
		pTextureSnow->Link(pShader, 4, "textureSnow");

		pTextureTerrain->Bind(0);
		pTextureCliff->Bind(1);
		pTextureGrass->Bind(2);
		pTextureMud->Bind(3);
		pTextureSnow->Bind(4);

		//draw terrain	
		pTerrain->draw(pShader, pCamera); //draws the triangle with the given shader
	
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
	delete pTerrain;
	delete pTerrainGenerator;
	delete pShader;
	delete pTextureTerrain;
	delete pTextureCliff;
	delete pTextureGrass;
	delete pTextureMud;
	delete pTextureSnow;
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