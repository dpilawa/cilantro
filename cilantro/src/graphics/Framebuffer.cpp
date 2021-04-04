#include "cilantroengine.h"
#include "graphics/Framebuffer.h"


Framebuffer::Framebuffer (unsigned int bufferWidth, unsigned int bufferHeight)
{
    this->bufferWidth = bufferWidth;
    this->bufferHeight = bufferHeight;
}

Framebuffer::~Framebuffer ()
{
}

unsigned int Framebuffer::GetWidth () const 
{
    return bufferWidth;
}

unsigned int Framebuffer::GetHeight () const 
{
    return bufferHeight;
}

void Framebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    this->bufferWidth = bufferWidth;
    this->bufferHeight = bufferHeight;
}
