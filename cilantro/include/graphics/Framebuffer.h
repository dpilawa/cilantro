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

    unsigned int GetWidth () const;
    unsigned int GetHeight () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

protected:

    unsigned int bufferWidth;
    unsigned int bufferHeight;
};

#endif