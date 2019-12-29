#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFramebuffer.h"

class GLMultisampleFramebuffer : public GLFramebuffer 
{
public:

    GLMultisampleFramebuffer () = delete;
    GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight);
    virtual ~GLMultisampleFramebuffer ();

    __EAPI virtual void BindFramebuffer () const;
    __EAPI virtual void BlitFramebuffer ();

	__EAPI void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

	__EAPI GLuint GetMultisampleFramebufferTexture () const;
    __EAPI GLuint GetMultisampleFramebuffer () const;

protected:

    Framebuffers multisampleFramebuffers;

private:

    void Initialize ();
    void Deinitialize ();
};

#endif