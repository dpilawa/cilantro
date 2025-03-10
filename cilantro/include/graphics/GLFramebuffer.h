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

class GLFramebuffer : public Framebuffer
{
public:
    __EAPI GLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    __EAPI virtual ~GLFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void BindFramebuffer () const override;
    __EAPI virtual void BindFramebufferColorTexturesAsColor () const override;
    __EAPI virtual void BindFramebufferDepthArrayTextureAsColor (unsigned int index) const override;
    __EAPI virtual void BindFramebufferDepthArrayTextureAsDepth () const override;
    __EAPI virtual void BindFramebufferRenderbuffer () const override;

    __EAPI virtual void UnbindFramebuffer () const override;    

    __EAPI virtual void BlitFramebuffer () const override {};

    __EAPI virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI GLuint virtual GetFramebufferRenderbufferGLId () const;
    __EAPI GLuint virtual GetFramebufferTextureGLId (unsigned int textureNumber) const;
    __EAPI GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers m_glBuffers;

};

} // namespace cilantro

#endif