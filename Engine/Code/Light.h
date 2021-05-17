#pragma once

#include "platform.h"

enum class LightType
{
	LIGHT_TYPE_POINT = 0,
	LIGHT_TYPE_DIRECTIONAL
};

struct Light
{
	LightType type; 
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;

	Light(){}
	Light(glm::vec3 position, glm::vec3 color, LightType type = LightType::LIGHT_TYPE_POINT, glm::vec3 direction = glm::vec3(0.0f, -1.0f, -1.0f))
		: position(position),
		color(color),
		type(type),
		direction(direction)
	{}

	glm::mat4 CalculateLightRotation()
	{
		glm::mat4 matrix;

		glm::vec3 lightForward = glm::normalize(this->direction);
		glm::vec3 lightRight = glm::normalize(glm::cross(glm::vec3(0, 1, 0), lightForward));
		glm::vec3 lightUp = glm::cross(lightForward, lightRight);

		matrix = glm::lookAt(this->position, this->position + lightForward, lightUp);

		return matrix;
	}
};