#pragma once

#include "platform.h"

enum RenderTargetType
{
	FBO = 0,
	RENDER_TEXTURE,
	DEPTH_TEXTURE,
	ALBEDO_TEXTURE,
	NORMALS_TEXTURE,
	MAX
};

class FrameBufferObject
{

public: 

	FrameBufferObject();

	~FrameBufferObject();

	void Initialize(float _width, float _height);

	void ReserveMemory();
	void FreeMemory();
	
	void UpdateFBO(); 

	void Resize(float _width, float _height);

	void Bind(); 
	void Unbind(); 


private:

	float width = 0.f;
	float height = 0.f; 

	u32 IDs[RenderTargetType::MAX];
};