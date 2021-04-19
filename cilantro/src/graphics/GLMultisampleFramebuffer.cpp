#include "cilantroengine.h"
#include "graphics/GLMultisampleFramebuffer.h"
#include "system/LogMessage.h"

GLMultisampleFramebuffer::GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount) 
: GLFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount)
{
}

GLMultisampleFramebuffer::~GLMultisampleFramebuffer ()
{
}

void GLMultisampleFramebuffer::Initialize ()
{
    GLFramebuffer::Initialize ();

    // create and bind framebuffer
    glGenFramebuffers (1, &multisampleFramebuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, multisampleFramebuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (rgbTextureCount + rgbaTextureCount, multisampleFramebuffers.textureBuffer);
    for (unsigned int i = 0; i < rgbTextureCount + rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer[i]);
        glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, 4, (i < rgbTextureCount) ? GL_RGB : GL_RGBA, bufferWidth, bufferHeight, GL_TRUE);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, multisampleFramebuffers.textureBuffer[i], 0);
    }

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &multisampleFramebuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, multisampleFramebuffers.RBO);
    glRenderbufferStorageMultisample (GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleFramebuffers.RBO);

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Multisample framebuffer is not complete";
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized multisample framebuffer" << bufferWidth << bufferHeight;
    }
    
}

void GLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &multisampleFramebuffers.RBO);
    glDeleteTextures (rgbTextureCount + rgbaTextureCount, multisampleFramebuffers.textureBuffer);
    glDeleteFramebuffers (1, &multisampleFramebuffers.FBO);

    GLFramebuffer::Deinitialize ();
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

GLuint GLMultisampleFramebuffer::GetMultisampleFramebufferRenderbufferGLId () const
{
    return multisampleFramebuffers.RBO;
}

GLuint GLMultisampleFramebuffer::GetMultisampleFramebufferTextureGLId (unsigned int textureNumber) const
{
    return multisampleFramebuffers.textureBuffer[textureNumber];
}

GLuint GLMultisampleFramebuffer::GetMultisampleFramebufferGLId () const
{
    return multisampleFramebuffers.FBO;
}

