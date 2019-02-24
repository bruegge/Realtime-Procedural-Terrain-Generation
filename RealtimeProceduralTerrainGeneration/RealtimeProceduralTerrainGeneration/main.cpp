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
unsigned int nTerrainWidth = 256;
unsigned int nTerrainGridWidth = 128;
CModel* pTerrain;
CShader* pShader;
CTexture* pTextureGrass;
CTexture* pTextureSnow;
CTexture* pTextureCliff;

CCamera* pCamera;
CTerrainGenerator* pTerrainGenerator;
CGLFWWindow* pWindow;

void loadContent() //load all objects and fill them
{
	srand(time(NULL));
	//create 100 cubes
	pTerrain = new CModel(nTerrainWidth, nTerrainGridWidth);
	pTerrainGenerator = new CTerrainGenerator(nTerrainWidth, nTerrainGridWidth);
	std::pair<std::vector<CModel::SDataVBO>, std::vector<GLuint>> meshData = pTerrainGenerator->GenerateMeshData();

	pTerrain->SetVBOandIBOData(&(meshData.first), &(meshData.second));
	//create camera
	pCamera = new CCamera(90, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f, glm::vec3(0, 0, 7), glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
	//create shader program
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	pShader = CShader::createShaderProgram("../shaders/VS_vertex.glsl", 
											"../shaders/TCS_tessellationControl.glsl",
											"../shaders/TES_tessellationEvaluation.glsl",
											nullptr, 
											"../shaders/FS_fragment.glsl");
	//create and load texture
	pTextureGrass = new CTexture("../textures/grass.bmp");
	pTextureCliff = new CTexture("../textures/cliff.bmp");
	pTextureSnow = new CTexture("../textures/snow.bmp");

	//bind keys to the objects operations
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::XAxis, std::bind(&CCamera::TurnX, pCamera, std::placeholders::_1));
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::YAxis, std::bind(&CCamera::TurnY, pCamera, std::placeholders::_1));
	
	pCamera->AddKeyBinding(GLFW_KEY_W, std::bind(&CCamera::Forward, pCamera, std::placeholders::_1));
	pCamera->AddKeyBinding(GLFW_KEY_S, std::bind(&CCamera::Backward, pCamera, std::placeholders::_1));
	pCamera->AddKeyBinding(GLFW_KEY_A, std::bind(&CCamera::TurnX, pCamera, -1));
	pCamera->AddKeyBinding(GLFW_KEY_D, std::bind(&CCamera::TurnX, pCamera, 1));

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
		
		//check to change terrain
		if (glfwGetKey(window->getWindowPointer(), GLFW_KEY_3) == GLFW_PRESS)
		{
			pTerrainGenerator->GenerateHeightMapCPU(7,50); 
		}
		if (glfwGetKey(window->getWindowPointer(), GLFW_KEY_4) == GLFW_PRESS)
		{
			pTerrainGenerator->GenerateHeightMapGPU(7, 50);
		}
	
		pShader->bind();
		
		pTerrainGenerator->GetHeightMap()->Link(pShader, 0, "textureTerrain");
		pTerrainGenerator->GetNormalMap()->Link(pShader, 1, "textureTerrainNormal");
		pTerrainGenerator->Get2ndDerivativeMap()->Link(pShader, 2, "textureTerrain2ndDerAcc");
		pTextureGrass->Link(pShader, 3, "textureGrass");
		pTextureCliff->Link(pShader, 4, "textureCliff");
		pTextureSnow->Link(pShader, 5, "textureSnow");

		pTerrainGenerator->GetHeightMap()->Bind(0);
		pTerrainGenerator->GetNormalMap()->Bind(1);
		pTerrainGenerator->Get2ndDerivativeMap()->Bind(2);
		pTextureGrass->Bind(3); 
		pTextureCliff->Bind(4);
		pTextureSnow->Bind(5);

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
	delete pTextureCliff;
	delete pTextureGrass;
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