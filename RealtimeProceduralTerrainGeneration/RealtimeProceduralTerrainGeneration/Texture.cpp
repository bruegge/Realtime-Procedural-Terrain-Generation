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
		if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
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

void CTexture::SetTextureData(unsigned int nWidth, unsigned int nHeight, unsigned int nCountChannels, std::vector<GLfloat>* data, bool bCreateMipmaps)
{
	if (m_nID)
	{
		glDeleteTextures(1, &m_nID);
	}
	glGenTextures(1, &m_nID);
	glBindTexture(GL_TEXTURE_2D, m_nID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glEnable(GL_TEXTURE_2D);
	// set the texture wrapping parameters
	GLenum format = GL_RED;
	GLenum format2 = GL_R32F;
	switch (nCountChannels)
	{
	case 1:
		format = GL_RED;
		format2 = GL_R32F;
		break;
	case 2:
		format = GL_RG;
		format2 = GL_RG32F;
		break;
	case 3:
		format = GL_RGB;
		format2 = GL_RGB32F;
		break;
	case 4:
		format = GL_RGBA;
		format2 = GL_RGBA32F;
		break;
	default:
		break;
	}
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format2, nWidth, nHeight, 0, format, GL_FLOAT, &data->front());
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format2, nWidth, nHeight, 0, format, GL_FLOAT, NULL);
	}
	if (bCreateMipmaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	if (bCreateMipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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