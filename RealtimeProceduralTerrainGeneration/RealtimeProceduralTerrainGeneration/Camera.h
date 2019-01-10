#pragma once
#include <glm/glm.hpp>
#include "KeyManager.h"

class CCamera : public CKeyManager // camera should act on key input
{
public:
	CCamera(float fFieldOfView, float fNearClippingplane, float fFarClippingPlane, float fAspectRatio, glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector);
	~CCamera();

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;

	void SetOrientation(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector);
	void SetPerspective(float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane);
	void Rotate(glm::vec3 RotationAxe, float fAngle);
	void Translate(glm::vec3 Translation);
	void TurnX(double deltaX);
	void TurnY(double deltaY);
	void Forward();
	void Backward();
	glm::vec3 GetPosition() const;

private:

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
};

