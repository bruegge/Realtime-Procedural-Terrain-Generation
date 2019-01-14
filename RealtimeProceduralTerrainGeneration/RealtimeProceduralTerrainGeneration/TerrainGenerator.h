#pragma once
#include <vector>
#include "Model.h"
class CTerrainGenerator
{
public:
	CTerrainGenerator(unsigned int nWidth, unsigned int nHeight);
	~CTerrainGenerator();

	std::pair<std::vector<CModel::SDataVBO>,std::vector<GLuint>> GenerateMeshData();
	
	unsigned int GetWidth();
	unsigned int GetHeight();
	std::vector<GLubyte>* GetDataSet();
	void GenerateVoronoi(unsigned int nCount);
	void GenerateErosion(unsigned int nSteps);
	void GenerateNoise();


private:
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	std::vector<GLubyte> m_vecDataSet;
};

