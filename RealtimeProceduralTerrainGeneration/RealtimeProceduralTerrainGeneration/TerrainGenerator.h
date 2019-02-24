#pragma once
#include <vector>
#include "Model.h"
#include <stdlib.h>
#include "Texture.h"
#include "Shader.h"

class CTerrainGenerator
{
public:
	CTerrainGenerator(unsigned int nWidth, unsigned int nGridWidth);
	~CTerrainGenerator();

	std::pair<std::vector<CModel::SDataVBO>,std::vector<GLuint>> GenerateMeshData();
	
	unsigned int GetWidth();
	std::vector<GLfloat>* GetDataHeight();
	std::vector<GLfloat>* GetData1stDerivative();
	std::vector<GLfloat>* GetData2ndDerivative();
	std::vector<GLfloat>* GetData2ndDerivativeAccumulated();

	void GenerateHeightMapCPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps);
	void GenerateHeightMapGPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps);
	void ApplyChangesToTextures();
	CTexture* GetHeightMap();
	CTexture* GetNormalMap();
	CTexture* Get2ndDerivativeMap();

private:
	
	void GenerateVoronoi(unsigned int nCount);
	void GenerateErosion(unsigned int nSteps);
	void GenerateNoise();
	void GenerateDerivatives();

	float GetTerrainHeight(unsigned int x, unsigned int y);
	glm::vec4 Get1stDerivative(unsigned int x, unsigned int y);
	glm::vec2 Get2ndDerivative(unsigned int x, unsigned int y);
	glm::vec2 Get2ndDerivativeAccumulated(unsigned int x, unsigned int y);
	unsigned int m_nWidth;
	unsigned int m_nGridWidth;

	std::vector<GLfloat> m_vecDataSetHeight;
	std::vector<GLfloat> m_vecData1stDerivative;
	std::vector<GLfloat> m_vecData2ndDerivative;
	std::vector<GLfloat> m_vecData2ndDerivativeAccumulated;
	std::vector<CTexture*> m_vecTextures;
	CShader* m_pShaderNormalAnd2ndDerivative;
	CShader* m_pShaderAccumulate2ndDerivative;
	CShader* m_pShaderVoronoi;
	CShader* m_pShaderNoise;
	CShader* m_pShaderErosion;
};

