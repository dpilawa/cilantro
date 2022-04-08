#include "graphics/Framebuffer.h"
#include "system/LogMessage.h"

CFramebuffer::CFramebuffer (uint32_t bufferWidth, uint32_t bufferHeight, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayLayerCount, bool depthStencilRenderbufferEnabled)
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

uint32_t CFramebuffer::GetWidth () const 
{
    return m_bufferWidth;
}

uint32_t CFramebuffer::GetHeight () const 
{
    return m_bufferHeight;
}

size_t CFramebuffer::GetColorTextureCount () const
{
    return m_rgbTextureCount + m_rgbaTextureCount;
}

size_t CFramebuffer::GetRGBTextureCount () const
{
    return m_rgbTextureCount;
}

size_t CFramebuffer::GetRGBATextureCount () const
{
    return m_rgbaTextureCount;
}

size_t CFramebuffer::GetDepthArrayLayerCount () const
{
    return m_depthBufferArrayLayerCount;
}

bool CFramebuffer::IsDepthStencilRenderbufferEnabled () const
{
    return m_depthStencilRenderbufferEnabled;
}

void CFramebuffer::SetFramebufferResolution (uint32_t bufferWidth, uint32_t bufferHeight)
{
    m_bufferWidth = bufferWidth;
    m_bufferHeight = bufferHeight;
}