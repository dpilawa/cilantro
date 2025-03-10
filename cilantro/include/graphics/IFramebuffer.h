#pragma once

#include "cilantroengine.h"
#include <cstddef>
#include <cstdint>

namespace cilantro {

struct IFramebuffer 
{
    virtual ~IFramebuffer () {};

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void BindFramebuffer () const = 0;
    virtual void BindFramebufferColorTexturesAsColor () const = 0;
    virtual void BindFramebufferDepthArrayTextureAsColor (unsigned int index) const = 0;
    virtual void BindFramebufferDepthArrayTextureAsDepth () const = 0;
    virtual void BindFramebufferRenderbuffer () const = 0;
    
    virtual void UnbindFramebuffer () const = 0;    

    virtual void BlitFramebuffer () const = 0;

    virtual unsigned int GetWidth () const = 0;
    virtual unsigned int GetHeight () const = 0;

    virtual unsigned int GetColorTextureCount() const = 0;
    virtual unsigned int GetRGBTextureCount () const = 0;
    virtual unsigned int GetRGBATextureCount () const = 0;
    virtual unsigned int GetDepthArrayLayerCount () const = 0;

    virtual bool IsDepthStencilRenderbufferEnabled () const = 0;
    virtual bool IsDepthArrayEnabled () const = 0;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) = 0;
};

} // namespace cilantro
