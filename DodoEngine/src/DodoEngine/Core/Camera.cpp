#include <DodoEngine/Core/Camera.h>

#include <DodoEngine/Core/InputManager.h>

#include <glm/gtc/matrix_transform.hpp>


DODO_BEGIN_NAMESPACE

Camera::Camera(glm::vec3 _position)
	: m_Position(_position)
	, m_Pitch(0)
	, m_Yaw(0)
	, m_Roll(0)
	, m_Distance(0)
{

}

void Camera::Update(float _deltaTime)
{
    InputManager& inputManager = InputManager::Get();

    const double mouseDeltaScrollY = inputManager.GetScrollDeltaY();
    if(mouseDeltaScrollY != 0)
    {
	m_Distance -= mouseDeltaScrollY * MOUSE_SENSITIVITY * _deltaTime;
    }

    if(inputManager.IsMiddleMouseDown())
    {
	const double mouseDeltaX = inputManager.GetMouseDeltaX();
	if (mouseDeltaX != 0)
	{
		m_Yaw -= mouseDeltaX * MOUSE_SENSITIVITY * _deltaTime;
	}

	const double mouseDeltaY = inputManager.GetMouseDeltaY();
	if (mouseDeltaY != 0)
	{
		m_Pitch -= mouseDeltaY * MOUSE_SENSITIVITY * _deltaTime;
	}
    }

    inputManager.ResetMouseDelta();
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -m_Distance));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Yaw), glm::vec3(0, 1, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Pitch), glm::vec3(1, 0, 0));
	return glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, 0));
}

DODO_END_NAMESPACE
