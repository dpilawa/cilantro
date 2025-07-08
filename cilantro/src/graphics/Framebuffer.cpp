#include "graphics/Framebuffer.h"
#include "system/LogMessage.h"

namespace cilantro {

Framebuffer::Framebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled)
    : m_bufferWidth (bufferWidth)
    , m_bufferHeight (bufferHeight)
    , m_rgbTextureCount (rgbTextureCount)
    , m_rgbaTextureCount (rgbaTextureCount)
    , m_depthBufferArrayLayerCount (depthBufferArrayLayerCount)
    , m_depthStencilRenderbufferEnabled (depthStencilRenderbufferEnabled)
{
    if (depthBufferArrayLayerCount > 0 && depthStencilRenderbufferEnabled)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Framebuffer should have either depth texture or renderbuffer";
    }
}

unsigned int Framebuffer::GetWidth () const
{
    return m_bufferWidth;
}

unsigned int Framebuffer::GetHeight () const
{
    return m_bufferHeight;
}

unsigned int Framebuffer::GetColorTextureCount () const
{
    return m_rgbTextureCount + m_rgbaTextureCount;
}

unsigned int Framebuffer::GetRGBTextureCount () const
{
    return m_rgbTextureCount;
}

unsigned int Framebuffer::GetRGBATextureCount () const
{
    return m_rgbaTextureCount;
}

unsigned int Framebuffer::GetDepthArrayLayerCount () const
{
    return m_depthBufferArrayLayerCount;
}

bool Framebuffer::IsDepthStencilRenderbufferEnabled () const
{
    return m_depthStencilRenderbufferEnabled;
}

bool Framebuffer::IsDepthTextureArrayEnabled () const
{
    return (m_depthBufferArrayLayerCount > 0);
}

void Framebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    m_bufferWidth = bufferWidth;
    m_bufferHeight = bufferHeight;
}

} // namespace cilantro