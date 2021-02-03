#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include "cilantroengine.h"
#include "game/GameComposite.h"

class RenderTarget : public GameComposite
{
public:

    RenderTarget (unsigned int width, unsigned int height);
    virtual ~RenderTarget ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    unsigned int GetWidth () const;
    unsigned int GetHeight () const;

protected:

    // resolution
    unsigned int width;
    unsigned int height;

};

#endif
