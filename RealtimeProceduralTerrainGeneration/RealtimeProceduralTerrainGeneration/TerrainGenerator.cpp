#include "TerrainGenerator.h"
#include <iostream>

CTerrainGenerator::CTerrainGenerator(unsigned int nWidth, unsigned int nGridWidth)
{
	m_nWidth = nWidth;
	m_nGridWidth = nGridWidth;

	//create Textures
	for (unsigned int i = 0; i < 4; ++i) //0: Heightmap  //1: normalMap //2: 2nd derivative
	{
		m_vecTextures.push_back(new CTexture());
	}
	//create Compute_Shaders
	m_pShaderNormalAnd2ndDerivative = CShader::createComputeShaderProgram("../shaders/CS_NormalAnd2ndDerivative.glsl");
	m_pShaderAccumulate2ndDerivative = CShader::createComputeShaderProgram("../shaders/CS_Accumulate2ndDerivative.glsl");
	m_pShaderNoise = CShader::createComputeShaderProgram("../shaders/CS_Noise.glsl");
	m_pShaderVoronoi = CShader::createComputeShaderProgram("../shaders/CS_Voronoi.glsl");
	m_pShaderErosion = CShader::createComputeShaderProgram("../shaders/CS_Erosion.glsl");
	m_pShaderSetTextures = CShader::createComputeShaderProgram("../shaders/CS_SetTexturePositions.glsl");
}

CTerrainGenerator::~CTerrainGenerator()
{
	delete m_vecTextures[3];
	delete m_vecTextures[2];
	delete m_vecTextures[1];
	delete m_vecTextures[0];
	m_vecTextures.clear();
}

CTexture* CTerrainGenerator::GetHeightMap()
{
	return m_vecTextures[0];
}

CTexture* CTerrainGenerator::GetNormalMap()
{
	return m_vecTextures[1];
}

CTexture* CTerrainGenerator::Get2ndDerivativeMap()
{
	return m_vecTextures[2];
}

CTexture* CTerrainGenerator::GetTerrainTexture()
{
	return m_vecTextures[3];
}

void CTerrainGenerator::GenerateHeightMapCPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps)
{
	GenerateNoise();
	GenerateVoronoi(nCountVoronoiPoints);
	GenerateErosion(nErosionSteps);
	ApplyChangesToTextures();
	GenerateDerivatives();
}

