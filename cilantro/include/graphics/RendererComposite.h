#ifndef _RENDERERCOMPOSITE_H_
#define _RENDERERCOMPOSITE_H_

#include "cilantroengine.h"

class Renderer;

class RendererComposite
{
public:

    RendererComposite ();
    virtual ~RendererComposite ();

    // attach object to renderer
    __EAPI void AttachToRenderer (Renderer* renderer);

protected:

    // renderer pointer
    Renderer* renderer;

};

#endif