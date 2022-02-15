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

unsigned int Framebuffer::GetTextureCount () const
{
    return rgbTextureCount + rgbaTextureCount;
}

unsigned int Framebuffer::GetRGBTextureCount () const
{
    return rgbTextureCount;
}

unsigned int Framebuffer::GetRGBATextureCount () const
{
    return rgbaTextureCount;
}

void Framebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    this->bufferWidth = bufferWidth;
    this->bufferHeight = bufferHeight;
}
