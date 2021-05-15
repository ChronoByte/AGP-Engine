#pragma once

#include "platform.h"

enum RenderTargetType
{
	RENDER_TEXTURE = 0,
	G_POSITION_TEXTURE,
	G_NORMALS_TEXTURE,
	G_ALBEDO_TEXTURE,
	DEPTH_TEXTURE,
	FBO,
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


	u32 GetTexture(RenderTargetType textureType);

private:

	float width = 0.f;
	float height = 0.f; 

	u32 IDs[RenderTargetType::MAX];
};