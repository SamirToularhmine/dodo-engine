#pragma once

#include <DodoEngine/Core/Types.h>

#include <glm/glm.hpp>


DODO_BEGIN_NAMESPACE

class Camera
{
public:
	Camera(glm::vec3 _position);

	~Camera() = default;

	void Update(float _deltaTime);

	glm::mat4 GetViewMatrix() const;

private:
	static constexpr double MOUSE_SENSITIVITY = 10.0;

	glm::vec3 m_Position;

	float m_Pitch;
	float m_Yaw;
	float m_Roll;

	float m_Distance;
};

DODO_END_NAMESPACE