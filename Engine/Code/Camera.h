#pragma once

#include "platform.h"
//#include "engine.h"
struct App;

struct Camera
{

	Camera();
	Camera(glm::vec3 pos, glm::vec3 rot);

	glm::vec3 position;
	glm::vec3 target; //deprecated
	glm::vec3 pivot = { 0.0f, 0.0f, 0.0f };
	
	glm::vec3 orientation;
	glm::vec3 camRight;
	glm::vec3 camUp;
	glm::vec3 camForward;

	float yaw;
	float pitch;
	float roll;

	float vertical_fov = 60.0f;
	float aspect_ratio;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	void Update(App* app);
	glm::mat4 CalculateCameraRotation(App* app);

	bool isOrbit = false;
	
};