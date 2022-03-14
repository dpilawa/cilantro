#pragma once

#include "cilantroengine.h"

struct IFramebuffer 
{
    virtual ~IFramebuffer () {};

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void BindFramebuffer () const = 0;
    virtual void UnbindFramebuffer () const = 0;
    virtual void BlitFramebuffer () const = 0;
    virtual void BindFramebufferTextures () const = 0;
    virtual void BindFramebufferRenderbuffer () const = 0;

    virtual unsigned int GetWidth () const = 0;
    virtual unsigned int GetHeight () const = 0;

    virtual unsigned int GetTextureCount() const = 0;
    virtual unsigned int GetRGBTextureCount () const = 0;
    virtual unsigned int GetRGBATextureCount () const = 0;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight) = 0;
};
