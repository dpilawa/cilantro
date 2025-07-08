#include "graphics/GLMultisampleFramebuffer.h"
#include "system/LogMessage.h"

namespace cilantro {

GLMultisampleFramebuffer::GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled) 
    : GLFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount, depthBufferArrayLayerCount, depthStencilRenderbufferEnabled)
{
    for (unsigned int i = 0; i < CILANTRO_MAX_FRAMEBUFFER_TEXTURES; ++i)
    {
        m_glMultisampleBuffers.colorAttachments[i] = static_cast <GLuint> (GL_COLOR_ATTACHMENT0 + i);
    }
    m_glMultisampleBuffers.colorNone = GL_NONE;
}

void GLMultisampleFramebuffer::Initialize ()
{
    GLint fbStatus;

    GLFramebuffer::Initialize ();

    // create and bind framebuffer
    glGenFramebuffers (1, &m_glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, m_glMultisampleBuffers.FBO);

    // create texture and attach to framebuffer as color attachment
    glGenTextures (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glMultisampleBuffers.textureBuffer);
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
        glDrawBuffers (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glMultisampleBuffers.colorAttachments);
    }
    else
    {
        glDrawBuffer (GL_NONE);
    }

    if (m_depthBufferArrayLayerCount > 0)
    {
        // create depth buffer array (used by shadow maps)
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        // create depth texture array
        glGenTextures (1, &m_glBuffers.depthTextureArray);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_glBuffers.depthTextureArray);
        glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, CILANTRO_MULTISAMPLE, GL_DEPTH_COMPONENT32F, CILANTRO_SHADOW_MAP_SIZE, CILANTRO_SHADOW_MAP_SIZE, static_cast<GLsizei> (m_depthBufferArrayLayerCount), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        glTexParameteri (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv (GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
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
    if ((fbStatus = glCheckFramebufferStatus (GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (fbStatus)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete (incomplete attachment)";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete (incomplete missing attachment)";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete (incomplete layer targets)";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete (unsupported)";
            break;
        default:
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer is not complete (unknown error)";
            break;
        }
    }
    else
    {
        LogMessage (MSG_LOCATION) << "Initialized multisample framebuffer" << m_bufferWidth << m_bufferHeight;
    }
    
}

void GLMultisampleFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &m_glMultisampleBuffers.RBO);
    glDeleteTextures (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glMultisampleBuffers.textureBuffer);
    glDeleteFramebuffers (1, &m_glMultisampleBuffers.FBO);

    GLFramebuffer::Deinitialize ();
}

void GLMultisampleFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, m_glMultisampleBuffers.FBO);
}

void GLMultisampleFramebuffer::BlitFramebuffer () const
{
    // blit multisample framebuffer to standard framebuffer
    glBindFramebuffer (GL_READ_FRAMEBUFFER, m_glMultisampleBuffers.FBO);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, m_glBuffers.FBO);
    glBlitFramebuffer (0, 0, m_bufferWidth, m_bufferHeight, 0, 0, m_bufferWidth, m_bufferHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); 
    glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
}

void GLMultisampleFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    GLFramebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Deinitialize ();
    Initialize ();
}

GLuint GLMultisampleFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return m_glMultisampleBuffers.RBO;
}

GLuint GLMultisampleFramebuffer::GetFramebufferGLId () const
{
    return m_glMultisampleBuffers.FBO;
}

} // namespace cilantro



