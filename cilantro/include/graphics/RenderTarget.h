#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include "cilantroengine.h"

class RenderTarget
{
public:

    RenderTarget (unsigned int width, unsigned int height);
    virtual ~RenderTarget ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;

protected:

    // resolution
    unsigned int width;
    unsigned int height;

};

#endif
