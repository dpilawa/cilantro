#include "cilantroengine.h"
#include "LogMessage.h"
#include "RenderTarget.h"
#include "FrameBufferRenderTarget.h"

FrameBufferRenderTarget::FrameBufferRenderTarget (int xRes, int yRes) : RenderTarget (xRes, yRes)
{
}


FrameBufferRenderTarget::~FrameBufferRenderTarget ()
{
}

void FrameBufferRenderTarget::Initialize ()
{
	// Note: FrameBufferRenderTarget requires GL context already created and extensions (e.g. GLEW) loaded.
	// It only prepares a target for off-screen rendering.

	// create and bind framebuffer
	glGenFramebuffers (1, &FBO);
	glBindFramebuffer (GL_FRAMEBUFFER, FBO);

	// create texture and attach to framebuffer as color attachment
	glGenTextures (1, &textureColorBuffer);
	glBindTexture (GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, xResolution, yResolution, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture (GL_TEXTURE_2D, 0);

	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	// create renderbuffer for a (combined) depth and stencil buffer
	glGenRenderbuffers (1, &RBO);
	glBindRenderbuffer (GL_RENDERBUFFER, RBO);
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, xResolution, yResolution);
	glBindRenderbuffer (GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// check status
	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LogMessage (__func__, EXIT_FAILURE) << "Framebuffer is not complete";
	}
	else
	{
		LogMessage (__func__) << "FrameBufferRenderTarget started";
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void FrameBufferRenderTarget::Deinitialize ()
{
	glDeleteRenderbuffers (1, &RBO);
	glDeleteTextures (1, &textureColorBuffer);
	glDeleteFramebuffers (1, &FBO);
}

void FrameBufferRenderTarget::BeforeFrame ()
{
	// bind framebuffer
	glBindFramebuffer (GL_FRAMEBUFFER, FBO);
}

void FrameBufferRenderTarget::AfterFrame ()
{
	// bind default framebuffer
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}
