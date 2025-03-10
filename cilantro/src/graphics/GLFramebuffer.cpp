#include "cilantroengine.h"
#include "graphics/GLFramebuffer.h"
#include "system/LogMessage.h"

namespace cilantro {

CGLFramebuffer::CGLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled) 
    : CFramebuffer (bufferWidth, bufferHeight, rgbTextureCount, rgbaTextureCount, depthBufferArrayLayerCount, depthStencilRenderbufferEnabled)
{
    for (unsigned int i = 0; i < CILANTRO_MAX_FRAMEBUFFER_TEXTURES; ++i)
    {
        m_glBuffers.colorAttachments[i] = static_cast <GLuint> (GL_COLOR_ATTACHMENT0 + i);
    }
    m_glBuffers.colorNone = GL_NONE;
}

void CGLFramebuffer::Initialize ()
{
    GLint fbStatus;

    // create and bind framebuffer
    glGenFramebuffers (1, &m_glBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, m_glBuffers.FBO);

    // create textures and attach to framebuffer as color attachments
    glGenTextures (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glBuffers.textureBuffer);
    for (unsigned int i = 0; i < m_rgbTextureCount + m_rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D, m_glBuffers.textureBuffer[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, (i < m_rgbTextureCount) ? GL_RGB16F : GL_RGBA16F, m_bufferWidth, m_bufferHeight, 0, (i < m_rgbTextureCount) ? GL_RGB : GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture (GL_TEXTURE_2D, 0);
     
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_glBuffers.textureBuffer[i], 0);
    }

    // specify color buffers to draw to
    if (m_rgbTextureCount + m_rgbaTextureCount > 0)
    {
        glDrawBuffers (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glBuffers.colorAttachments);
    }
    else
    {
        glDrawBuffers (1, &m_glBuffers.colorNone);
    }

    if (m_depthBufferArrayLayerCount > 0)
    {
        // create depth buffer array (used by shadow maps)
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glGenTextures (1, &m_glBuffers.depthTextureArray);
        glBindTexture (GL_TEXTURE_2D_ARRAY, m_glBuffers.depthTextureArray);
        glTexImage3D (GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_bufferWidth, m_bufferHeight, static_cast<GLsizei> (m_depthBufferArrayLayerCount), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);        
        glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
        glBindTexture (GL_TEXTURE_2D_ARRAY, 0);

        glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_glBuffers.depthTextureArray, 0);
    }
    else if (m_depthStencilRenderbufferEnabled)
    {
        // create renderbuffer for a (combined) depth and stencil buffer
        glGenRenderbuffers (1, &m_glBuffers.RBO);
        glBindRenderbuffer (GL_RENDERBUFFER, m_glBuffers.RBO);
        glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_bufferWidth, m_bufferHeight);
        glBindRenderbuffer (GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_glBuffers.RBO);
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
        LogMessage (MSG_LOCATION) << "Initialized framebuffer" << m_bufferWidth << m_bufferHeight;
    }
}

void CGLFramebuffer::Deinitialize ()
{
    glDeleteRenderbuffers (1, &m_glBuffers.RBO);
    glDeleteTextures (static_cast<GLsizei> (m_rgbTextureCount + m_rgbaTextureCount), m_glBuffers.textureBuffer);
    glDeleteFramebuffers (1, &m_glBuffers.FBO);
}

void CGLFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, m_glBuffers.FBO);
}

void CGLFramebuffer::BindFramebufferColorTexturesAsColor () const
{
    for (unsigned int i = 0; i < GetColorTextureCount (); ++i)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, GetFramebufferTextureGLId (i));
    }
}

void CGLFramebuffer::BindFramebufferDepthArrayTextureAsColor (unsigned int index) const
{
    glActiveTexture (static_cast<GLenum> (GL_TEXTURE0 + index));
    glBindTexture (GL_TEXTURE_2D_ARRAY, m_glBuffers.depthTextureArray);
}

void CGLFramebuffer::BindFramebufferDepthArrayTextureAsDepth () const
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_glBuffers.depthTextureArray, 0);
}

void CGLFramebuffer::BindFramebufferRenderbuffer () const
{
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GetFramebufferRenderbufferGLId ());
}

void CGLFramebuffer::UnbindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
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
    return m_glBuffers.RBO;
}

GLuint CGLFramebuffer::GetFramebufferTextureGLId (unsigned int textureNumber) const
{
    return m_glBuffers.textureBuffer[textureNumber];
}

GLuint CGLFramebuffer::GetFramebufferGLId () const
{
    return m_glBuffers.FBO;
}

} // namespace cilantro

