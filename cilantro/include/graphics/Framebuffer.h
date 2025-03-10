#pragma once

#include "cilantroengine.h"
#include "graphics/IFramebuffer.h"

namespace cilantro {

class CFramebuffer : public IFramebuffer
{
public:
    CFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled);
    virtual ~CFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual unsigned int GetWidth () const override final;
    virtual unsigned int GetHeight () const override final;

    virtual unsigned int GetColorTextureCount() const override final;
    virtual unsigned int GetRGBTextureCount () const override final;
    virtual unsigned int GetRGBATextureCount () const override final;
    virtual unsigned int GetDepthArrayLayerCount () const override final;

    virtual bool IsDepthStencilRenderbufferEnabled () const override final;
    virtual bool IsDepthArrayEnabled () const override final;

    void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

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