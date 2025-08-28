#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float Zoom;
	float MovementSpeed;
	float Sensitivity;

	Camera()
	{
		Position = glm::vec3(0.0f, 1.f, 3.0f);
		Front = glm::vec3(0.0f, 0.0f, -1.0f);
		Up = glm::vec3(0.0f, 1.0f, 0.0f);
		Right = glm::normalize(glm::cross(Front, Up));
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Yaw = -90.0f;
		Pitch = 0.0f;

		Zoom = 45.0f;
		//MovementSpeed = 0.003f;
		MovementSpeed = 0.005f;
		Sensitivity = 0.1f;

		updateCameraVector();
	}

	Camera(
		glm::vec3 position,
		glm::vec3 front,
		glm::vec3 up,
		glm::vec3 right,
		glm::vec3 worldUp,

		float yaw,
		float pitch,

		float zoom,
		float movementSpeed,
		float sensitivity
	)
	{
		Position = position;
		Front = front;
		Up = up;
		Right = right;
		WorldUp = worldUp;

		Yaw = yaw;
		Pitch = pitch;

		Zoom = zoom;
		MovementSpeed = movementSpeed;
		Sensitivity = sensitivity;

		updateCameraVector();
	}

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 getBackwardViewMatrix() const
	{
		return glm::lookAt(Position, Position - Front, Up);
	}

	void processKeyboard(Camera_Movement movement, float deltaTime)
	{
		glm::vec3 planeFront = glm::normalize(glm::vec3(Front.x, 0, Front.z));
		switch (movement) {
		case (FORWARD):
			Position += MovementSpeed * planeFront * deltaTime;
			break;
		case (BACKWARD):
			Position -= MovementSpeed * planeFront * deltaTime;
			break;
		case (LEFT):
			Position -= glm::normalize(glm::cross(planeFront, Up)) * MovementSpeed * deltaTime;
			break;
		case (RIGHT):
			Position += glm::normalize(glm::cross(planeFront, Up)) * MovementSpeed * deltaTime;
			break;
		case (UP):
			Position += WorldUp * MovementSpeed * deltaTime;
			break;
		case (DOWN):
			Position -= WorldUp * MovementSpeed * deltaTime;
			break;
		}
	}

	void processMouse(float xoffset, float yoffset, bool pitchConstraint = true)
	{
		xoffset *= Sensitivity;
		yoffset *= Sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (pitchConstraint) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVector();
	}

	void processMouseScroll(float yoffset)
	{
		Zoom -= yoffset;
		if (Zoom < 1)
			Zoom = 1;
		if (Zoom > 45)
			Zoom = 45;
	}

private:
	void updateCameraVector()
	{
		// calculate the new Front vector
		glm::vec3 front{};
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};