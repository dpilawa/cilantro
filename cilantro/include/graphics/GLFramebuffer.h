#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Framebuffer.h"

struct GLBuffers
{
public:
    GLuint FBO;
    GLuint RBO;
    GLuint textureBuffer[MAX_FRAMEBUFFER_TEXTURES];
    GLuint attachments[MAX_FRAMEBUFFER_TEXTURES];
};

class GLFramebuffer : public Framebuffer
{
public:
    GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount);
    virtual ~GLFramebuffer ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void BindFramebuffer () const;
    virtual void UnbindFramebuffer () const;
    virtual void BlitFramebuffer () const;
    void BindFramebufferTextures () const;
    void BindFramebufferRenderbuffer () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

    GLuint virtual GetFramebufferRenderbufferGLId () const;
    GLuint virtual GetFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers glBuffers;

};

#endif