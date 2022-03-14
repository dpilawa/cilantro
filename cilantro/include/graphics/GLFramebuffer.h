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
    GLuint attachments[CILANTRO_MAX_FRAMEBUFFER_TEXTURES];
};

class CGLFramebuffer : public CFramebuffer
{
public:
    CGLFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount);
    virtual ~CGLFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual void BindFramebuffer () const override;
    virtual void UnbindFramebuffer () const override;
    virtual void BlitFramebuffer () const override;
    void BindFramebufferTextures () const override;
    void BindFramebufferRenderbuffer () const override;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint virtual GetFramebufferRenderbufferGLId () const;
    GLuint virtual GetFramebufferTextureGLId (unsigned int textureNumber) const;
    GLuint virtual GetFramebufferGLId () const;

protected:

    GLBuffers m_GlBuffers;

};

#endif