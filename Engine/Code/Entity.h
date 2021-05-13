#pragma once

#include "platform.h"

struct Entity
{
	Entity(glm::mat4 worldMatrix, u32 modelIndex, u32 localParamsOffset, u32 localParamsSize)
		: worldMatrix(worldMatrix), modelIndex(modelIndex), localParamsOffset(localParamsOffset), localParamsSize(localParamsSize) {}

	glm::mat4   worldMatrix;
	u32         modelIndex;
	u32         localParamsOffset;
	u32         localParamsSize;
};