void CTerrainGenerator::GenerateHeightMapGPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps)
{
	m_vecTextures[0]->SetTextureData(m_nWidth, m_nWidth, 1, nullptr, false);
	m_vecTextures[1]->SetTextureData(m_nWidth, m_nWidth, 4, nullptr, false);
	m_vecTextures[2]->SetTextureData(m_nWidth, m_nWidth, 4, nullptr, false);
	m_vecTextures[3]->SetTextureData(m_nWidth, m_nWidth, 1, nullptr, false, false);


	glBindImageTexture(0, m_vecTextures[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, m_vecTextures[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, m_vecTextures[2]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, m_vecTextures[3]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

	GLfloat vecRandomNumbers[100];
	for (unsigned int i = 0; i< 100; ++i)
	{
		vecRandomNumbers[i] = rand() % m_nWidth;
	}
	m_pShaderNoise->bind();
	glDispatchCompute(m_nWidth, m_nWidth, 1);
	glUniform1f(glGetUniformLocation(m_pShaderNoise->getID(), "fWidth"), m_nWidth);

	m_pShaderVoronoi->bind();
	glUniform1f(glGetUniformLocation(m_pShaderVoronoi->getID(), "fWidth"), m_nWidth);
	GLint nLocationRandomCount = glGetUniformLocation(m_pShaderVoronoi->getID(), "randomCount");
	glUniform1f(nLocationRandomCount, nCountVoronoiPoints);
	GLint nLocationRandomVector = glGetUniformLocation(m_pShaderVoronoi->getID(), "random");
	glUniform1fv(nLocationRandomVector, 100, vecRandomNumbers);
	glDispatchCompute(m_nWidth, m_nWidth, 1);
	
	m_pShaderErosion->bind();
	glUniform1f(glGetUniformLocation(m_pShaderErosion->getID(), "fWidth"), m_nWidth);
	glDispatchCompute(m_nWidth, m_nWidth, 1);

	GenerateDerivatives();

	glBindImageTexture(0, m_vecTextures[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, m_vecTextures[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, m_vecTextures[2]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, m_vecTextures[3]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

	m_pShaderSetTextures->bind();
	glUniform1f(glGetUniformLocation(m_pShaderSetTextures->getID(), "randomCount"), 100);
	glUniform1f(glGetUniformLocation(m_pShaderSetTextures->getID(), "fTextureCount"), 27);
	nLocationRandomVector = glGetUniformLocation(m_pShaderSetTextures->getID(), "random");
	glUniform1fv(nLocationRandomVector, 100, vecRandomNumbers);

	glDispatchCompute(m_nWidth, m_nWidth, 1);

	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void CTerrainGenerator::ApplyChangesToTextures()
{
	std::vector<GLfloat>* pTerrainHeight = GetDataHeight();
	m_vecTextures[0]->SetTextureData(m_nWidth, m_nWidth, 1, pTerrainHeight, false);
}

std::pair<std::vector<CModel::SDataVBO>, std::vector<GLuint>> CTerrainGenerator::GenerateMeshData()
{
	std::vector<CModel::SDataVBO> resultVBO(m_nGridWidth * m_nGridWidth);

	for (unsigned int x = 0; x < m_nGridWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nGridWidth; ++y)
		{
			CModel::SDataVBO sVertex;
			GLfloat fXPosition = static_cast<GLfloat>(x) / static_cast<GLfloat>(m_nGridWidth - 1);
			GLfloat fYPosition = static_cast<GLfloat>(y) / static_cast<GLfloat>(m_nGridWidth - 1);

			sVertex.SetData((fXPosition-0.5f)*10.0f, (fYPosition-0.5f)*10.0f, 0, fXPosition, fYPosition, 0, 0, 0);
			resultVBO[m_nGridWidth * x + y] = sVertex;
		}
	}
	
	std::vector<unsigned int> resultIBO((m_nGridWidth -1) * (m_nGridWidth - 1)*4);
	unsigned int nVectorPosition = 0;
	for (unsigned int x = 0; x < m_nGridWidth -1; ++x)
	{
		for (unsigned int y = 0; y < m_nGridWidth -1; ++y)
		{
			resultIBO[nVectorPosition++] = (x + 1) * m_nGridWidth + y + 1;
			resultIBO[nVectorPosition++] = (x + 1) * m_nGridWidth + y;
			resultIBO[nVectorPosition++] = x * m_nGridWidth + y + 1;
			resultIBO[nVectorPosition++] = x * m_nGridWidth + y;
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

void CTerrainGenerator::GenerateVoronoi(unsigned int nCount)
{
	std::vector<glm::vec2> vecRandomNumbers(nCount);
	for (unsigned int i = 0; i< nCount; ++i)
	{
		vecRandomNumbers[i].x = rand() % m_nWidth;
		vecRandomNumbers[i].y = rand() % m_nWidth;
	}

	float fDiagonal = sqrt(m_nWidth * m_nWidth + m_nWidth * m_nWidth) / 5.0f;

	float fWidth = m_nWidth;

	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			glm::vec2 position = glm::vec2(x, y);
			float fDistance1stClosest = 100;
			float fDistance2ndClosest = 100;

			for each (glm::vec2 var in vecRandomNumbers)
			{
				float fDistance = glm::length(var - position) / fDiagonal;
				
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						glm::vec2 difference = var + glm::vec2(i * fWidth, j * fWidth) - position;
						float fDistanceCalculated = glm::length(difference) / fDiagonal;
						fDistance = std::fmin(fDistance, fDistanceCalculated);
					}
				}

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
			m_vecDataSetHeight[(x*m_nWidth + y)] = (-fDistance1stClosest +fDistance2ndClosest);
		}
	}
}

void CTerrainGenerator::GenerateErosion(unsigned int nSteps)
{
	float di[3][3];
	float hi[3][3];
	float c = 0.5f;
	float N = m_nWidth;
	float T = 4.0f / N;
	
	for (int step = 0; step < nSteps; ++step);
	{

		int indexXDmax = 0;
		int indexYDmax = 0;
		float dTotal = 0;

		for (int i = 1; i < m_nWidth-1; ++i)
		{
			for (int j = 1; j < m_nWidth-1; ++j)
			{
				//calculate neighborhoodHeights
				float h = GetTerrainHeight(i, j);
				for (int x = 0; x < 3; ++x)
				{
					for (int y = 0; y < 3; ++y)
					{
						hi[x][y] = GetTerrainHeight(i + (x - 1), j + (y - 1));
						di[x][y] = h - GetTerrainHeight(i + (x - 1), j + (y - 1));

						if (hi[x][y] > hi[indexXDmax][indexYDmax])
						{
							indexXDmax = x;
							indexYDmax = y;
						}
						if (di[x][y] > T)
						{
							dTotal += di[x][y];
						}
					}
				}

				for (int x = 0; x < 3; ++x)
				{
					for (int y = 0; y < 3; ++y)
					{
						hi[x][y] = hi[x][y] + c * (hi[indexXDmax][indexYDmax] - T) * di[x][y] / dTotal;
						m_vecDataSetHeight[((i + x - 1)*m_nWidth + (j + y - 1))] = hi[x][y];
					}
				}

				
			}
		}
	}
	//GenerateErosion2(nSteps);
}

void CTerrainGenerator::GenerateErosion2(unsigned int nSteps)
{
	float* W = new float[m_nWidth * m_nWidth];
	float* M = new float[m_nWidth * m_nWidth];
	float Ke = 0.5f;
	float Kc = 0.01f;
	float Kr = 0.01f;
	float Ks = 0.01f;
	float a = 0;

	for (int i = 0; i < m_nWidth; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			W[i *m_nWidth + j] = 0;
			M[i *m_nWidth + j] = 0;
		}
	}

	for (int step = 0; step < nSteps; ++step)
	{

		for (int i = 0; i < m_nWidth; i++)
		{
			for (int j = 0; j < m_nWidth; j++)
			{
				W[i *m_nWidth + j] += Kr;
				M[i *m_nWidth + j] += Ks*W[i *m_nWidth + j];
				m_vecDataSetHeight[i *m_nWidth + j] -= Ks * W[i *m_nWidth + j];
				if (m_vecDataSetHeight[i *m_nWidth + j] > 10)
				{
					int muh = 0;
					muh++;
				}
			}
		}

		for (int i = 1; i < m_nWidth - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				float ai = 0;
				float di = 0;
				float aEverage = 0;
				float dTotal = 0.0;

				float a = m_vecDataSetHeight[i *m_nWidth + j] + W[i *m_nWidth +j];
				int countPositives = 0;
				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{
						aEverage += (W[(i + x - 1) *m_nWidth + (j + y - 1)] + m_vecDataSetHeight[(i + x - 1) *m_nWidth + (j + y - 1)]) / 9.0f;
						float ai = m_vecDataSetHeight[(i + x - 1) *m_nWidth + (j + y - 1)] + W[(i + x - 1) *m_nWidth + (j + y - 1)];
						float di = a - ai;
						if (di > 0)
						{
							countPositives++;
							dTotal += di;
						}
						if (x == 2 && y == 2 && dTotal == 0.0f)
						{
							int muh = 0; 
							muh++;
						}
					}
				}

				float deltaA = a - aEverage;

				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{
						float ai = m_vecDataSetHeight[(i + x - 1) *m_nWidth + (j + y - 1)] + W[(i + x - 1) *m_nWidth + (j + y - 1)];;
						float di = a - ai;

						float deltaW = 0;
						if (dTotal != 0)
						{
							deltaW = std::fminf(W[i *m_nWidth + j], deltaA) * di / dTotal;
						}
						else
						{
							int muh = 0; 
							muh++;
						}

						if (W[i *m_nWidth + j] != 0)
						{
							M[(i + x - 1) *m_nWidth + (j + y - 1)] += M[i*m_nWidth + j] * deltaW / W[i *m_nWidth + j];
						}
						
					}
				}

				W[i*m_nWidth + j] *= (1 - Ke);
				float mMax = Kc * W[i*m_nWidth + j];
				float deltaM = std::fmax(0, M[i*m_nWidth + j] - mMax);
				M[i*m_nWidth + j] -= deltaM;
				m_vecDataSetHeight[i*m_nWidth + j] += deltaM;
				if (m_vecDataSetHeight[i *m_nWidth + j] > 10)
				{
					int muh = 0;
					muh++;
				}
			}
		}
	}
	delete[] W;
	delete[] M;
}


float CTerrainGenerator::GetTerrainHeight(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nWidth - 1);
	return m_vecDataSetHeight[(x*m_nWidth + y)];
}

void CTerrainGenerator::GenerateNoise()
{
	m_vecDataSetHeight.resize(m_nWidth*m_nWidth);
	float fFrequence = 10;
	float fPI = 3.1415926f;
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			float fHeight = 0.5f + sin(static_cast<float>(x) / static_cast<float>(m_nWidth) * 2 * fPI * fFrequence) / 4 +cos(static_cast<float>(y) / static_cast<float>(m_nWidth) * 2 * fPI * fFrequence) / 4;
			if (x > 20 && x < 40)
			{
				if (y > 20 && y < 40)
				{
					fHeight = (x-20)/20.0f;
				}
			}
			m_vecDataSetHeight[(x*m_nWidth + y)] = fHeight;
		}
	}
}

void CTerrainGenerator::GenerateDerivatives()
{

	glBindImageTexture(0, m_vecTextures[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, m_vecTextures[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, m_vecTextures[2]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	m_pShaderNormalAnd2ndDerivative->bind();
	glUniform1f(glGetUniformLocation(m_pShaderNormalAnd2ndDerivative->getID(), "fWidth"), m_nWidth);
	glDispatchCompute(m_nWidth, m_nWidth, 1);

	m_pShaderAccumulate2ndDerivative->bind();
	glUniform1f(glGetUniformLocation(m_pShaderAccumulate2ndDerivative->getID(), "fWidth"), m_nWidth);
	glUniform1f(glGetUniformLocation(m_pShaderAccumulate2ndDerivative->getID(), "direction"), 0.0f);
	glDispatchCompute(m_nWidth, 1, 1);
	glUniform1f(glGetUniformLocation(m_pShaderAccumulate2ndDerivative->getID(), "direction"), 1.0f);
	glDispatchCompute(m_nWidth, 1, 1);

	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

unsigned int CTerrainGenerator::GetWidth()
{
	return m_nWidth;
}



