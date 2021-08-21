#include "cilantroengine.h"
#include "graphics/GLFramebuffer.h"
#include "system/LogMessage.h"

GLFramebuffer::GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount) : Framebuffer (bufferWidth, bufferHeight)
{
    this->rgbTextureCount = rgbTextureCount;
    this->rgbaTextureCount = rgbaTextureCount;

    for (int i = 0; i < MAX_FRAMEBUFFER_TEXTURES; i++)
    {
        framebuffers.attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
}

GLFramebuffer::~GLFramebuffer ()
{
}

void GLFramebuffer::Initialize ()
{
    // create and bind framebuffer
    glGenFramebuffers (1, &framebuffers.FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffers.FBO);

    // create textures and attach to framebuffer as color attachments
    glGenTextures (rgbTextureCount + rgbaTextureCount, framebuffers.textureBuffer);
    for (unsigned int i = 0; i < rgbTextureCount + rgbaTextureCount; i++)
    {
        glBindTexture (GL_TEXTURE_2D, framebuffers.textureBuffer[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, (i < rgbTextureCount) ? GL_RGB : GL_RGBA, bufferWidth, bufferHeight, 0, (i < rgbTextureCount) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
        
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffers.textureBuffer[i], 0);
    }

    glDrawBuffers (rgbTextureCount + rgbaTextureCount, framebuffers.attachments);

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
    glDeleteTextures (rgbTextureCount + rgbaTextureCount, framebuffers.textureBuffer);
    glDeleteFramebuffers (1, &framebuffers.FBO);
}

void GLFramebuffer::BindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffers.FBO);
}

void GLFramebuffer::UnbindFramebuffer () const
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void GLFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    // resize framebuffer texture and viewport
    Deinitialize ();
    Framebuffer::SetFramebufferResolution (bufferWidth, bufferHeight);
    Initialize ();
}

int GLFramebuffer::GetTextureCount () const
{
    return rgbTextureCount + rgbaTextureCount;
}

GLuint GLFramebuffer::GetFramebufferRenderbufferGLId () const
{
    return framebuffers.RBO;
}

GLuint GLFramebuffer::GetFramebufferTextureGLId (unsigned int textureNumber) const
{
    return framebuffers.textureBuffer[textureNumber];
}

GLuint GLFramebuffer::GetFramebufferGLId () const
{
    return framebuffers.FBO;
}