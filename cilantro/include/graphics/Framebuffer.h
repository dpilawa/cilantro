#pragma once

#include "cilantroengine.h"
#include "graphics/IFramebuffer.h"

class CFramebuffer : public IFramebuffer
{
public:
    CFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount);
    virtual ~CFramebuffer () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual unsigned int GetWidth () const override final;
    virtual unsigned int GetHeight () const override final;

    virtual unsigned int GetTextureCount() const override final;
    virtual unsigned int GetRGBTextureCount () const override final;
    virtual unsigned int GetRGBATextureCount () const override final;

    void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    unsigned int m_RgbTextureCount;
    unsigned int m_RgbaTextureCount;
    unsigned int m_BufferWidth;
    unsigned int m_BufferHeight;
};