#include "graphics/RenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "math/Vector4f.h"

namespace cilantro {

RenderStage::RenderStage ()
    
    : m_isMultisampleEnabled (false)
    , m_isStencilTestEnabled (false)
    , m_isDepthTestEnabled (true)
    , m_isFaceCullingEnabled (true)
    , m_isFramebufferEnabled (true)

    , m_isClearColorOnFrameEnabled (true)
    , m_isClearDepthOnFrameEnabled (true)
    , m_isClearStencilOnFrameEnabled (true)

    , m_viewportU (0.0f)
    , m_viewportV (0.0f)
    , m_viewportSizeU (1.0f)
    , m_viewportSizeV (1.0f)

    , m_renderer (nullptr)
    , m_framebuffer (nullptr)

    , m_colorAttachmentsFramebufferLink (EPipelineLink::LINK_CURRENT)
    , m_depthStencilFramebufferLink (EPipelineLink::LINK_CURRENT)
    , m_depthArrayFramebufferLink (EPipelineLink::LINK_CURRENT)
    , m_drawFramebufferLink (EPipelineLink::LINK_CURRENT)

    , m_linkedColorAttachmentsFramebuffer (nullptr)
    , m_linkedDepthStencilFramebuffer (nullptr)
    , m_linkedDepthArrayFramebuffer (nullptr)
    , m_linkedDrawFramebuffer (nullptr)

    , m_stencilTestFunction (EStencilTestFunction::FUNCTION_ALWAYS)
    , m_stencilTestValue (0)

