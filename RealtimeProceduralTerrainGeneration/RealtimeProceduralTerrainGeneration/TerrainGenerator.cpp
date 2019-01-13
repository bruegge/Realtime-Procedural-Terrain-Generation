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
	std::vector<CModel::SDataVBO> resultVBO(m_nWidth * m_nHeight);

	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight; ++y)
		{
			CModel::SDataVBO sVertex;
			GLfloat fXPosition = static_cast<GLfloat>(x) / static_cast<GLfloat>(m_nWidth - 1);
			GLfloat fYPosition = static_cast<GLfloat>(y) / static_cast<GLfloat>(m_nHeight - 1);

			sVertex.SetData((fXPosition-0.5f)*10.0f, (fYPosition-0.5f)*10.0f, 0, fXPosition, fYPosition, 0, 0, 0);
			resultVBO[m_nHeight * x + y] = sVertex;
		}
	}
	
	std::vector<unsigned int> resultIBO;
	for (unsigned int x = 0; x < m_nWidth-1; ++x)
	{
		for (unsigned int y = 0; y < m_nHeight-1; ++y)
		{
			resultIBO.push_back(x * m_nHeight + y);
			resultIBO.push_back(x * m_nHeight + y + 1);
			resultIBO.push_back((x + 1) * m_nHeight + y);

			resultIBO.push_back(x * m_nHeight + y + 1);
			resultIBO.push_back((x + 1) * m_nHeight + y + 1);
			resultIBO.push_back((x + 1) * m_nHeight + y);
		}
	}
	std::pair<std::vector<CModel::SDataVBO>, std::vector<unsigned int>> resultTotal;
	resultTotal.first = resultVBO;
	resultTotal.second = resultIBO;
	return resultTotal;
}

unsigned int CTerrainGenerator::GetWidth()
{
	return m_nWidth;
}

unsigned int CTerrainGenerator::GetHeight()
{
	return m_nHeight;
}

