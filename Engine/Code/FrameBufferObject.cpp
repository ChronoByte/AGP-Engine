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

void FrameBufferObject::Resize(float _width, float _height)
{
	width = _width;
	height = _height;

	UpdateFBO();
}

void FrameBufferObject::Bind(bool aClear)
{
	glBindFramebuffer(GL_FRAMEBUFFER, IDs[FBO]);
	if (aClear)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
}

void FrameBufferObject::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

u32 FrameBufferObject::GetTexture(RenderTargetType textureType)
{
	return IDs[textureType];
}

void GBuffer::ReserveMemory()
{
	// Here we reserve all the memory, to be later on easy to delete 

	glGenTextures(1, &IDs[RENDER_TEXTURE]);
	glGenTextures(1, &IDs[DEPTH_TEXTURE]);
	glGenTextures(1, &IDs[G_POSITION_TEXTURE]);
	glGenTextures(1, &IDs[G_ALBEDO_TEXTURE]);
	glGenTextures(1, &IDs[G_NORMALS_TEXTURE]);

	glGenFramebuffers(1, &IDs[FBO]);
	glGenRenderbuffers(1, &IDs[ZBO]);
}

void GBuffer::FreeMemory()
{
	glDeleteTextures(1, &IDs[RENDER_TEXTURE]);
	glDeleteTextures(1, &IDs[DEPTH_TEXTURE]);
	glDeleteTextures(1, &IDs[G_POSITION_TEXTURE]);
	glDeleteTextures(1, &IDs[G_ALBEDO_TEXTURE]);
	glDeleteTextures(1, &IDs[G_NORMALS_TEXTURE]);

	glDeleteFramebuffers(1, &IDs[FBO]);
	glDeleteRenderbuffers(1, &IDs[ZBO]);
}

void GBuffer::UpdateFBO()
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------


	// ------------------------ Define Position Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[G_POSITION_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------

	// ------------------------ Define Normal Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[G_NORMALS_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	// ----------------------------------------------------------------------

	// ------------------------ Define Albedo Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[G_ALBEDO_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------



	// ------------------------ Define FrameBuffer Object------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, IDs[FBO]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, IDs[RENDER_TEXTURE], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, IDs[G_POSITION_TEXTURE], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, IDs[G_NORMALS_TEXTURE], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, IDs[G_ALBEDO_TEXTURE], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, IDs[DEPTH_TEXTURE], 0);

	GLuint drawBuffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, drawBuffers);

	// ------------------------ Define ZBuffer Object------------------------
	glBindRenderbuffer(GL_RENDERBUFFER, IDs[ZBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, IDs[ZBO]);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

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

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void ShadingBuffer::ReserveMemory()
{
	glGenTextures(1, &IDs[RENDER_TEXTURE]);
	glGenTextures(1, &IDs[DEPTH_TEXTURE]);
	glGenTextures(1, &IDs[BRIGHT_COLOR_TEXTURE]);

	glGenFramebuffers(1, &IDs[FBO]);
	

}

void ShadingBuffer::FreeMemory()
{
	glDeleteTextures(1, &IDs[RENDER_TEXTURE]);
	glDeleteTextures(1, &IDs[DEPTH_TEXTURE]);
	glDeleteTextures(1, &IDs[BRIGHT_COLOR_TEXTURE]);

	glDeleteFramebuffers(1, &IDs[FBO]);
	
}

void ShadingBuffer::UpdateFBO()
{
	// ------------------------ Define Render Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[RENDER_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------------------------------------------------------------------

	// ------------------------ Define Render Texture ------------------------

	glBindTexture(GL_TEXTURE_2D, IDs[BRIGHT_COLOR_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, IDs[BRIGHT_COLOR_TEXTURE], 0);
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

	GLuint drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PingPongBuffer::ReserveMemory()
{
	glGenFramebuffers(2, pingPongFBO);
	glGenTextures(2, pingPongTextures);
}

void PingPongBuffer::FreeMemory()
{
	glDeleteTextures(2, pingPongTextures);
	glDeleteFramebuffers(2, pingPongFBO);
}

void PingPongBuffer::UpdateFBO()
{
	for (u32 i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongTextures[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextures[i], 0
		);
	}


}

void PingPongBuffer::BlurImage(int iterations, u32 colorTexture, Program shaderBlur, void (*f)())
{
	bool horizontal = true, first_iteration = true;
	glUseProgram(shaderBlur.handle);
	for (unsigned int i = 0; i < iterations; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
		glUniform1f(glGetUniformLocation(shaderBlur.handle, "horizontal"), horizontal);		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorTexture : pingPongTextures[!horizontal]);
		(*f)(); // RENDER QUAD FUNCTION 
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

u32 PingPongBuffer::GetBlurredTexture()
{
	return pingPongTextures[0];
}
