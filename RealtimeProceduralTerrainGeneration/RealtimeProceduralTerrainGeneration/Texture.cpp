#include "Texture.h"
#include "ImageLoader.h"
#include <iostream>
#include <sstream>
#include <string>

enum class EOpenGLError : GLuint
{
	None = 0x0,
	INVALID_ENUM = 0x0500,
	INVALID_VALUE = 0x0501,
	INVALID_OPERATION = 0x0502,
	INVALID_FRAMEBUFFER_OPERATION = 0x0506,
	OUT_OF_MEMORY = 0x0505,
	STACK_UNDERFLOW = 0x0504,
	STACK_OVERFLOW = 0x0503,
};

EOpenGLError GetOpenGLError(bool bEnableBreakpoint)
{
	GLenum errorcode = glGetError();
	if (errorcode)
	{
		EOpenGLError eError = static_cast<EOpenGLError>(errorcode);
		if (bEnableBreakpoint)
		{
			return eError;
		}
		else
		{
			return eError;
		}
	}
	return EOpenGLError::None;
}


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
	if (m_nMaterialColorSpecularID)
	{
		glDeleteTextures(1, &m_nMaterialColorSpecularID);
	}
	if (m_nMaterialNormalHeightID)
	{
		glDeleteTextures(1, &m_nMaterialNormalHeightID);
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

void CTexture::LoadSingleMaterialToTextureArray(const char* pMaterialFolderName, unsigned int nTextureNumber, unsigned int nMaterialNumber)
{
	if (pMaterialFolderName)
	{
		std::stringstream sFileName1;
		sFileName1 << pMaterialFolderName <<"_"<<nTextureNumber<<"_BC_R.tga\0";
		
		int width, height, nrChannels;
		unsigned char *pData = stbi_load(sFileName1.str().c_str(), &width, &height, &nrChannels, 0);
		if (pData)
		{
			if (nrChannels == 4)
			{
				GetOpenGLError(true);
				glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialColorSpecularID);
				GetOpenGLError(true);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, nMaterialNumber, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pData);
				GetOpenGLError(true);
			}		
		}
		else
		{
			int muh = 0;
			muh++;
		}
		stbi_image_free(pData);

		//normal + Heightmap
		std::stringstream sFileName2;
		sFileName2 << pMaterialFolderName << "_" << nTextureNumber << "_N_H.png";
		pData = stbi_load(sFileName2.str().c_str(), &width, &height, &nrChannels, 0);
		if (pData)
		{
			if (nrChannels == 4)
			{
				glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialNormalHeightID);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, nMaterialNumber, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pData);
			}
		}
		else
		{
			int muh = 0;
			muh++;
		}
		stbi_image_free(pData);
	}
}

void CTexture::LoadMaterials()
{
	GetOpenGLError(true);
	if (m_nMaterialColorSpecularID == 0)
	{
		glGenTextures(1, &m_nMaterialColorSpecularID);
		GetOpenGLError(true);
		glGenTextures(1, &m_nMaterialNormalHeightID);
		GetOpenGLError(true);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialColorSpecularID);
		GetOpenGLError(true);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 8, GL_RGBA32F, 1024, 1024, 27);
		GetOpenGLError(true);
		
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialNormalHeightID);
		GetOpenGLError(true);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 8, GL_RGBA32F, 1024, 1024, 27);
		GetOpenGLError(true);
	}
	GetOpenGLError(true);
	int nCounter = 0;
	for (int i = 0; i < 4; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Grass/T_Ground_Grass", i + 1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 4; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Grass_Leaves/T_Ground_Grass_Leaves", i+1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 2; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Litter/T_Ground_Litter", i+1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 1; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Moss/T_Ground_Moss", i + 1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 4; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Mud/T_Ground_Mud", i + 1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 3; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Road_Path/T_Ground_Road_Path", i + 1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 8; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Rock/T_Ground_Rock", i + 1, nCounter);
		nCounter++;
	}
	for (int i = 0; i < 1; ++i)
	{
		LoadSingleMaterialToTextureArray("../textures/T_Ground_Snow/T_Ground_Snow", i + 1, nCounter);
		nCounter++;
	}
	GetOpenGLError(true);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialNormalHeightID);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// set texture filtering parameters
	if (true)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialColorSpecularID);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// set texture filtering parameters
	if (true)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void CTexture::SetTextureData(unsigned int nWidth, unsigned int nHeight, unsigned int nCountChannels, std::vector<GLfloat>* data, bool bCreateMipmaps, bool bLinearInterpolation)
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
	GLenum interpolationMode = GL_LINEAR;
	if (!bLinearInterpolation)
	{
		interpolationMode = GL_NEAREST;
	}
	if (bCreateMipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolationMode);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolationMode);

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

void CTexture::BindMaterial(unsigned int nBindNumberColorSpec, unsigned int nBindNumberNormalHeight)
{
	GetOpenGLError(true);
	glActiveTexture(GL_TEXTURE0 + nBindNumberColorSpec);
	GetOpenGLError(true);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialColorSpecularID);
	GetOpenGLError(true);
	glActiveTexture(GL_TEXTURE0 + nBindNumberNormalHeight);
	GetOpenGLError(true);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_nMaterialNormalHeightID);
	GetOpenGLError(true);

}

void CTexture::LinkMaterial(CShader* pShader, unsigned int nBindNumberColorSpec, unsigned int nBindNumberNormalHeight, const char* pLocationNameColorSpec, const char* pLocationNameNormalHeight)
{
	GetOpenGLError(true);
	GLuint uniformLocation = glGetUniformLocation(pShader->getID(), pLocationNameColorSpec);
	GetOpenGLError(true);
	glUniform1i(uniformLocation, nBindNumberColorSpec);
	GetOpenGLError(true);
	uniformLocation = glGetUniformLocation(pShader->getID(), pLocationNameNormalHeight);
	GetOpenGLError(true);
	glUniform1i(uniformLocation, nBindNumberNormalHeight);
	GetOpenGLError(true);
}

GLuint CTexture::GetTextureID()
{
	return m_nID;
}