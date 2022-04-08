#pragma once

#include "cilantroengine.h"
#include "graphics/IFramebuffer.h"

class CFramebuffer : public IFramebuffer
{
public:
    CFramebuffer (uint32_t bufferWidth, uint32_t bufferHeight, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    virtual ~CFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual uint32_t GetWidth () const override final;
    virtual uint32_t GetHeight () const override final;

    virtual size_t GetColorTextureCount() const override final;
    virtual size_t GetRGBTextureCount () const override final;
    virtual size_t GetRGBATextureCount () const override final;
    virtual size_t GetDepthArrayLayerCount () const override final;

    virtual bool IsDepthStencilRenderbufferEnabled () const override final;

    void SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    size_t      m_rgbTextureCount;
    size_t      m_rgbaTextureCount;
    size_t      m_depthBufferArrayLayerCount;
    uint32_t    m_bufferWidth;
    uint32_t    m_bufferHeight;
    bool        m_depthStencilRenderbufferEnabled;
};