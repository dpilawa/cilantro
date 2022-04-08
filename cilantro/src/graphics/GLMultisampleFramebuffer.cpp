#include "graphics/GLMultisampleFramebuffer.h"
#include "system/LogMessage.h"

CGLMultisampleFramebuffer::CGLMultisampleFramebuffer (uint32_t bufferWidth, uint32_t bufferHeight, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled) 
    : CGLFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount, depthBufferArrayLayerCount, depthStencilRenderbufferEnabled)
{
}

void CGLMultisampleFramebuffer::Initialize ()
{
    CGLFramebuffer::Initialize ();

    // create and bind framebuffer
    glGenFramebuffers (1, &m_glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, m_glMultisampleBuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (m_rgbTextureCount + m_rgbaTextureCount, m_glMultisampleBuffers.textureBuffer);
    for (unsigned int i = 0; i < m_rgbTextureCount + m_rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_glMultisampleBuffers.textureBuffer[i]);
        glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, CILANTRO_MULTISAMPLE, (i < m_rgbTextureCount) ? GL_RGB16F : GL_RGBA16F, m_bufferWidth, m_bufferHeight, GL_TRUE);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_glMultisampleBuffers.textureBuffer[i], 0);
    }

    // specify color buffers to draw to
    if (m_rgbTextureCount + m_rgbaTextureCount > 0)
    {
        glDrawBuffers (m_rgbTextureCount + m_rgbaTextureCount, m_glMultisampleBuffers.colorAttachments);
    }
    else
    {
        glDrawBuffer (GL_NONE);
    }

    if (m_depthBufferArrayLayerCount > 0)
    {
        // create depth buffer array (used by shadow maps)
        glGenTextures(1, &m_glBuffers.depthTextureArray);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_glBuffers.depthTextureArray);
        glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, CILANTRO_MULTISAMPLE, GL_DEPTH_COMPONENT32F, CILANTRO_SHADOW_MAP_SIZE, CILANTRO_SHADOW_MAP_SIZE, m_depthBufferArrayLayerCount, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);

        glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_glBuffers.depthTextureArray, 0);
    }
    else if (m_depthStencilRenderbufferEnabled)
    {
        // create renderbuffer for a (combined) depth and stencil buffer
        glGenRenderbuffers (1, &m_glMultisampleBuffers.RBO);
        glBindRenderbuffer (GL_RENDERBUFFER, m_glMultisampleBuffers.RBO);
        glRenderbufferStorageMultisample (GL_RENDERBUFFER, CILANTRO_MULTISAMPLE, GL_DEPTH24_STENCIL8, m_bufferWidth, m_bufferHeight);
        glBindRenderbuffer (GL_RENDERBUFFER, 0);
    
        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_glMultisampleBuffers.RBO);
    }

    // check status
    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Multisample framebuffer is not complete";
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized multisample framebuffer" << m_bufferWidth << m_bufferHeight;
    }
    
}

void CGLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &m_glMultisampleBuffers.RBO);
    glDeleteTextures (m_rgbTextureCount + m_rgbaTextureCount, m_glMultisampleBuffers.textureBuffer);
    glDeleteFramebuffers (1, &m_glMultisampleBuffers.FBO);

    CGLFramebuffer::Deinitialize ();
}

void CGLMultisampleFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, m_glMultisampleBuffers.FBO);
}

void CGLMultisampleFramebuffer::BlitFramebuffer () const
{
    // blit multisample framebuffer to standard framebuffer
    glBindFramebuffer (GL_READ_FRAMEBUFFER, m_glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, m_glBuffers.FBO);
    glBlitFramebuffer (0, 0, m_bufferWidth, m_bufferHeight, 0, 0, m_bufferWidth, m_bufferHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); 
    glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
}

void CGLMultisampleFramebuffer::SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight)
{
    // resize framebuffer texture and viewport
    CGLFramebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Deinitialize ();
    Initialize ();
}

GLuint CGLMultisampleFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return m_glMultisampleBuffers.RBO;
}

GLuint CGLMultisampleFramebuffer::GetFramebufferGLId () const
{
    return m_glMultisampleBuffers.FBO;
}



