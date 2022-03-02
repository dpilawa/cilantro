#include "cilantroengine.h"
#include "graphics/GLFramebuffer.h"
#include "system/LogMessage.h"

GLFramebuffer::GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount) : Framebuffer (bufferWidth, bufferHeight)
{
    this->rgbTextureCount = rgbTextureCount;
    this->rgbaTextureCount = rgbaTextureCount;

    for (int i = 0; i < CILANTRO_MAX_FRAMEBUFFER_TEXTURES; i++)
    {
        glBuffers.attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
}

GLFramebuffer::~GLFramebuffer ()
{
}

void GLFramebuffer::Initialize ()
{
    // create and bind framebuffer
    glGenFramebuffers (1, &glBuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, glBuffers.FBO);

    // create textures and attach to framebuffer as color attachments
    glGenTextures (rgbTextureCount + rgbaTextureCount, glBuffers.textureBuffer);
    for (unsigned int i = 0; i < rgbTextureCount + rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D, glBuffers.textureBuffer[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, (i < rgbTextureCount) ? GL_RGB16F : GL_RGBA16F, bufferWidth, bufferHeight, 0, (i < rgbTextureCount) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture (GL_TEXTURE_2D, 0);
        
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, glBuffers.textureBuffer[i], 0);
    }

    glDrawBuffers (rgbTextureCount + rgbaTextureCount, glBuffers.attachments);

    // create renderbuffer for a (combined) depth and stencil buffer
    glGenRenderbuffers (1, &glBuffers.RBO);
    glBindRenderbuffer (GL_RENDERBUFFER, glBuffers.RBO);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glBuffers.RBO);

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
    glDeleteRenderbuffers (1, &glBuffers.RBO);
    glDeleteTextures (rgbTextureCount + rgbaTextureCount, glBuffers.textureBuffer);
    glDeleteFramebuffers (1, &glBuffers.FBO);
}

void GLFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, glBuffers.FBO);
}

void GLFramebuffer::UnbindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void GLFramebuffer::BlitFramebuffer () const
{
    // no op
}

void GLFramebuffer::BindFramebufferTextures () const
{
    for (unsigned int i = 0; i < GetTextureCount (); i++)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, GetFramebufferTextureGLId (i));
    }
}

void GLFramebuffer::BindFramebufferRenderbuffer () const
{
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GetFramebufferRenderbufferGLId ());
}

void GLFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    Deinitialize ();
    Framebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Initialize ();
}

GLuint GLFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return glBuffers.RBO;
}

GLuint GLFramebuffer::GetFramebufferTextureGLId (unsigned int textureNumber) const
{
    return glBuffers.textureBuffer[textureNumber];
}

GLuint GLFramebuffer::GetFramebufferGLId () const
{
    return glBuffers.FBO;
}

