#include "graphics/GLMultisampleFramebuffer.h"
#include "system/LogMessage.h"

CGLMultisampleFramebuffer::CGLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount) 
    : CGLFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount)
{
}

void CGLMultisampleFramebuffer::Initialize ()
{
    CGLFramebuffer::Initialize ();

    // create and bind framebuffer
    glGenFramebuffers (1, &m_GlMultisampleBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, m_GlMultisampleBuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (m_RgbTextureCount + m_RgbaTextureCount, m_GlMultisampleBuffers.textureBuffer);
    for (unsigned int i = 0; i < m_RgbTextureCount + m_RgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_GlMultisampleBuffers.textureBuffer[i]);
        glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, 4, (i < m_RgbTextureCount) ? GL_RGB16F : GL_RGBA16F, m_BufferWidth, m_BufferHeight, GL_TRUE);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_GlMultisampleBuffers.textureBuffer[i], 0);
    }

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &m_GlMultisampleBuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, m_GlMultisampleBuffers.RBO);
    glRenderbufferStorageMultisample (GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_BufferWidth, m_BufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);
  
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_GlMultisampleBuffers.RBO);

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Multisample framebuffer is not complete";
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized multisample framebuffer" << m_BufferWidth << m_BufferHeight;
    }
    
}

void CGLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &m_GlMultisampleBuffers.RBO);
    glDeleteTextures (m_RgbTextureCount + m_RgbaTextureCount, m_GlMultisampleBuffers.textureBuffer);
    glDeleteFramebuffers (1, &m_GlMultisampleBuffers.FBO);

    CGLFramebuffer::Deinitialize ();
}

void CGLMultisampleFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, m_GlMultisampleBuffers.FBO);
}

void CGLMultisampleFramebuffer::BlitFramebuffer () const
{
    // blit multisample framebuffer to standard framebuffer
    glBindFramebuffer (GL_READ_FRAMEBUFFER, m_GlMultisampleBuffers.FBO);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, m_GlBuffers.FBO);
    glBlitFramebuffer (0, 0, m_BufferWidth, m_BufferHeight, 0, 0, m_BufferWidth, m_BufferHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); 
    glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
}

void CGLMultisampleFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    CGLFramebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Deinitialize ();
    Initialize ();
}

GLuint CGLMultisampleFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return m_GlMultisampleBuffers.RBO;
}

GLuint CGLMultisampleFramebuffer::GetFramebufferGLId () const
{
    return m_GlMultisampleBuffers.FBO;
}



