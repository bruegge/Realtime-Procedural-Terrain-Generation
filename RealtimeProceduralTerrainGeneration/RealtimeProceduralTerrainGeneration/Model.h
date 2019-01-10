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
	CModel(glm::vec3 position, glm::vec3 rotationAxe); //usual constructor with generating VAO / VBO / IBO
	CModel(CModel* pModel, glm::vec3 position, glm::vec3 rotationAxe); //constructor without generating a new VAO / VBO / IBO
	~CModel();

	void draw(CShader* pShader, CCamera* pCamera); //draw the model with respect to the camera

	void EnableAnimation(); //enables the animation
	void DisableAnimation(); //disables the animation
	void IncreaseAnimationSpeed(); //increases the animation speed
	void DecreaseAnimationSpeed(); //decreases the animation speed

private:

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

	GLuint m_nVAO = 0;
	GLuint m_nVBO = 0;
	GLuint m_nIBO = 0;
	float m_fTime = 0.0f; //the time of the animation
	bool m_bEnableAnimation = false; //the state of the animation (on / off)
	float m_fAnimationSpeed = 0.001f;
	glm::vec3 m_vecPosition; //position of the model
	glm::vec3 m_vecRotationAxe; //axis of rotation

	void CreateCube(); //method to fill the VBO / IBO
};

