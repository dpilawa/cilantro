#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/GLFramebuffer.h"

class CGLMultisampleFramebuffer : public CGLFramebuffer 
{
public:
    CGLMultisampleFramebuffer (uint32_t bufferWidth, uint32_t bufferHeight, size_t rgbTextureCount, size_t rgbaTextureCount, size_t dsBufferArraySize, bool hasDepthStencilRB);
    virtual ~CGLMultisampleFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual void BindFramebuffer () const override;
    virtual void BlitFramebuffer () const override;

    virtual void SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint virtual GetFramebufferRenderbufferGLId () const override;
    GLuint virtual GetFramebufferGLId () const override;

protected:

    GLBuffers m_glMultisampleBuffers;

};

#endif