#include "TerrainGenerator.h"

CTerrainGenerator::CTerrainGenerator(unsigned int nWidth, unsigned int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

CTerrainGenerator::~CTerrainGenerator()
{
}

std::pair<std::vector<CModel::SDataVBO>, std::vector<GLuint>> CTerrainGenerator::GenerateMeshData()
{
	float fGridWidth = 100;
	float fGridHeight = 100;
	std::vector<CModel::SDataVBO> resultVBO(fGridWidth * fGridHeight);

	for (unsigned int x = 0; x < fGridWidth; ++x)
	{
		for (unsigned int y = 0; y < fGridHeight; ++y)
		{
			CModel::SDataVBO sVertex;
			GLfloat fXPosition = static_cast<GLfloat>(x) / static_cast<GLfloat>(fGridWidth - 1);
			GLfloat fYPosition = static_cast<GLfloat>(y) / static_cast<GLfloat>(fGridHeight - 1);

			sVertex.SetData((fXPosition-0.5f)*10.0f, (fYPosition-0.5f)*10.0f, 0, fXPosition, fYPosition, 0, 0, 0);
			resultVBO[fGridHeight * x + y] = sVertex;
		}
	}
	
	std::vector<unsigned int> resultIBO((fGridWidth -1) * (fGridHeight - 1)*4);
	unsigned int nVectorPosition = 0;
	for (unsigned int x = 0; x < fGridWidth -1; ++x)
	{
		for (unsigned int y = 0; y < fGridHeight -1; ++y)
		{
			resultIBO[nVectorPosition++] = (x + 1) * fGridHeight + y + 1;
			resultIBO[nVectorPosition++] = (x + 1) * fGridHeight + y;
			resultIBO[nVectorPosition++] = x * fGridHeight + y + 1;
			resultIBO[nVectorPosition++] = x * fGridHeight + y;
		}
	}
	std::pair<std::vector<CModel::SDataVBO>, std::vector<unsigned int>> resultTotal;
	resultTotal.first = resultVBO;
	resultTotal.second = resultIBO;
	return resultTotal;
}

std::vector<GLfloat>* CTerrainGenerator::GetDataHeight()
{
	return &m_vecDataSetHeight;
}

std::vector<GLfloat>* CTerrainGenerator::GetData1stDerivative()
{
	return &m_vecData1stDerivative;
}

std::vector<GLfloat>* CTerrainGenerator::GetData2ndDerivative()
{
	return &m_vecData2ndDerivative;
}

std::vector<GLfloat>* CTerrainGenerator::GetData2ndDerivativeAccumulated()
{
	return &m_vecData2ndDerivativeAccumulated;
}

void CTerrainGenerator::GenerateVoronoi(unsigned int nCount)
{
	std::vector<glm::vec2> vecRandomNumbers(nCount);
	for (unsigned int i = 0; i < nCount; i++)
	{
		vecRandomNumbers[i].x = rand() % m_nWidth;
		vecRandomNumbers[i].y = rand() % m_nHeight;
	}
	float FDiagonal = sqrt(m_nWidth*m_nWidth + m_nHeight*m_nHeight)/5.0f;
	
	for (unsigned int x = 0; x < m_nWidth; x++)
	{
		for (unsigned int y = 0; y < m_nHeight; y++)
		{
			glm::vec2 position = glm::vec2(x,y);
			float fDistance1stClosest = 100;
			float fDistance2ndClosest = 100;
			
			for each(glm::vec2 var in vecRandomNumbers)
			{
				float fDistance = glm::length(var - position)/fDiagonal;
				fDistance = std::fmin(fDistance, glm::length(var - glm::vec2(m_nWidth, 0) - position)/fDiagonal);
				fDistance = std::fmin(fDistance, glm::length(var + glm::vec2(m_nWidth, 0) - position)/fDiagonal);
				fDistance = std::fmin(fDistance, glm::length(var - glm::vec2(0, m_nHeight) - position)/fDiagonal);
				fDistance = std::fmin(fDistance, glm::length(var + glm::vec2(0, m_nHeight) - position)/fDiagonal);
				if (fDistance < fDistance1stClosest)
				{
					fDistance2ndClosest = fDistance1stClosest;
					fDistance1stClosest = fDistance;
				}
				else if (fDistance < fDistance2ndClosest)
				{
					fDistance2ndClosest = fDistance;
				}
			}
			m_vecDataSetHeight[(x*m_nHeight + y)] = (-fDistance1stClosest + fDistance2ndClosest);
		}
	}
}

void CTerrainGenerator::GenerateErosion(unsigned int nSteps)
{

}

float CTerrainGenerator::GetTerrainHeight(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nHeight - 1);
	return m_vecDataSetHeight[(x*m_nHeight + y)];
}

