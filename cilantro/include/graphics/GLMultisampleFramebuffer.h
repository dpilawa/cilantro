#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/GLFramebuffer.h"

class GLMultisampleFramebuffer : public GLFramebuffer 
{
public:
    GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount);
    virtual ~GLMultisampleFramebuffer ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void BindFramebuffer () const;
    virtual void BlitFramebuffer () const;

    GLuint virtual GetFramebufferRenderbufferGLId () const;
    GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers glMultisampleBuffers;

};

#endif