#pragma once

class cCamera
{
public:
	glm::vec3 position = glm::vec3(0.0f, 28.0f, 10.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	float sensitivity = 20.0f;
	float pitch = 0.0f; // X axis rotation ( up, down)
	float yaw = -90.0f; // Y axis rotation (left, right)
	float speed = 25.0f;
	glm::vec3 direction = glm::normalize(position - forward);
	glm::vec3 right = glm::normalize(glm::cross(up, direction));
};

// mInterpolatedTransform = glm::inverse(finalTransform * initialTransform)