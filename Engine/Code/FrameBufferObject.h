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
	ZBO,
	MAX
};

class FrameBufferObject
{

public: 

	FrameBufferObject();

	~FrameBufferObject();

	void Initialize(float _width, float _height);

	virtual void ReserveMemory() = 0;
	virtual void FreeMemory() {}
	
	virtual void UpdateFBO() = 0;

	void Resize(float _width, float _height);

	void Bind(); 
	void Unbind(); 

	u32 GetTexture(RenderTargetType textureType);

protected:

	float width = 0.f;
	float height = 0.f; 

	u32 IDs[RenderTargetType::MAX];
};

class GBuffer : public FrameBufferObject
{
public: 

	void ReserveMemory() override; 
	void FreeMemory() override;

	void UpdateFBO() override;
};

class ShadingBuffer : public FrameBufferObject
{
public: 
	void ReserveMemory() override; 
	void FreeMemory() override; 

	void UpdateFBO() override;
};