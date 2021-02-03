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


void Framebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    this->bufferWidth = bufferWidth;
    this->bufferHeight = bufferHeight;
}
