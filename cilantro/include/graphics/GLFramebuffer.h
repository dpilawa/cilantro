#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "graphics/Framebuffer.h"
#include "glad/glad.h"

struct Framebuffers
{
public:
    GLuint FBO;
    GLuint RBO;
    GLuint textureBuffer;
};

class GLFramebuffer : public Framebuffer
{
public:
    GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight);
    virtual ~GLFramebuffer ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void BindFramebuffer () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

    GLuint GetFramebufferTextureGLId () const;
    GLuint GetFramebufferGLId () const;

protected:

    Framebuffers framebuffers;

};

#endif