#pragma once
#include <glad\glad.h>
#include <glfw3.h>

class CGLFWWindow
{
public:

	CGLFWWindow(unsigned int nWidth, unsigned int nHeight);
	~CGLFWWindow();

	GLFWwindow* getWindowPointer();
	bool IO(); // checks if keys have been pressed


private:
	void checkKey(unsigned int nKey); //helper function for the keyManager
	
	GLFWwindow* m_pWindow = nullptr; //pointer to the GLFW window
	double m_dXPos = 0; //mouse position
	double m_dYPos = 0;
};


