#include "graphics/Framebuffer.h"
#include "system/LogMessage.h"

namespace cilantro {

CFramebuffer::CFramebuffer (unsigned int bufferWidth, unsigned int bufferHeight, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled)
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

unsigned int CFramebuffer::GetWidth () const
{
    return m_bufferWidth;
}

unsigned int CFramebuffer::GetHeight () const
{
    return m_bufferHeight;
}

unsigned int CFramebuffer::GetColorTextureCount () const
{
    return m_rgbTextureCount + m_rgbaTextureCount;
}

unsigned int CFramebuffer::GetRGBTextureCount () const
{
    return m_rgbTextureCount;
}

unsigned int CFramebuffer::GetRGBATextureCount () const
{
    return m_rgbaTextureCount;
}

unsigned int CFramebuffer::GetDepthArrayLayerCount () const
{
    return m_depthBufferArrayLayerCount;
}

bool CFramebuffer::IsDepthStencilRenderbufferEnabled () const
{
    return m_depthStencilRenderbufferEnabled;
}

bool CFramebuffer::IsDepthArrayEnabled () const
{
    return (m_depthBufferArrayLayerCount > 0);
}

void CFramebuffer::SetFramebufferResolution (unsigned int bufferWidth, unsigned int bufferHeight)
{
    m_bufferWidth = bufferWidth;
    m_bufferHeight = bufferHeight;
}

} // namespace cilantro