#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/GLFramebuffer.h"

class GLMultisampleFramebuffer : public GLFramebuffer 
{
public:
    GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight);
    virtual ~GLMultisampleFramebuffer ();

    void Initialize ();
    void Deinitialize ();

    virtual void BindFramebuffer () const;
    virtual void BlitFramebuffer ();

    void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

    GLuint GetMultisampleFramebufferTexture () const;
    GLuint GetMultisampleFramebuffer () const;

protected:

    Framebuffers multisampleFramebuffers;

};

#endif