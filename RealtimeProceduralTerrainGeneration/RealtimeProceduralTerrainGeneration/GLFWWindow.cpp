#include "GLFWWindow.h"
#include <iostream>
#include <unordered_map>
#include "Model.h"
#include "KeyManager.h"

CGLFWWindow::CGLFWWindow(unsigned int nScreenWidth, unsigned int nScreenHeight)
{

	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return;
	}
	glfwWindowHint(GLFW_SAMPLES, 0); // 0x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	m_pWindow = glfwCreateWindow(nScreenWidth, nScreenHeight, "OpenGL Window", NULL, NULL);
	if (m_pWindow == NULL)
	{
		std::cout << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_pWindow); // Initialize GLEW
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);

	std::cout << "Window creation successful" << std::endl;

	GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);
}

CGLFWWindow::~CGLFWWindow()
{
}

GLFWwindow* CGLFWWindow::getWindowPointer()
{
	return m_pWindow;
}

bool CGLFWWindow::IO()
{
	glfwPollEvents();

	for (int i = 0; i < 400; ++i) // check all keys if they're pressed
	{
		checkKey(i);
	}
	
	double xPos, yPos;
	glfwGetCursorPos(m_pWindow, &xPos, &yPos);
	double deltaX = m_dXPos - xPos; //calculate delta X
	double deltaY = m_dYPos - yPos; //calculate delta Y
	m_dXPos = xPos; //save the new mouse position
	m_dYPos = yPos;

	if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (m_dXPos != 0)
		{
			CKeyManager::MouseHasMoved(CKeyManager::EMouseAxis::XAxis, deltaX);
		}
		if (m_dYPos != 0)
		{
			CKeyManager::MouseHasMoved(CKeyManager::EMouseAxis::YAxis, deltaY);
		}
	}
	return glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(m_pWindow); //stop program when closing the window or pressing "escape"
}

void CGLFWWindow::checkKey(unsigned int nKey)
{
	if (glfwGetKey(m_pWindow, nKey) == GLFW_PRESS)
	{
		CKeyManager::KeyHasPressed(nKey);
	}
}
