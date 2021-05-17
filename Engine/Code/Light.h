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
	unsigned int intensity; // From 0 to 100

	Light(){}
	Light(glm::vec3 position, glm::vec3 color, LightType type = LightType::LIGHT_TYPE_POINT, glm::vec3 direction = glm::vec3(0.0f, -1.0f, -1.0f), unsigned int intensity = 100U)
		: position(position),
		color(color),
		type(type),
		direction(direction),
		intensity(intensity)
	{}
};