#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/GLFramebuffer.h"

class CGLMultisampleFramebuffer : public CGLFramebuffer 
{
public:
    CGLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int dsBufferArraySize, bool hasDepthStencilRB);
    virtual ~CGLMultisampleFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual void BindFramebuffer () const override;
    virtual void BlitFramebuffer () const override;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint virtual GetFramebufferRenderbufferGLId () const override;
    GLuint virtual GetFramebufferGLId () const override;

protected:

    GLBuffers m_glMultisampleBuffers;

};

#endif