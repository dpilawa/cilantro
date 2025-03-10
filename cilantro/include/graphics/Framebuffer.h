#pragma once

#include "cilantroengine.h"
#include "graphics/IFramebuffer.h"

namespace cilantro {

class Framebuffer : public IFramebuffer
{
public:
    __EAPI Framebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    __EAPI virtual ~Framebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual unsigned int GetWidth () const override final;
    __EAPI virtual unsigned int GetHeight () const override final;

    __EAPI virtual unsigned int GetColorTextureCount() const override final;
    __EAPI virtual unsigned int GetRGBTextureCount () const override final;
    __EAPI virtual unsigned int GetRGBATextureCount () const override final;
    __EAPI virtual unsigned int GetDepthArrayLayerCount () const override final;

    __EAPI virtual bool IsDepthStencilRenderbufferEnabled () const override final;
    __EAPI virtual bool IsDepthArrayEnabled () const override final;

    __EAPI void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    unsigned int    m_rgbTextureCount;
    unsigned int    m_rgbaTextureCount;
    unsigned int    m_depthBufferArrayLayerCount;
    unsigned int    m_bufferWidth;
    unsigned int    m_bufferHeight;
    bool            m_depthStencilRenderbufferEnabled;
};

} // namespace cilantro