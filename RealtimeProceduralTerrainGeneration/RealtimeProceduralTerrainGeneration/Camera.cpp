#include "Camera.h"
#include <glm\gtx\transform.hpp>
#include <math.h>
#include <algorithm>

CCamera::CCamera(float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane, glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector)
{
	this->SetOrientation(EyePosition, LookAtVector, UpVector);
	this->SetPerspective(fFieldOfView, fAspectRatio, fNearClippingplane, fFarClippingPlane);
}

void CCamera::SetPerspective(float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane)
{
	m_ProjectionMatrix = glm::perspective(fFieldOfView * 3.1415926f / 180.0f, fAspectRatio, fNearClippingplane, fFarClippingPlane);
}

void CCamera::SetOrientation(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector)
{
	m_ViewMatrix = glm::lookAt(EyePosition, LookAtVector, UpVector);
}

CCamera::~CCamera()
{
}

glm::mat4 CCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

glm::mat4 CCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

glm::mat4 CCamera::GetViewProjectionMatrix() const
{
	return m_ProjectionMatrix * m_ViewMatrix;
}

void CCamera::Rotate(glm::vec3 RotationAxe, float fAngle)
{
	m_ViewMatrix = m_ViewMatrix * glm::rotate(fAngle * 3.1415926f / 180.0f, RotationAxe);
}

void CCamera::Translate(glm::vec3 Translation)
{
	m_ViewMatrix = glm::translate(Translation)* m_ViewMatrix;
}

glm::vec3 CCamera::GetPosition() const
{
	glm::mat4 modelViewT = transpose(m_ViewMatrix);

	// Get plane normals 
	glm::vec3 n1(modelViewT[0]);
	glm::vec3 n2(modelViewT[1]);
	glm::vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	glm::vec3 n2n3 = glm::cross(n2, n3);
	glm::vec3 n3n1 = glm::cross(n3, n1);
	glm::vec3 n1n2 = glm::cross(n1, n2);

	glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}

void CCamera::TurnX(double dDeltaX)
{
	/*glm::mat4 transposed = glm::transpose(m_ViewMatrix);
	glm::vec3 left = transposed[0];
	glm::vec3 up = transposed[1];
	glm::vec3 front = transposed[2];
	glm::vec3 position = GetPosition();
	float fDistanceToCenter = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	position += left * static_cast<float>(dDeltaX) * fDistanceToCenter * 0.01f;
	position = glm::normalize(position)* fDistanceToCenter;
	m_ViewMatrix = glm::lookAt(position, glm::vec3(0,0,0), up);*/
	glm::mat4 rotationMatrix = glm::rotate(static_cast<float>(-dDeltaX/180.0f), glm::vec3(0, 0, 1));
	m_ViewMatrix = m_ViewMatrix * rotationMatrix;
}

void CCamera::TurnY(double dDeltaY)
{
	glm::mat4 transposed = glm::transpose(m_ViewMatrix);
	glm::vec3 left = transposed[0];
	glm::vec3 up = transposed[1];
	glm::vec3 front = transposed[2];
	glm::vec3 position = GetPosition();
	float fDistanceToCenter = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	position -= up * static_cast<float>(dDeltaY) * fDistanceToCenter * 0.01f;
	position = glm::normalize(position)* fDistanceToCenter;
	if (position.z > 0.0f && (up.z > 0.0f || dDeltaY > 0))
	{
		m_ViewMatrix = glm::lookAt(position, glm::vec3(0, 0, 0), up);
	}
}

void CCamera::Forward(double dDeltaTime)
{
	glm::vec3 position = GetPosition();
	float fDistanceToCenter = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	if (fDistanceToCenter > 1.0f)
	{
		Translate(glm::vec3(0, 0, 4.0f * dDeltaTime));
	}
}

void CCamera::Backward(double dDeltaTime)
{
	glm::vec3 position = GetPosition();
	float fDistanceToCenter = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	if (fDistanceToCenter < 10.0f)
	{
		Translate(glm::vec3(0, 0, -4.0f * dDeltaTime));
	}
}