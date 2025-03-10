#ifndef _GLMULTISAMPLEFRAMEBUFFER_H_
#define _GLMULTISAMPLEFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/GLFramebuffer.h"

namespace cilantro {

class GLMultisampleFramebuffer : public GLFramebuffer 
{
public:
    __EAPI GLMultisampleFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int dsBufferArraySize, bool hasDepthStencilRB);
    __EAPI virtual ~GLMultisampleFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void BindFramebuffer () const override;
    __EAPI virtual void BlitFramebuffer () const override;

    __EAPI virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI GLuint virtual GetFramebufferRenderbufferGLId () const override;
    __EAPI GLuint virtual GetFramebufferGLId () const override;

protected:

    GLBuffers m_glMultisampleBuffers;

};

} // namespace cilantro

#endif