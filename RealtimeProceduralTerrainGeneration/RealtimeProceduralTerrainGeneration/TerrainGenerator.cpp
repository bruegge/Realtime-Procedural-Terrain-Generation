#include "TerrainGenerator.h"
#include <iostream>

CTerrainGenerator::CTerrainGenerator(unsigned int nWidth, unsigned int nGridWidth)
{
	m_nWidth = nWidth;
	m_nGridWidth = nGridWidth;

	//create Textures
	for (unsigned int i = 0; i < 3; ++i) //0: Heightmap  //1: normalMap //2: 2nd derivative
	{
		m_vecTextures.push_back(new CTexture());
	}
	//create Compute_Shaders
	m_pShaderNormalAnd2ndDerivative = CShader::createComputeShaderProgram("../shaders/CS_NormalAnd2ndDerivative.glsl");
	m_pShaderAccumulate2ndDerivative = CShader::createComputeShaderProgram("../shaders/CS_Accumulate2ndDerivative.glsl");
	m_pShaderNoise = CShader::createComputeShaderProgram("../shaders/CS_Noise.glsl");
	m_pShaderVoronoi = CShader::createComputeShaderProgram("../shaders/CS_Voronoi.glsl");
	m_pShaderErosion = CShader::createComputeShaderProgram("../shaders/CS_Erosion.glsl");
}

CTerrainGenerator::~CTerrainGenerator()
{
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

void CTerrainGenerator::GenerateHeightMapCPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps)
{
	GenerateNoise();
	GenerateVoronoi(nCountVoronoiPoints);
	GenerateErosion(nErosionSteps);
	GenerateDerivatives();
	ApplyChangesToTextures();
}

void CTerrainGenerator::GenerateHeightMapGPU(unsigned int nCountVoronoiPoints, unsigned int nErosionSteps)
{
	m_vecTextures[0]->SetTextureData(m_nWidth, m_nWidth, 1, nullptr, false);
	m_vecTextures[1]->SetTextureData(m_nWidth, m_nWidth, 4, nullptr, false);
	m_vecTextures[2]->SetTextureData(m_nWidth, m_nWidth, 2, nullptr, false);


	glBindImageTexture(0, m_vecTextures[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(1, m_vecTextures[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, m_vecTextures[2]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);

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
	glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
}

void CTerrainGenerator::ApplyChangesToTextures()
{
	std::vector<GLfloat>* pTerrainHeight = GetDataHeight();
	m_vecTextures[0]->SetTextureData(m_nWidth, m_nWidth, 1, pTerrainHeight, false);
	std::vector<GLfloat>* pTerrainNormal = GetData1stDerivative();
	m_vecTextures[1]->SetTextureData(m_nWidth, m_nWidth, 4, pTerrainNormal, false);
	std::vector<GLfloat>* pTerrain2ndDerivative = GetData2ndDerivativeAccumulated();
	m_vecTextures[2]->SetTextureData(m_nWidth, m_nWidth, 2, pTerrain2ndDerivative, false);
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

}

float CTerrainGenerator::GetTerrainHeight(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nWidth - 1);
	return m_vecDataSetHeight[(x*m_nWidth + y)];
}

glm::vec4 CTerrainGenerator::Get1stDerivative(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nWidth - 1);
	float f1 = m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 0];
	float f2 = m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 1];
	float f3 = m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 2];
	float f4 = m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 3];

	return glm::vec4(f1, f2, f3, f4);
}

glm::vec2 CTerrainGenerator::Get2ndDerivative(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nWidth - 1);
	float f1 = m_vecData2ndDerivative[(x*m_nWidth + y) * 2 + 0];
	float f2 = m_vecData2ndDerivative[(x*m_nWidth + y) * 2 + 1];

	return glm::vec2(f1, f2);
}

glm::vec2 CTerrainGenerator::Get2ndDerivativeAccumulated(unsigned int x, unsigned int y)
{
	x = std::fmin(std::fmax(0, x), m_nWidth - 1);
	y = std::fmin(std::fmax(0, y), m_nWidth - 1);
	float f1 = m_vecData2ndDerivativeAccumulated[(x*m_nWidth + y) * 2 + 0];
	float f2 = m_vecData2ndDerivativeAccumulated[(x*m_nWidth + y) * 2 + 1];
	
	return glm::vec2(f1, f2);
}

double fade(double t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double t, double a, double b) {
	return a + t * (b - a);
}

