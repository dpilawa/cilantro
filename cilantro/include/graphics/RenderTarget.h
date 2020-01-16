#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include "game/GameLoop.h"

class RenderTarget
{
public:
    RenderTarget () = delete;
    RenderTarget (GameLoop* gameLoop, unsigned int width, unsigned int height);
    virtual ~RenderTarget ();

    virtual void OnFrame () = 0;

    unsigned int GetWidth () const;
    unsigned int GetHeight () const;

protected:

    GameLoop* gameLoop;

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    // resolution
    unsigned int width;
    unsigned int height;

};

#endif
