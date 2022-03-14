#include "graphics/Framebuffer.h"

CFramebuffer::CFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount)
    : m_BufferWidth (bufferWidth)
    , m_BufferHeight (bufferHeight)
    , m_RgbTextureCount (rgbTextureCount)
    , m_RgbaTextureCount (rgbaTextureCount)
{
}

unsigned int CFramebuffer::GetWidth () const 
{
    return m_BufferWidth;
}

unsigned int CFramebuffer::GetHeight () const 
{
    return m_BufferHeight;
}

unsigned int CFramebuffer::GetTextureCount () const
{
    return m_RgbTextureCount + m_RgbaTextureCount;
}

unsigned int CFramebuffer::GetRGBTextureCount () const
{
    return m_RgbTextureCount;
}

unsigned int CFramebuffer::GetRGBATextureCount () const
{
    return m_RgbaTextureCount;
}

void CFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    m_BufferWidth = bufferWidth;
    m_BufferHeight = bufferHeight;
}