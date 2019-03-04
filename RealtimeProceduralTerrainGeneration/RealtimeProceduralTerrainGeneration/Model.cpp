#include "Model.h"

CModel::CModel(float nTerrainTextureWidth, float nTerrainGridWidth)
{
	m_nTerrainGridWidth = nTerrainGridWidth;
	m_nTerrainTextureWidth = nTerrainTextureWidth;
	glGenVertexArrays(1, &m_nVAO);
	glBindVertexArray(m_nVAO);
	glGenBuffers(1, &m_nIBO);
	glGenBuffers(1, &m_nVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, NULL); //position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3)); //textureCoordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 5)); //color
	glBindVertexArray(0); //unbind
	for (unsigned int i = 0; i< 1000; ++i)
	{
		m_vecRandomNumbers[i] = rand();
	}
}

CModel::~CModel()
{
	glDeleteBuffers(1, &m_nVBO);
	glDeleteBuffers(1, &m_nIBO);
	glDeleteVertexArrays(1, &m_nVAO);
}

void CModel::EnableTessellation(float fEnable)
{
	m_fEnableTessellation = fEnable;
}

void CModel::EnableNormalMapping(float fEnable)
{
	m_fEnableNormalMapping = fEnable;
}

void CModel::EnableBezierSurface(float fEnable)
{
	m_fEnableBezierSurface = fEnable;
}

void CModel::EnableWireFrame(float fEnable)
{
	if(fEnable)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CModel::SetVBOandIBOData(std::vector<SDataVBO>* pvVBO, std::vector<GLuint>* pvIBO)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SDataVBO) * pvVBO->size(), &pvVBO->front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pvIBO->size(), &pvIBO->front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_nCountIBO = pvIBO->size();
}

void CModel::CreateCube()
{
	SDataVBO sCubeData[8];
	sCubeData[0].SetData(-1, -1, -1, 0, 0, 0, 0, 0);  //vertex 0
	sCubeData[1].SetData(-1, -1, 1, 0, 1, 0, 0, 1);  //vertex 1
	sCubeData[2].SetData(1, -1, 1, 1, 1, 1, 0, 1);  //vertex 2
	sCubeData[3].SetData(1, -1, -1, 1, 0, 1, 0, 0);  //vertex 3
	sCubeData[4].SetData(-1, 1, -1, 0, 0, 0, 1, 0);  //vertex 4
	sCubeData[5].SetData(-1, 1, 1, 0, 1, 0, 1, 1);  //vertex 5
	sCubeData[6].SetData(1, 1, 1, 1, 1, 1, 1, 1);  //vertex 6
	sCubeData[7].SetData(1, 1, -1, 1, 0, 1, 1, 0);  //vertex 7


	//write data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SDataVBO)*8, &sCubeData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint iboData[36];
	//triangle 0
	iboData[0] = 0; 
	iboData[1] = 1;
	iboData[2] = 2;
	
	//triangle 1
	iboData[3] = 0;
	iboData[4] = 2;
	iboData[5] = 3;
	
	//triangle 2
	iboData[6] = 0;
	iboData[7] = 1;
	iboData[8] = 4;
	
	//triangle 3
	iboData[9] = 4;
	iboData[10] = 1;
	iboData[11] = 5;
	
	iboData[12] = 0;
	iboData[13] = 4;
	iboData[14] = 7;

	iboData[15] = 0;
	iboData[16] = 7;
	iboData[17] = 3;

	iboData[18] = 4;
	iboData[19] = 5;
	iboData[20] = 6;

	iboData[21] = 4;
	iboData[22] = 6;
	iboData[23] = 7;

	iboData[24] = 1;
	iboData[25] = 5;
	iboData[26] = 6;

	iboData[27] = 1;
	iboData[28] = 6;
	iboData[29] = 2;

	iboData[30] = 3;
	iboData[31] = 7;
	iboData[32] = 6;

	iboData[33] = 3;
	iboData[34] = 6;
	iboData[35] = 2;

	//write data to IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, iboData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CModel::draw(CShader* pShader, CCamera* pCamera)
{
	pShader->bind();
	glBindVertexArray(m_nVAO); //bind the VAO including VBO, IBO ...
	
	GLint uniformLocationModelMatrix = glGetUniformLocation(pShader->getID(), "modelMatrix");
	GLint uniformLocationViewMatrix = glGetUniformLocation(pShader->getID(), "viewMatrix");
	GLint uniformLocationProjectionMatrix = glGetUniformLocation(pShader->getID(), "projectionMatrix");
	GLint uniformLocationCameraPosition = glGetUniformLocation(pShader->getID(), "cameraPosition");
	GLint uniformLocationTerrainGridWidth = glGetUniformLocation(pShader->getID(), "fGridWidth");
	GLint uniformLocationTerrainTextureWidth = glGetUniformLocation(pShader->getID(), "fTextureWidth");
	GLint uniformLocationEnableTessellation = glGetUniformLocation(pShader->getID(), "fEnableTessellation");
	GLint uniformLocationEnableBezier = glGetUniformLocation(pShader->getID(), "fEnableBezierSurface");
	GLint uniformLocationEnableNormalMapping = glGetUniformLocation(pShader->getID(), "fEnableNormalMapping");
	GLint uniformLocationRandomCount = glGetUniformLocation(pShader->getID(), "randomCount");
	GLint uniformLocationRandomVector = glGetUniformLocation(pShader->getID(), "random");
	GLint uniformLocationMaterialCount = glGetUniformLocation(pShader->getID(), "numberMaterials");

	glm::vec3 cameraPosition = pCamera->GetPosition();
	
	glUniform1i(uniformLocationMaterialCount, 27);
	glUniform1i(uniformLocationRandomCount, 1000);
	glUniform1fv(uniformLocationRandomVector, 1000, m_vecRandomNumbers);
	glUniform1f(uniformLocationEnableTessellation, m_fEnableTessellation);
	glUniform1f(uniformLocationEnableNormalMapping, m_fEnableNormalMapping);
	glUniform1f(uniformLocationEnableBezier, m_fEnableBezierSurface);
	glUniform1f(uniformLocationTerrainGridWidth, m_nTerrainGridWidth - 1);
	glUniform1f(uniformLocationTerrainTextureWidth, m_nTerrainTextureWidth);
	glUniform3f(uniformLocationCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniformMatrix4fv(uniformLocationModelMatrix, 1, GL_FALSE, &m_mModelMatrix[0][0]);
	glUniformMatrix4fv(uniformLocationViewMatrix, 1, GL_FALSE, &(pCamera->GetViewMatrix()[0][0]));
	glUniformMatrix4fv(uniformLocationProjectionMatrix, 1, GL_FALSE, &(pCamera->GetProjectionMatrix()[0][0]));
	glDrawElements(GL_PATCHES, m_nCountIBO, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	pShader->unBind();
}
