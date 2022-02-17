#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "cilantroengine.h"

class Framebuffer 
{
public:
    Framebuffer (unsigned int bufferWidth, unsigned int bufferHeight);
    virtual ~Framebuffer ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void BindFramebuffer () const = 0;
    virtual void UnbindFramebuffer () const = 0;
    virtual void BlitFramebuffer () const = 0;
    virtual void BindFramebufferTextures () const = 0;
    virtual void BindFramebufferRenderbuffer () const = 0;

    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;

    unsigned int GetTextureCount() const;
    unsigned int GetRGBTextureCount () const;
    unsigned int GetRGBATextureCount () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

protected:

    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;
    unsigned int bufferWidth;
    unsigned int bufferHeight;
};

#endif