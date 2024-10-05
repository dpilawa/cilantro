#pragma once

#include "cilantroengine.h"
#include <cstddef>
#include <cstdint>

struct IFramebuffer 
{
    virtual ~IFramebuffer () {};

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void BindFramebuffer () const = 0;
    virtual void BindFramebufferColorTexturesAsColor () const = 0;
    virtual void BindFramebufferDepthArrayTextureAsColor (size_t index) const = 0;
    virtual void BindFramebufferDepthArrayTextureAsDepth () const = 0;
    virtual void BindFramebufferRenderbuffer () const = 0;
    
    virtual void UnbindFramebuffer () const = 0;    

    virtual void BlitFramebuffer () const = 0;

    virtual uint32_t GetWidth () const = 0;
    virtual uint32_t GetHeight () const = 0;

    virtual size_t GetColorTextureCount() const = 0;
    virtual size_t GetRGBTextureCount () const = 0;
    virtual size_t GetRGBATextureCount () const = 0;
    virtual size_t GetDepthArrayLayerCount () const = 0;

    virtual bool IsDepthStencilRenderbufferEnabled () const = 0;
    virtual bool IsDepthArrayEnabled () const = 0;

    virtual void SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight) = 0;
};
