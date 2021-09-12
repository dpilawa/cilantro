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

    void Initialize ();
    void Deinitialize ();

    virtual void BindFramebuffer () const;
    virtual void BlitFramebuffer ();

    GLuint virtual GetDrawFramebufferRenderbufferGLId () const;
    GLuint virtual GetDrawFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetDrawFramebufferGLId () const;

protected:

    GLBuffers glMultisampleFramebuffers;

};

#endif