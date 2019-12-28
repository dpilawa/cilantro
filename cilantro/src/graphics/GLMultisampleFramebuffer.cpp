#include "cilantroengine.h"
#include "graphics/GLMultisampleFramebuffer.h"
#include "util/LogMessage.h"

GLMultisampleFramebuffer::GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight) : GLFramebuffer (bufferWidth, bufferHeight)
{
    this->Initialize ();
}

GLMultisampleFramebuffer::~GLMultisampleFramebuffer ()
{
    this->Deinitialize ();
}

void GLMultisampleFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    GLFramebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);

    // resize framebuffer texture and viewport
	glDeleteRenderbuffers (1, &multisampleFramebuffers.RBO);
	glDeleteTextures (1, &multisampleFramebuffers.textureBuffer);
	glDeleteFramebuffers (1, &multisampleFramebuffers.FBO);

	Initialize ();
}

GLuint GLMultisampleFramebuffer::GetMultisampleFramebufferTexture () const
{
    return multisampleFramebuffers.textureBuffer;
}

GLuint GLMultisampleFramebuffer::GetMultisampleFramebuffer () const
{
    return multisampleFramebuffers.FBO;
}

void GLMultisampleFramebuffer::Initialize ()
{
	// create and bind framebuffer
	glGenFramebuffers (1, &multisampleFramebuffers.FBO);
	glBindFramebuffer (GL_FRAMEBUFFER, multisampleFramebuffers.FBO);

	// create texture and attach to framebuffer as color attachment
	// on GL versions <= 3.1 use normal texture instead of multisampled
	glGenTextures (1, &multisampleFramebuffers.textureBuffer);
#if (CILANTRO_MIN_GL_VERSION <= 310)
	glBindTexture (GL_TEXTURE_2D, framebuffers.textureBuffer);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture (GL_TEXTURE_2D, 0);

	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers.textureBuffer, NULL);
#elif
	glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer);
	glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, bufferWidth, bufferHeight, GL_TRUE);
	glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer, 0);
#endif

	// create renderbuffer for a (combined) depth and stencil buffer
	glGenRenderbuffers (1, &multisampleFramebuffers.RBO);
	glBindRenderbuffer (GL_RENDERBUFFER, multisampleFramebuffers.RBO);
#if (CILANTRO_MIN_GL_VERSION <= 310)
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
#elif
	glRenderbufferStorageMultisample (GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
#endif
	glBindRenderbuffer (GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleFramebuffers.RBO);

	// check status
	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LogMessage (__func__, EXIT_FAILURE) << "Multisample framebuffer is not complete";
	}
}

void GLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &multisampleFramebuffers.RBO);
	glDeleteTextures (1, &multisampleFramebuffers.textureBuffer);
	glDeleteFramebuffers (1, &multisampleFramebuffers.FBO);
}