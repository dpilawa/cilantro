#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "graphics/Framebuffer.h"
#include "glad/glad.h"
#include <vector>

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

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

    int GetTextureCount() const;

    GLuint virtual GetDrawFramebufferRenderbufferGLId () const;
    GLuint virtual GetDrawFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetDrawFramebufferGLId () const;

    GLuint virtual GetReadFramebufferRenderbufferGLId () const;
    GLuint virtual GetReadFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetReadFramebufferGLId () const;

protected:

    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;
    GLBuffers glBuffers;

};

#endif