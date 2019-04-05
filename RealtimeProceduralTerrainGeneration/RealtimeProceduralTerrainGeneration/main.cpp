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
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../ImGUI/imgui_impl_opengl3.h"

int windowWidth = 800;
int windowHeight = 600;
unsigned int nTerrainWidth = 256;
unsigned int nTerrainGridWidth = 256;
CModel* pTerrain;
CShader* pShader;
CTexture* pTextureMaterial;

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
	pCamera = new CCamera(90, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.01f, 1000.0f, glm::vec3(0, 0, 7), glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
	//create shader program
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	pShader = CShader::createShaderProgram("../shaders/VS_vertex.glsl", 
											"../shaders/TCS_tessellationControl.glsl",
											"../shaders/TES_tessellationEvaluation.glsl",
											nullptr, 
											"../shaders/FS_fragment.glsl");
	//create and load texture
	pTextureMaterial = new CTexture();	
	pTextureMaterial->LoadMaterials();

	//bind keys to the objects operations
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::XAxis, std::bind(&CCamera::TurnX, pCamera, std::placeholders::_1));
	pCamera->AddMouseAxisBinding(CKeyManager::EMouseAxis::YAxis, std::bind(&CCamera::TurnY, pCamera, std::placeholders::_1));
	
	pCamera->AddKeyBinding(GLFW_KEY_W, std::bind(&CCamera::Forward, pCamera, std::placeholders::_1));
	pCamera->AddKeyBinding(GLFW_KEY_S, std::bind(&CCamera::Backward, pCamera, std::placeholders::_1));
	pCamera->AddKeyBinding(GLFW_KEY_A, std::bind(&CCamera::TurnX, pCamera, -1));
	pCamera->AddKeyBinding(GLFW_KEY_D, std::bind(&CCamera::TurnX, pCamera, 1));
	
	{ //GUI
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(pWindow->getWindowPointer(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

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
		
		pTerrainGenerator->GetHeightMap()->Link(pShader, 0, "textureTerrain");
		pTerrainGenerator->GetNormalMap()->Link(pShader, 1, "textureTerrainNormal");
		pTerrainGenerator->Get2ndDerivativeMap()->Link(pShader, 2, "textureTerrain2ndDerAcc");
		pTerrainGenerator->GetTerrainTexture()->Link(pShader, 3, "textureTerrainTexture");
		pTextureMaterial->LinkMaterial(pShader, 4, 5, "textureMaterialColorSpecular", "textureMaterialNormalHeight");
		
		pTerrainGenerator->GetHeightMap()->Bind(0);
		pTerrainGenerator->GetNormalMap()->Bind(1);
		pTerrainGenerator->Get2ndDerivativeMap()->Bind(2);
		pTerrainGenerator->GetTerrainTexture()->Bind(3);
		pTextureMaterial->BindMaterial(4, 5);
		
		//draw terrain	
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		pTerrain->draw(pShader, pCamera); //draws the triangle with the given shader
	
		{ //imGUI
			ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.
			if (ImGui::Button("Generate new Terrain with GPU"))
			{
				pTerrainGenerator->GenerateHeightMapGPU(7, 50);
			}
			if (ImGui::Button("Generate new Terrain with CPU"))
			{
				pTerrainGenerator->GenerateHeightMapCPU(7, 50);
			}
		
			bool bEnableWireframe = pTerrain->IsWireFrameEnabled();
			ImGui::Checkbox("Enable WireFrames", &bEnableWireframe);      // Edit bools storing our window open/close state
			if (bEnableWireframe != pTerrain->IsWireFrameEnabled())
			{
				pTerrain->EnableWireFrame(bEnableWireframe);
			}
			
			bool bEnableNormalMapping = pTerrain->IsNormalMappingEnabled();
			ImGui::Checkbox("Enable Normal mapping", &bEnableNormalMapping);      // Edit bools storing our window open/close state
			if (bEnableNormalMapping != pTerrain->IsNormalMappingEnabled())
			{
				pTerrain->EnableNormalMapping(bEnableNormalMapping);
			}
		
			bool bEnableTessellation = pTerrain->IsTessellationEnabled();
			ImGui::Checkbox("Enable Tessellation", &bEnableTessellation);      // Edit bools storing our window open/close state
			if (bEnableTessellation != pTerrain->IsTessellationEnabled())
			{
				pTerrain->EnableTessellation(bEnableTessellation);
			}

			bool bEnableBezier = pTerrain->IsBezierSurfaceEnabled();
			ImGui::Checkbox("Enable Bezier surface", &bEnableBezier);      // Edit bools storing our window open/close state
			if (bEnableBezier != pTerrain->IsBezierSurfaceEnabled())
			{
				pTerrain->EnableBezierSurface(bEnableBezier);
			}
			
			/*ImGui::Text("Number instances: %i", pSettings->m_nInfoInstanceCount);               // Display some text (you can use a format strings too)
			ImGui::Text("ComputeTime CPU: %f ms", pSettings->m_dComputeTimeCPU);               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Enable face culling", &(pSettings->m_bEnableCullFace));      // Edit bools storing our window open/close state
			ImGui::Checkbox("Enable QuadTree update", &(pSettings->m_bEnableQuadTreeUpdate));      // Edit bools storing our window open/close state
			ImGui::Checkbox("Enable TJunction elimination", &(pSettings->m_bEnableTJunctionElimination));      // Edit bools storing our window open/close state

			int nRadioButtonValue;
			ImGui::RadioButton("Number 1", &nRadioButtonValue, 1);
			ImGui::RadioButton("Number 2", &nRadioButtonValue, 2);

			float fGridSize = static_cast<float>(pSettings->m_nGeometrySize);
			ImGui::SliderFloat("single grid size", &fGridSize, 2.0f, 33.0f);
			pSettings->m_nGeometrySize = static_cast<unsigned int>(fGridSize);
			pSettings->m_nGeometrySize += 1 - pSettings->m_nGeometrySize % 2;


			float fMaxQuadTreeDepth = static_cast<float>(pSettings->m_nMaxQuadTreeDepth);
			ImGui::SliderFloat("QuadTree Max Depth", &fMaxQuadTreeDepth, 0.0f, 20.0f);
			pSettings->m_nMaxQuadTreeDepth = static_cast<unsigned int>(fMaxQuadTreeDepth);

			ImGui::SliderFloat("QuadTree Division Angle", &pSettings->m_fTileDivisionAngle, 90.0f, 6.0f);

			float fPerlinNoiseCount = static_cast<float>(pSettings->m_nPerlinNoiseCount);
			ImGui::SliderFloat("Number iterations perlin noise", &fPerlinNoiseCount, 0.0f, 30.0f);
			pSettings->m_nPerlinNoiseCount = static_cast<unsigned int>(fPerlinNoiseCount);

			*/
			//ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
			ImGui::End();
	
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	delete pTerrain;
	delete pTerrainGenerator;
	delete pShader;
	delete pTextureMaterial;
	delete pCamera;
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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