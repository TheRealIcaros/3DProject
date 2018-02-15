#ifndef CAMERA_H
#define CAMERA_H

using namespace std;

#include "Defines.h"

//3D-math
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


class Camera
{
private:
	glm::mat4 View;

	glm::vec3 cameraPosition;
	glm::vec3 lookAtVector;
	glm::vec3 upVector;

	float yaw;
	float pitch;
	float sensitivity;

	float speed;
public:
	Camera();
	Camera(glm::vec3 cameraPosition, glm::vec3 lookAtVector);
	~Camera();
	void setLookAtVector(glm::vec3 lookAtVector);
	void moveCameraPosition(glm::vec3 movement);
	void mouseMovement(float xoffset, float yoffset);

	// - GET/SET
	void setYaw(float yaw);
	void setPitch(float pitch);
	float getYaw()const;
	float getPitch()const;
	glm::mat4 getView()const;
	float getSpeed()const;
	glm::vec3 getLookAtVector()const;
	glm::vec3 getUpVector()const;
	glm::vec3 getPosition()const;
	void setPosition(glm::vec3 position);
	void setHeight(float Y);
};


#endif // CAMERA