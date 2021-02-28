#include "graphics/RendererComposite.h"

RendererComposite::RendererComposite ()
{

}

RendererComposite::~RendererComposite ()
{
    
}

void RendererComposite::AttachToRenderer (Renderer* renderer)
{
    this->renderer = renderer;
}