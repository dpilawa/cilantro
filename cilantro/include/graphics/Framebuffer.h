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

    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;

    virtual void SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight);

protected:

    unsigned int bufferWidth;
    unsigned int bufferHeight;
};

#endif