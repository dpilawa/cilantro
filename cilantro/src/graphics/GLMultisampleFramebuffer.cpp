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

void GLMultisampleFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, multisampleFramebuffers.FBO);
}

void GLMultisampleFramebuffer::BlitFramebuffer ()
{
    // blit multisample framebuffer to standard framebuffer
    glBindFramebuffer (GL_READ_FRAMEBUFFER, multisampleFramebuffers.FBO);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, framebuffers.FBO);
    glBlitFramebuffer (0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); 
    glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
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
    glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer);
    glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, bufferWidth, bufferHeight, GL_TRUE);
    glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer, 0);

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &multisampleFramebuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, multisampleFramebuffers.RBO);
    glRenderbufferStorageMultisample (GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleFramebuffers.RBO);

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (__func__, EXIT_FAILURE) << "Multisample framebuffer is not complete";
    }
    else
    {
        LogMessage (__func__) << "Initialized multisample framebuffer";
    }
    
}

void GLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &multisampleFramebuffers.RBO);
    glDeleteTextures (1, &multisampleFramebuffers.textureBuffer);
    glDeleteFramebuffers (1, &multisampleFramebuffers.FBO);
}