#include "cilantroengine.h"
#include "graphics/RenderTarget.h"

RenderTarget::RenderTarget (unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
}

RenderTarget::~RenderTarget ()
{
}

unsigned int RenderTarget::GetWidth () const
{
    return width;
}

unsigned int RenderTarget::GetHeight () const
{
    return height;
}