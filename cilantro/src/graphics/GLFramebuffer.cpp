#include "cilantroengine.h"
#include "graphics/GLFramebuffer.h"
#include "system/LogMessage.h"

CGLFramebuffer::CGLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount) 
    : CFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount)
{
    for (int i = 0; i < CILANTRO_MAX_FRAMEBUFFER_TEXTURES; i++)
    {
        m_GlBuffers.attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
}

void CGLFramebuffer::Initialize ()
{
    // create and bind framebuffer
    glGenFramebuffers (1, &m_GlBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, m_GlBuffers.FBO);

    // create textures and attach to framebuffer as color attachments
    glGenTextures (m_RgbTextureCount + m_RgbaTextureCount, m_GlBuffers.textureBuffer);
    for (unsigned int i = 0; i < m_RgbTextureCount + m_RgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D, m_GlBuffers.textureBuffer[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, (i < m_RgbTextureCount) ? GL_RGB16F : GL_RGBA16F, m_BufferWidth, m_BufferHeight, 0, (i < m_RgbTextureCount) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture (GL_TEXTURE_2D, 0);
        
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_GlBuffers.textureBuffer[i], 0);
    }

    glDrawBuffers (m_RgbTextureCount + m_RgbaTextureCount, m_GlBuffers.attachments);

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &m_GlBuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, m_GlBuffers.RBO);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_BufferWidth, m_BufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_GlBuffers.RBO);

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete";
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized framebuffer" << m_BufferWidth << m_BufferHeight;
    }
}

void CGLFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &m_GlBuffers.RBO);
    glDeleteTextures (m_RgbTextureCount + m_RgbaTextureCount, m_GlBuffers.textureBuffer);
    glDeleteFramebuffers (1, &m_GlBuffers.FBO);
}

void CGLFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, m_GlBuffers.FBO);
}

void CGLFramebuffer::UnbindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void CGLFramebuffer::BlitFramebuffer () const
{
    // no op
}

void CGLFramebuffer::BindFramebufferTextures () const
{
    for (unsigned int i = 0; i < GetTextureCount (); i++)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, GetFramebufferTextureGLId (i));
    }
}

void CGLFramebuffer::BindFramebufferRenderbuffer () const
{
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GetFramebufferRenderbufferGLId ());
}

void CGLFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    CFramebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Deinitialize (); 
    Initialize ();
}

GLuint CGLFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return m_GlBuffers.RBO;
}

GLuint CGLFramebuffer::GetFramebufferTextureGLId (unsigned int textureNumber) const
{
    return m_GlBuffers.textureBuffer[textureNumber];
}

GLuint CGLFramebuffer::GetFramebufferGLId () const
{
    return m_GlBuffers.FBO;
}

