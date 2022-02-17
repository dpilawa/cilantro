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
    glGenFramebuffers (1, &glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, glMultisampleBuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (rgbTextureCount + rgbaTextureCount, glMultisampleBuffers.textureBuffer);
    for (unsigned int i = 0; i < rgbTextureCount + rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, glMultisampleBuffers.textureBuffer[i]);
        glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, 4, (i < rgbTextureCount) ? GL_RGB16F : GL_RGBA16F, bufferWidth, bufferHeight, GL_TRUE);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, glMultisampleBuffers.textureBuffer[i], 0);
    }

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &glMultisampleBuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, glMultisampleBuffers.RBO);
    glRenderbufferStorageMultisample (GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);
  
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glMultisampleBuffers.RBO);

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
    glDeleteRenderbuffers (1, &glMultisampleBuffers.RBO);
    glDeleteTextures (rgbTextureCount + rgbaTextureCount, glMultisampleBuffers.textureBuffer);
    glDeleteFramebuffers (1, &glMultisampleBuffers.FBO);

    GLFramebuffer::Deinitialize ();
}

void GLMultisampleFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, glMultisampleBuffers.FBO);
}

void GLMultisampleFramebuffer::BlitFramebuffer () const
{
    // blit multisample framebuffer to standard framebuffer
    glBindFramebuffer (GL_READ_FRAMEBUFFER, glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, glBuffers.FBO);
    glBlitFramebuffer (0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); 
    glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
}

GLuint GLMultisampleFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return glMultisampleBuffers.RBO;
}

GLuint GLMultisampleFramebuffer::GetFramebufferGLId () const
{
    return glMultisampleBuffers.FBO;
}