    , m_faceCullingFace (EFaceCullingFace::FACE_BACK)
    , m_faceCullingDirection (EFaceCullingDirection::DIR_CCW)
    
{
}

RenderStage::~RenderStage ()
{
    if (m_framebuffer != nullptr)
    {
        m_framebuffer->Deinitialize ();
        delete m_framebuffer;
    }
}

IRenderer* RenderStage::GetRenderer () const
{
    return m_renderer;
}

IFramebuffer* RenderStage::GetFramebuffer () const
{
    return m_framebuffer;
}

IFramebuffer* RenderStage::GetLinkedColorAttachmentsFramebuffer () const
{
    return m_renderer->GetPipelineFramebuffer (m_colorAttachmentsFramebufferLink);
}

IFramebuffer* RenderStage::GetLinkedDepthStencilFramebuffer () const
{
    return m_renderer->GetPipelineFramebuffer (m_depthStencilFramebufferLink);
}

IFramebuffer* RenderStage::GetLinkedDepthArrayFramebuffer () const
{
    return m_renderer->GetPipelineFramebuffer (m_depthArrayFramebufferLink);
}

IFramebuffer* RenderStage::GetLinkedDrawFramebuffer () const
{
    return m_renderer->GetPipelineFramebuffer (m_drawFramebufferLink);
}

void RenderStage::OnFrame ()
{
    size_t width;
    size_t height;

    // link pipeline framebuffers
    m_linkedColorAttachmentsFramebuffer = GetLinkedColorAttachmentsFramebuffer ();
    m_linkedDepthStencilFramebuffer = GetLinkedDepthStencilFramebuffer ();
    m_linkedDepthArrayFramebuffer = GetLinkedDepthArrayFramebuffer ();
    m_linkedDrawFramebuffer = GetLinkedDrawFramebuffer ();
    
    // bind framebuffer to render to
    if (m_linkedDrawFramebuffer != nullptr)    
    {
        m_linkedDrawFramebuffer->BindFramebuffer ();
    }
    else
    {
        m_renderer->BindDefaultFramebuffer ();
    }
    
    // bind depth and stencil buffers from previous/linked stage
    if (m_linkedDepthStencilFramebuffer != nullptr)
    {
        if (m_linkedDepthStencilFramebuffer->IsDepthStencilRenderbufferEnabled ()) 
        {
            m_linkedDepthStencilFramebuffer->BindFramebufferRenderbuffer ();
        } 
        else if (m_linkedDepthStencilFramebuffer->GetDepthArrayLayerCount () > 0)
        {
            m_linkedDepthStencilFramebuffer->BindFramebufferDepthArrayTextureAsDepth ();
        }
        else
        {
            m_renderer->BindDefaultDepthBuffer ();
            m_renderer->BindDefaultStencilBuffer ();
        }
    }

    // optionally clear
    if (m_isClearColorOnFrameEnabled)
    {
        m_renderer->ClearColorBuffer (Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
    }

    if (m_isClearDepthOnFrameEnabled)
    {
        m_renderer->ClearDepthBuffer ();
    }

    if (m_isClearStencilOnFrameEnabled)
    {
        m_renderer->ClearStencilBuffer ();
    }

    // optionally enable depth test
    m_renderer->SetDepthTestEnabled (m_isDepthTestEnabled);
    if (m_isDepthTestEnabled)
    {
        m_renderer->ClearDepthBuffer ();
    }

    // optionally enable face culling
    m_renderer->SetFaceCullingEnabled (m_isFaceCullingEnabled);
    if (m_isFaceCullingEnabled)
    {
        m_renderer->SetFaceCullingMode (m_faceCullingFace, m_faceCullingDirection);
    }

    // optionally enable multisampling
    m_renderer->SetMultisamplingEnabled (m_isMultisampleEnabled);

    // optionally enable stencil test
    m_renderer->SetStencilTestEnabled (m_isStencilTestEnabled);
    if (m_isStencilTestEnabled)
    {
        m_renderer->SetStencilTestFunction (m_stencilTestFunction, m_stencilTestValue);
    }

    // set viewport

    if (m_linkedDrawFramebuffer != nullptr)
    {
        width = m_linkedDrawFramebuffer->GetWidth ();
        height = m_linkedDrawFramebuffer->GetHeight ();
    }
    else
    {
        width = m_renderer->GetWidth ();
        height = m_renderer->GetHeight ();
    }

    m_renderer->SetViewport 
        ( (unsigned int) (m_viewportU * width)
        , (unsigned int) (m_viewportV * height)
        , (unsigned int) (m_viewportSizeU * width)
        , (unsigned int) (m_viewportSizeV * height));

}

IRenderStage& RenderStage::SetViewport (float u, float v, float su, float sv)
{
    m_viewportU = u;
    m_viewportV = v;
    m_viewportSizeU = su;
    m_viewportSizeV = sv;

    return *this;
}

IRenderStage& RenderStage::SetMultisampleEnabled (bool value)
{
    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;
    unsigned int dsArraySize;
    unsigned int width;
    unsigned int height;
    bool hasDSRenderbuffer;

    if (value != m_isMultisampleEnabled)
    {
        // set flag
        m_isMultisampleEnabled = value;

        // destroy and recreate framebuffer if already existed
        if (m_framebuffer != nullptr)
        {
            rgbaTextureCount = m_framebuffer->GetRGBATextureCount ();
            rgbTextureCount = m_framebuffer->GetRGBTextureCount ();
            dsArraySize = m_framebuffer->GetDepthArrayLayerCount ();
            hasDSRenderbuffer = m_framebuffer->IsDepthStencilRenderbufferEnabled ();

            width = m_framebuffer->GetWidth ();
            height = m_framebuffer->GetHeight ();

            m_framebuffer->Deinitialize ();
            delete m_framebuffer;

            m_framebuffer = m_renderer->CreateFramebuffer (width, height, rgbTextureCount, rgbaTextureCount, dsArraySize, hasDSRenderbuffer, m_isMultisampleEnabled);
            m_framebuffer->Initialize ();
        }
    }

    return *this;
}

IRenderStage& RenderStage::SetStencilTestEnabled (bool value)
{
    m_isStencilTestEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetDepthTestEnabled (bool value)
{
    m_isDepthTestEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetFaceCullingEnabled (bool value)
{
    m_isFaceCullingEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetFramebufferEnabled (bool value)
{
    if (m_isFramebufferEnabled != value)
    {
        if (value == false) // disabling
        {
            if (m_framebuffer != nullptr)
            {
                m_framebuffer->Deinitialize ();
                
                delete m_framebuffer;
                m_framebuffer = nullptr;
            }
        }
        else // enabling 
        {
            InitializeFramebuffer ();
        }
    }    

    m_isFramebufferEnabled = value;
    return *this;
}

IRenderStage& RenderStage::SetClearColorOnFrameEnabled (bool value)
{
    m_isClearColorOnFrameEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetClearDepthOnFrameEnabled (bool value)
{
    m_isClearDepthOnFrameEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetClearStencilOnFrameEnabled (bool value)
{
    m_isClearStencilOnFrameEnabled = value;

    return *this;
}

IRenderStage& RenderStage::SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue)
{
    m_stencilTestFunction = stencilTestFunction;
    m_stencilTestValue = stencilTestValue;

    return *this;
}

IRenderStage& RenderStage::SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection)
{
    m_faceCullingFace = faceCullingFace;
    m_faceCullingDirection = faceCullingDirection;

    return *this;
}

bool RenderStage::IsMultisampleEnabled () const
{
    return m_isMultisampleEnabled;
}

bool RenderStage::IsStencilTestEnabled () const
{
    return m_isStencilTestEnabled;
}

bool RenderStage::IsDepthTestEnabled () const
{
    return m_isDepthTestEnabled;
}

bool RenderStage::IsFaceCullingEnabled () const
{
    return m_isFaceCullingEnabled;
}

bool RenderStage::IsFramebufferEnabled () const
{
    return m_isFramebufferEnabled;
}

bool RenderStage::IsClearColorOnFrameEnabled () const
{
    return m_isClearColorOnFrameEnabled;
}

bool RenderStage::IsClearDepthOnFrameEnabled () const
{
    return m_isClearDepthOnFrameEnabled;
}

bool RenderStage::IsClearStencilOnFrameEnabled () const
{
    return m_isClearStencilOnFrameEnabled;
}

IRenderStage& RenderStage::SetColorAttachmentsFramebufferLink (EPipelineLink link)
{
    m_colorAttachmentsFramebufferLink = link;

    return *this;
}

IRenderStage& RenderStage::SetDepthStencilFramebufferLink (EPipelineLink link)
{
    m_depthStencilFramebufferLink = link;

    return *this;
}

IRenderStage& RenderStage::SetDepthArrayFramebufferLink (EPipelineLink link)
{
    m_depthArrayFramebufferLink = link;

    return *this;
}

IRenderStage& RenderStage::SetOutputFramebufferLink (EPipelineLink link)
{
    m_drawFramebufferLink = link;

    return *this;
}

} // namespace cilantro
