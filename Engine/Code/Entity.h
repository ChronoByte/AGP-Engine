#pragma once

#include "platform.h"

enum EntityType
{
	PATRICK = 0,
	PLANE,
	SPHERE,

	NONE
};
struct Entity
{
	Entity(glm::mat4 worldMatrix, u32 modelIndex, u32 localParamsOffset, u32 localParamsSize, EntityType type)
		: worldMatrix(worldMatrix), modelIndex(modelIndex), localParamsOffset(localParamsOffset), localParamsSize(localParamsSize), type(type) {}

	glm::mat4   worldMatrix;
	u32         modelIndex;
	u32         localParamsOffset;
	u32         localParamsSize;

	EntityType type = EntityType::NONE;
};
