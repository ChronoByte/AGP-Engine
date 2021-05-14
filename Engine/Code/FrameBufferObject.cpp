#include "FrameBufferObject.h"
#include "engine.h"

FrameBufferObject::FrameBufferObject()
{
}

FrameBufferObject::~FrameBufferObject()
{
	FreeMemory();
}

void FrameBufferObject::Initialize(float _width, float _height)
{
	for (u32 i = 0; i < RenderTargetType::MAX; ++i)
	{
		IDs[i] = 0u;
	}

	width = _width; 
	height = _height;

	ReserveMemory(); 
	UpdateFBO();
}

void FrameBufferObject::ReserveMemory()
{
	// Here we reserve all the memory, to be later on easy to delete 

	glGenTextures(1, &IDs[RENDER_TEXTURE]);
	glGenTextures(1, &IDs[DEPTH_TEXTURE]);

	glGenFramebuffers(1, &IDs[FBO]);

}

void FrameBufferObject::FreeMemory()
{
	glDeleteTextures(1, &IDs[RENDER_TEXTURE]);
	glDeleteTextures(1, &IDs[DEPTH_TEXTURE]);

	glDeleteFramebuffers(1, &IDs[FBO]);
}

void FrameBufferObject::UpdateFBO()
{
	// ------------------------ Define Render Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[RENDER_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------


	// ------------------------ Define Depth Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[DEPTH_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------


	// ------------------------ Define FrameBuffer Object------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, IDs[FBO]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, IDs[RENDER_TEXTURE], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, IDs[DEPTH_TEXTURE], 0);

	GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (frameBufferStatus)
		{
			case GL_FRAMEBUFFER_UNDEFINED: ELOG("GL_FRAMEBUFFER_UNDEFINED"); break; 
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break; 
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break; 
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
			case GL_FRAMEBUFFER_UNSUPPORTED: ELOG("GL_FRAMEBUFFER_UNSUPPORTED"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: ELOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
			default: ELOG("Unkown Framebuffer Status Error :)");
		}
	}

	glDrawBuffers(1, &IDs[RENDER_TEXTURE]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::Resize(float _width, float _height)
{
	width = _width;
	height = _height;

	UpdateFBO();
}

void FrameBufferObject::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, IDs[FBO]);

	GLuint drawBuffers[] = { IDs[RENDER_TEXTURE] };
	glDrawBuffers(ARRAY_COUNT(drawBuffers), drawBuffers);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FrameBufferObject::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

u32 FrameBufferObject::GetTexture(RenderTargetType textureType)
{
	return IDs[textureType];
}
