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
	void SetTextureData(unsigned int nWidth, unsigned int nHeight, unsigned int nCountChannels, std::vector<GLfloat>* data, bool bCreateMipmaps, bool bLinearInterpolation = true);
	void LoadTexture(const char* pTexturePath);
	void LoadMaterials();
	void BindMaterial(unsigned int nBindNumberColorSpec, unsigned int nBindNumberNormalHeight);
	void LinkMaterial(CShader* pShader, unsigned int nBindNumberColorSpec, unsigned int nBindNumberNormalHeight, const char* pLocationNameColorSpec, const char* pLocationNameNormalHeight);

private:
	void LoadSingleMaterialToTextureArray(const char* pMaterialFolderName, unsigned int nTextureNumber, unsigned int nMaterialNumber);
	GLuint m_nID = 0; //texture id
	GLuint m_nMaterialColorSpecularID = 0;
	GLuint m_nMaterialNormalHeightID = 0;
};

