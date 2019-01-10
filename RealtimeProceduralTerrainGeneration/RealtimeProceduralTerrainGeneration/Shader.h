#pragma once
#include <vector>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CShader
{
public:
	GLuint getID(); //return the shader id to work with
	~CShader();
	static CShader* createShaderProgram(const char* pVertex_file_path = nullptr,
		const char* pTesselationControl_file_path = nullptr,
		const char* pTesselationEvaluation_file_path = nullptr,
		const char* pGeometry_file_path = nullptr,
		const char* pFragment_file_path = nullptr);
	static std::vector<CShader*> getShaders(); //returns all shaders build in this program

	void bind();
	void unBind();

private:
	
	GLuint ID;
	
	CShader(int id);
};
