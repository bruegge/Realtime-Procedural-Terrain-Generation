#pragma once
#include <vector>
#include "Model.h"
#include <stdlib.h>
#include "Texture.h"
#include "Shader.h"

class CTerrainGenerator : public CKeyManager
{
public:
	CTerrainGenerator(unsigned int nWidth, unsigned int nGridWidth);
	~CTerrainGenerator();

	std::pair<std::vector<CModel::SDataVBO>,std::vector<GLuint>> GenerateMeshData();
	
	unsigned int GetWidth();
	std::vector<GLfloat>* GetDataHeight();
	
	void GenerateHeightMapCPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps);
	void GenerateHeightMapGPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps);
	void ApplyChangesToTextures();
	CTexture* GetHeightMap();
	CTexture* GetNormalMap();
	CTexture* Get2ndDerivativeMap();
	CTexture* GetTerrainTexture();

private:
	
	void GenerateVoronoi(unsigned int nCount);
	void GenerateErosion(unsigned int nSteps);
	void GenerateErosion2(unsigned int nSteps);
	void GenerateNoise();
	void GenerateDerivatives();
	void GenerateTextureDistribution();

	float GetTerrainHeight(unsigned int x, unsigned int y);
	unsigned int m_nWidth;
	unsigned int m_nGridWidth;

	std::vector<GLfloat> m_vecDataSetHeight;
	std::vector<CTexture*> m_vecTextures;
	CShader* m_pShaderNormalAnd2ndDerivative;
	CShader* m_pShaderAccumulate2ndDerivative;
	CShader* m_pShaderVoronoi;
	CShader* m_pShaderNoise;
	CShader* m_pShaderErosion;
	CShader* m_pShaderSetTextures;
};