double grad(int hash, double x, double y, double z) {
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
double noise(double x, double y, double z, int perm[]) {

	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	int A = perm[X] + Y;
	int AA = perm[A] + Z;
	int AB = perm[A + 1] + Z;
	int B = perm[X + 1] + Y;
	int BA = perm[B] + Z;
	int BB = perm[B + 1] + Z;

	double res = lerp(w, lerp(v, lerp(u, grad(perm[AA], x, y, z), grad(perm[BA], x - 1, y, z)), lerp(u, grad(perm[AB], x, y - 1, z), grad(perm[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(perm[AA + 1], x, y, z - 1), grad(perm[BA + 1], x - 1, y, z - 1)), lerp(u, grad(perm[AB + 1], x, y - 1, z - 1), grad(perm[BB + 1], x - 1, y - 1, z - 1))));
	return (res + 1.0) / 2.0;
}




void CTerrainGenerator::GenerateNoise()
{
	int perm[512];
	for (int i = 0; i < 256; i++) {
		perm[i] = i;
	}
	//shuffle
	for (int i = 0; i < 256; i++) {
		int swap_idx = rand() % 256;
		std::swap(perm[i], perm[swap_idx]);
	}

	for (int i = 256; i < 512; i++) {
		perm[i] = perm[i - 256];
	}

	m_vecDataSetHeight.resize(m_nWidth*m_nWidth);
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			float height = 2*noise(x / float(m_nWidth) * 10, y / float(m_nWidth) * 10, 0, perm);
			m_vecDataSetHeight[x * m_nWidth + y] = height;
		}
	}
}

void CTerrainGenerator::GenerateDerivatives()
{
	m_vecData1stDerivative.resize(m_nWidth*m_nWidth * 4);
	m_vecData2ndDerivative.resize(m_nWidth*m_nWidth * 2);
	m_vecData2ndDerivativeAccumulated.resize(m_nWidth*m_nWidth * 2);
	float fPixelDistance = 10.0f / m_nWidth * 2.0f;
	//derivative 1st order
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			float fHeightX_1 = GetTerrainHeight(x - 1, y);
			float fHeightX1 = GetTerrainHeight(x + 1, y);
			float fHeightY_1 = GetTerrainHeight(x, y - 1);
			float fHeightY1 = GetTerrainHeight(x, y + 1);

			float fZInXDir = (fHeightX1 - fHeightX_1);
			float fZInYDir = (fHeightY1 - fHeightY_1);
			glm::vec3 vXDir = glm::vec3(fPixelDistance, 0, fZInYDir);
			glm::vec3 vYDir = glm::vec3(0, fPixelDistance, fZInXDir);
			glm::vec3 vNormal = glm::normalize(glm::cross(vXDir, vYDir));

			m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 0] = vNormal.x;
			m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 1] = vNormal.y;
			m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 2] = vNormal.z;
			m_vecData1stDerivative[(x*m_nWidth + y) * 4 + 3] = 0;
		}
	}
	//derivative 2nd order
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			float fHeightX_1 = GetTerrainHeight(x - 1, y);
			float fHeightX0 = GetTerrainHeight(x, y);
			float fHeightX1 = GetTerrainHeight(x + 1, y);
				  
			float fHeightY_1 = GetTerrainHeight(x, y - 1);
			float fHeightY0 = GetTerrainHeight(x, y);
			float fHeightY1 = GetTerrainHeight(x, y + 1);

			float xDir = (fHeightX_1 - 2 * fHeightX0 + fHeightX1);
			float yDir = (fHeightY_1 - 2 * fHeightY0 + fHeightY1);

			m_vecData2ndDerivative[(x*m_nWidth + y) * 2 + 0] = xDir;
			m_vecData2ndDerivative[(x*m_nWidth + y) * 2 + 1] = yDir;
		}
	}
	//accumulate the 2nd derivative
	for (unsigned int x = 0; x < m_nWidth; ++x)
	{
		for (unsigned int y = 0; y < m_nWidth; ++y)
		{
			glm::vec2 derivative2nd = Get2ndDerivative(x, y);
			glm::vec2 derivative2ndX_1Acc = Get2ndDerivativeAccumulated(x - 1, y);
			glm::vec2 derivative2ndY_1Acc = Get2ndDerivativeAccumulated(x, y - 1);

			m_vecData2ndDerivativeAccumulated[(x*m_nWidth + y) * 2 + 0] = derivative2ndX_1Acc.x + abs(derivative2nd.x);
			m_vecData2ndDerivativeAccumulated[(x*m_nWidth + y) * 2 + 1] = derivative2ndY_1Acc.y + abs(derivative2nd.y);
		}
	}
}

unsigned int CTerrainGenerator::GetWidth()
{
	return m_nWidth;
}



