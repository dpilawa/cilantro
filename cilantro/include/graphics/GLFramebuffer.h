#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"

struct Framebuffers
{
public:
    GLuint FBO;
    GLuint RBO;
    GLuint textureBuffer;
};

class GLFramebuffer 
{
public:

    GLFramebuffer () = delete;
    GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight);
    virtual ~GLFramebuffer ();

    virtual void BindFramebuffer () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

    GLuint GetFramebufferTexture () const;
    GLuint GetFramebuffer () const;

protected:

    Framebuffers framebuffers;

    unsigned int bufferWidth;
    unsigned int bufferHeight;

private:

    void Initialize ();
    void Deinitialize ();
};

#endif