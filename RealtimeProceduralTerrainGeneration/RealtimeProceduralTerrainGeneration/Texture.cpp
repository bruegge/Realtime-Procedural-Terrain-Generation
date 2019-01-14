#include "Texture.h"
#include "ImageLoader.h"
#include <iostream>

CTexture::CTexture(const char* pTexturePath)
{
	LoadTexture(pTexturePath);
}

CTexture::CTexture()
{
}

CTexture::~CTexture()
{
	if (m_nID)
	{
		glDeleteTextures(1, &m_nID);
	}
}

void CTexture::LoadTexture(const char* pTexturePath)
{
	if (m_nID)
	{
		glDeleteTextures(1, &m_nID);
	}
	glGenTextures(1, &m_nID);
	glBindTexture(GL_TEXTURE_2D, m_nID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glEnable(GL_TEXTURE_2D);
	// set the texture wrapping parameters
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char *data = stbi_load(pTexturePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
}

void CTexture::SetTextureData(unsigned int nWidth, unsigned int nHeight, std::vector<GLfloat>* data)
{
	if (m_nID)
	{
		glDeleteTextures(1, &m_nID);
	}
	glGenTextures(1, &m_nID);
	glBindTexture(GL_TEXTURE_2D, m_nID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glEnable(GL_TEXTURE_2D);
	// set the texture wrapping parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, nWidth, nHeight, 0, GL_RED, GL_FLOAT, &data->front());
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
}

void CTexture::Bind(unsigned int nBindNumber)
{
	glActiveTexture(GL_TEXTURE0 + nBindNumber);
	glBindTexture(GL_TEXTURE_2D, m_nID);
}

void CTexture::Link(CShader* pShader, unsigned int nBindNumber, const char* pLocationName) //nBindNumber is the number of the texture starting at 0. pLocationName is the variable name of the texture inside the shader
{
	GLuint uniformLocation = glGetUniformLocation(pShader->getID(), pLocationName);
	glUniform1i(uniformLocation, nBindNumber);
}

GLuint CTexture::GetTextureID()
{
	return m_nID;
}