glm::vec3 CTerrainGenerator::Get1stDerivative(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nHeight - 1);
	float f1 = m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 0];
	float f2 = m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 1];
	float f3 = m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 2];

	return glm::vec3(f1, f2, f3);
}

glm::vec3 CTerrainGenerator::Get2ndDerivative(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nHeight - 1);
	float f1 = m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 0];
	float f2 = m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 1];
	float f3 = m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 2];

	return glm::vec3(f1, f2, f3);
}

glm::vec3 CTerrainGenerator::Get2ndDerivativeAccumulated(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nHeight - 1);
	float f1 = m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 0];
	float f2 = m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 1];
	float f3 = m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 2];

	return glm::vec3(f1, f2, f3);
}

void CTerrainGenerator::GenerateNoise()
{
	m_vecDataSetHeight.resize(m_nHeight*m_nWidth);
	float fFrequence = 10;
	float fPI = 3.1415926f;
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight; ++y)
		{
			float fHeight = 0.5f + sin(static_cast<float>(x) / static_cast<float>(m_nWidth) * 2 * fPI * fFrequence) / 4 +cos(static_cast<float>(y) / static_cast<float>(m_nHeight) * 2 * fPI * fFrequence) / 4;
			if (x > 20 && x < 40)
			{
				if (y > 20 && y < 40)
				{
					fHeight = (x-20)/20.0f;
				}
			}
			m_vecDataSetHeight[(x*m_nHeight + y)] = fHeight;
		}
	}
}

void CTerrainGenerator::GenerateDerivatives()
{
	m_vecData1stDerivative.resize(m_nHeight*m_nWidth * 3);
	m_vecData2ndDerivative.resize(m_nHeight*m_nWidth * 3);
	m_vecData2ndDerivativeAccumulated.resize(m_nHeight*m_nWidth * 3);
	float fPixelDistance = 10.0f / m_nWidth;
	//derivative 1st order
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight; ++y)
		{
			float fHeightX_1 = GetTerrainHeight(x - 1, y);
			float fHeightX1 = GetTerrainHeight(x + 1, y);
			float fHeightY_1 = GetTerrainHeight(x, y - 1);
			float fHeightY1 = GetTerrainHeight(x, y + 1);

			float xDir = (fHeightX_1 - fHeightX1) / fPixelDistance;
			float yDir = (fHeightY_1 - fHeightY1) / fPixelDistance;
	
			m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 0] = xDir;
			m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 1] = yDir;
			m_vecData1stDerivative[(x*m_nHeight + y) * 3 + 2] = 0;
		}
	}
	//derivative 2nd order
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight; ++y)
		{
			glm::vec3 NormalX_1 = Get1stDerivative(x - 1, y);
			glm::vec3 NormalX1 = Get1stDerivative(x + 1, y);
			glm::vec3 NormalY_1 = Get1stDerivative(x, y - 1);
			glm::vec3 NormalY1 = Get1stDerivative(x, y + 1);

			float xDir = (NormalX_1.x - NormalX1.x) / fPixelDistance;
			float yDir = (NormalY_1.y - NormalY1.y) / fPixelDistance;

			m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 0] = xDir;
			m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 1] = yDir;
			m_vecData2ndDerivative[(x*m_nHeight + y) * 3 + 2] = 0;
		}
	}
	//accumulate the 2nd derivative
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight; ++y)
		{
			glm::vec3 derivative2nd = Get2ndDerivative(x, y);
			glm::vec3 derivative2ndX_1Acc = Get2ndDerivativeAccumulated(x - 1, y);
			glm::vec3 derivative2ndY_1Acc = Get2ndDerivativeAccumulated(x, y - 1);

			m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 0] = derivative2ndX_1Acc.x + abs(derivative2nd.x);
			m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 1] = derivative2ndY_1Acc.y + abs(derivative2nd.y);
			m_vecData2ndDerivativeAccumulated[(x*m_nHeight + y) * 3 + 2] = 0;
		}
	}
}

unsigned int CTerrainGenerator::GetWidth()
{
	return m_nWidth;
}

unsigned int CTerrainGenerator::GetHeight()
{
	return m_nHeight;
}

