#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "graphics/Framebuffer.h"
#include "glad/glad.h"
#include <vector>

#define MAX_FRAMEBUFFER_TEXTURES 8

struct Framebuffers
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

    GLuint GetFramebufferRenderbufferGLId () const;
    GLuint GetFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint GetFramebufferGLId () const;

protected:

    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;
    Framebuffers framebuffers;

};

#endif