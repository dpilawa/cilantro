#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/Framebuffer.h"

namespace cilantro {

struct GLBuffers
{
    GLuint FBO;
    GLuint RBO;
    GLuint textureBuffer[CILANTRO_MAX_FRAMEBUFFER_TEXTURES];
    GLuint colorAttachments[CILANTRO_MAX_FRAMEBUFFER_TEXTURES];
    GLuint colorNone;
    GLuint depthTextureArray;
};

class CGLFramebuffer : public CFramebuffer
{
public:
    CGLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    virtual ~CGLFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual void BindFramebuffer () const override;
    virtual void BindFramebufferColorTexturesAsColor () const override;
    virtual void BindFramebufferDepthArrayTextureAsColor (unsigned int index) const override;
    virtual void BindFramebufferDepthArrayTextureAsDepth () const override;
    virtual void BindFramebufferRenderbuffer () const override;

    virtual void UnbindFramebuffer () const override;    

    virtual void BlitFramebuffer () const override {};

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint virtual GetFramebufferRenderbufferGLId () const;
    GLuint virtual GetFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers m_glBuffers;

};

} // namespace cilantro

#endif