#pragma once
#include <glad\glad.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "KeyManager.h"
#include "Camera.h"

class CModel : public CKeyManager //inherit the keyManager to let this object act on key strokes
{
public:
	CModel(float nTerrainTextureWidth, float nTerrainGridWidth); //usual constructor with generating VAO / VBO / IBO
	~CModel();

	void draw(CShader* pShader, CCamera* pCamera); //draw the model with respect to the camera
	struct SDataVBO //structure of a single vertex
	{
		void SetData(GLfloat fPositionX, GLfloat fPositionY, GLfloat fPositionZ, GLfloat fTextureU, GLfloat fTextureV, GLfloat fColorR, GLfloat fColorG, GLfloat fColorB)
		{
			this->Position[0] = fPositionX;
			this->Position[1] = fPositionY;
			this->Position[2] = fPositionZ;
			this->TextureCoordinates[0] = fTextureU;
			this->TextureCoordinates[1] = fTextureV;
			this->Color[0] = fColorR;
			this->Color[1] = fColorG;
			this->Color[2] = fColorB;
		}
		GLfloat Position[3];
		GLfloat TextureCoordinates[2];
		GLfloat Color[3];
	};

	void SetVBOandIBOData(std::vector<SDataVBO>* pvVBO, std::vector<GLuint>* pvIBO);
	void CreateCube(); //method to fill the VBO / IBO

private:

	GLfloat m_nTerrainGridWidth;
	GLfloat m_nTerrainTextureWidth;

	GLuint m_nCountIBO = 0;
	GLuint m_nVAO = 0;
	GLuint m_nVBO = 0;
	GLuint m_nIBO = 0;
	glm::mat4 m_mModelMatrix;
};

