#include "Camera.h"

Camera::Camera()
{
	this->cameraPosition = { 0.0f, 0.0f, 0.0f };
	this->lookAtVector = { 0.0f, 0.0f, -1.0f };
	this->upVector = { 0.0f, 1.0f, 0.0f };
	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->sensitivity = 0.05f;
	this->speed = 5.0f;

	this->View = glm::lookAt(this->cameraPosition, this->cameraPosition + this->lookAtVector, this->upVector);
}

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 lookAtVector)
{
	this->cameraPosition = cameraPosition;
	this->lookAtVector = lookAtVector;
	this->upVector = { 0.0f, 1.0f, 0.0f };
	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->sensitivity = 0.05f;
	this->speed = 5.0f;

	this->View = glm::lookAt(this->cameraPosition, this->cameraPosition + this->lookAtVector, this->upVector);
}

Camera::~Camera()
{
}

void Camera::setLookAtVector(glm::vec3 lookAtVector)
{
	this->lookAtVector = lookAtVector;
	this->View = glm::lookAt(this->cameraPosition, this->cameraPosition + this->lookAtVector, this->upVector);
}

void Camera::moveCameraPosition(glm::vec3 movement)
{
	this->cameraPosition += movement;
	this->View = glm::lookAt(this->cameraPosition, this->cameraPosition + this->lookAtVector, this->upVector);
}

void Camera::mouseMovement(float xoffset, float yoffset)
{
	xoffset *= this->sensitivity;
	yoffset *= this->sensitivity;

	this->yaw += xoffset;
	this->pitch += yoffset;

	//Check if within bounderys
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->setLookAtVector(glm::normalize(front));
}

// - GET/SET


void Camera::setYaw(float yaw)
{
	this->yaw = yaw;
}

float Camera::getYaw()const
{
	return this->yaw;
}

void Camera::setPitch(float pitch)
{
	this->pitch = pitch;
}

float Camera::getPitch()const
{
	return this->pitch;
}

glm::mat4 Camera::getView()const
{
	return this->View;
}

float Camera::getSpeed()const
{
	return this->speed;
}

glm::vec3 Camera::getLookAtVector()const
{
	return this->lookAtVector;
}

glm::vec3 Camera::getUpVector()const
{
	return this->upVector;
}