#pragma once
#include <glad\glad.h>
#include "Shader.h"

class CTexture
{
public:
	CTexture(const char* pTexturePath);
	CTexture();
	~CTexture();
	
	void Link(CShader* pShader, unsigned int nBindNumber, const char* pLocationName); //nBindNumber is the number of the texture starting at 0. pLocationName is the variable name of the texture inside the shader

	void Bind(unsigned int nBindNumber); //nBindNumber is the number of the texture starting at 0. pLocationName is the variable name of the texture inside the shader
	GLuint GetTextureID();
	void SetTextureData(unsigned int nWidth, unsigned int nHeight, std::vector<GLubyte>* data);
private:
	void LoadTexture(const char* pTexturePath);
	GLuint m_nID = 0; //texture id

};

