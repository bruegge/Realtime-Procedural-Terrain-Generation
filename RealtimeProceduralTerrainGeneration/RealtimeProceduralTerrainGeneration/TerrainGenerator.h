#pragma once
#include <vector>
#include "Model.h"
#include <stdlib.h>

class CTerrainGenerator
{
public:
	CTerrainGenerator(unsigned int nWidth, unsigned int nHeight);
	~CTerrainGenerator();

	std::pair<std::vector<CModel::SDataVBO>,std::vector<GLuint>> GenerateMeshData();
	
	unsigned int GetWidth();
	unsigned int GetHeight();
	std::vector<GLfloat>* GetDataHeight();
	std::vector<GLfloat>* GetData1stDerivative();
	std::vector<GLfloat>* GetData2ndDerivative();
	std::vector<GLfloat>* GetData2ndDerivativeAccumulated();

	void GenerateVoronoi(unsigned int nCount);
	void GenerateErosion(unsigned int nSteps);
	void GenerateNoise();
	void GenerateDerivatives();


private:
	float GetTerrainHeight(unsigned int x, unsigned int y);
	glm::vec3 Get1stDerivative(unsigned int x, unsigned int y);
	glm::vec3 Get2ndDerivative(unsigned int x, unsigned int y);
	glm::vec3 Get2ndDerivativeAccumulated(unsigned int x, unsigned int y);
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	std::vector<GLfloat> m_vecDataSetHeight;
	std::vector<GLfloat> m_vecData1stDerivative;
	std::vector<GLfloat> m_vecData2ndDerivative;
	std::vector<GLfloat> m_vecData2ndDerivativeAccumulated;


};

