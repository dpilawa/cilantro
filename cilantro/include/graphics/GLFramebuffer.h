#ifndef _GLFRAMEBUFFER_H_
#define _GLFRAMEBUFFER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Framebuffer.h"

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
    CGLFramebuffer (uint32_t bufferWidth, uint32_t bufferHeight, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    virtual ~CGLFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual void BindFramebuffer () const override;
    virtual void BindFramebufferColorTexturesAsColor () const override;
    virtual void BindFramebufferDepthArrayTextureAsColor (size_t index) const override;
    virtual void BindFramebufferDepthArrayTextureAsDepth () const override;
    virtual void BindFramebufferRenderbuffer () const override;

    virtual void UnbindFramebuffer () const override;    

    virtual void BlitFramebuffer () const override {};

    virtual void SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint virtual GetFramebufferRenderbufferGLId () const;
    GLuint virtual GetFramebufferTextureGLId (size_t textureNumber) const;
    GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers m_glBuffers;

};

#endif