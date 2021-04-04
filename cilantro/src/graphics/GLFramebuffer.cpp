#include "cilantroengine.h"
#include "graphics/GLFramebuffer.h"
#include "system/LogMessage.h"

GLFramebuffer::GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight) : Framebuffer (bufferWidth, bufferHeight)
{
}

GLFramebuffer::~GLFramebuffer ()
{
}

void GLFramebuffer::Initialize ()
{
    // create and bind framebuffer
    glGenFramebuffers (1, &framebuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (1, &framebuffers.textureBuffer);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, framebuffers.textureBuffer);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture (GL_TEXTURE_2D, 0);
    
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers.textureBuffer, 0);

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &framebuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, framebuffers.RBO);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffers.RBO);

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete";
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized framebuffer" << bufferWidth << bufferHeight;
    }
}

void GLFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &framebuffers.RBO);
    glDeleteTextures (1, &framebuffers.textureBuffer);
    glDeleteFramebuffers (1, &framebuffers.FBO);
}

void GLFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffers.FBO);
}

void GLFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    glDeleteRenderbuffers (1, &framebuffers.RBO);
    glDeleteTextures (1, &framebuffers.textureBuffer);
    glDeleteFramebuffers (1, &framebuffers.FBO);

    Framebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);

    Initialize ();
}

GLuint GLFramebuffer::GetFramebufferTextureGLId () const
{
    return framebuffers.textureBuffer;
}

GLuint GLFramebuffer::GetFramebufferGLId () const
{
    return framebuffers.FBO;
}