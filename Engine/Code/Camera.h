#pragma once

#include "platform.h"
//#include "engine.h"
struct App;

struct Camera
{

	Camera();
	Camera(glm::vec3 pos, glm::vec3 target);

	glm::vec3 position = { 0, 0, -10 };
	glm::vec3 target = { 0, 0, 0 };

	glm::vec3 rotationEuler = { 0, 0, 0 };
	float vertical_fov = 60.0f;
	float aspect_ratio;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	void Update(App* app);
	
};