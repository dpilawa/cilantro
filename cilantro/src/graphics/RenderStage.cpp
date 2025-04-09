#include "graphics/RenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "math/Vector4f.h"

namespace cilantro {

RenderStage::RenderStage (std::shared_ptr<IRenderer> renderer)
    
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

    , m_renderer (renderer)
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
    }
}

std::shared_ptr<IRenderer> RenderStage::GetRenderer () const
{
    return m_renderer.lock ();
}

std::shared_ptr<IFramebuffer> RenderStage::GetFramebuffer () const
{
    return m_framebuffer;
}

std::shared_ptr<IFramebuffer> RenderStage::GetLinkedColorAttachmentsFramebuffer () const
{
    return GetRenderer ()->GetPipelineFramebuffer (m_colorAttachmentsFramebufferLink);
}

std::shared_ptr<IFramebuffer> RenderStage::GetLinkedDepthStencilFramebuffer () const
{
    return GetRenderer ()->GetPipelineFramebuffer (m_depthStencilFramebufferLink);
}

std::shared_ptr<IFramebuffer> RenderStage::GetLinkedDepthArrayFramebuffer () const
{
    return GetRenderer ()->GetPipelineFramebuffer (m_depthArrayFramebufferLink);
}

std::shared_ptr<IFramebuffer> RenderStage::GetLinkedDrawFramebuffer () const
{
    return GetRenderer ()->GetPipelineFramebuffer (m_drawFramebufferLink);
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
        GetRenderer ()->BindDefaultFramebuffer ();
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
            GetRenderer ()->BindDefaultDepthBuffer ();
            GetRenderer ()->BindDefaultStencilBuffer ();
        }
    }

    // optionally clear
    if (m_isClearColorOnFrameEnabled)
    {
        GetRenderer ()->ClearColorBuffer (Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
    }

    if (m_isClearDepthOnFrameEnabled)
    {
        GetRenderer ()->ClearDepthBuffer ();
    }

    if (m_isClearStencilOnFrameEnabled)
    {
        GetRenderer ()->ClearStencilBuffer ();
    }

    // optionally enable depth test
    GetRenderer ()->SetDepthTestEnabled (m_isDepthTestEnabled);
    if (m_isDepthTestEnabled)
    {
        GetRenderer ()->ClearDepthBuffer ();
    }

    // optionally enable face culling
    GetRenderer ()->SetFaceCullingEnabled (m_isFaceCullingEnabled);
    if (m_isFaceCullingEnabled)
    {
        GetRenderer ()->SetFaceCullingMode (m_faceCullingFace, m_faceCullingDirection);
    }

    // optionally enable multisampling
    GetRenderer ()->SetMultisamplingEnabled (m_isMultisampleEnabled);

    // optionally enable stencil test
    GetRenderer ()->SetStencilTestEnabled (m_isStencilTestEnabled);
    if (m_isStencilTestEnabled)
    {
        GetRenderer ()->SetStencilTestFunction (m_stencilTestFunction, m_stencilTestValue);
    }

    // set viewport

    if (m_linkedDrawFramebuffer != nullptr)
    {
        width = m_linkedDrawFramebuffer->GetWidth ();
        height = m_linkedDrawFramebuffer->GetHeight ();
    }
    else
    {
        width = GetRenderer ()->GetWidth ();
        height = GetRenderer ()->GetHeight ();
    }

    GetRenderer ()->SetViewport 
        ( (unsigned int) (m_viewportU * width)
        , (unsigned int) (m_viewportV * height)
        , (unsigned int) (m_viewportSizeU * width)
        , (unsigned int) (m_viewportSizeV * height));

}

std::shared_ptr<IRenderStage> RenderStage::SetViewport (float u, float v, float su, float sv)
{
    m_viewportU = u;
    m_viewportV = v;
    m_viewportSizeU = su;
    m_viewportSizeV = sv;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetMultisampleEnabled (bool value)
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
            m_framebuffer = GetRenderer ()->CreateFramebuffer (width, height, rgbTextureCount, rgbaTextureCount, dsArraySize, hasDSRenderbuffer, m_isMultisampleEnabled);
            m_framebuffer->Initialize ();
        }
    }

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetStencilTestEnabled (bool value)
{
    m_isStencilTestEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetDepthTestEnabled (bool value)
{
    m_isDepthTestEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetFaceCullingEnabled (bool value)
{
    m_isFaceCullingEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetFramebufferEnabled (bool value)
{
    if (m_isFramebufferEnabled != value)
    {
        if (value == false) // disabling
        {
            if (m_framebuffer != nullptr)
            {
                m_framebuffer->Deinitialize ();              
                m_framebuffer = nullptr;
            }
        }
        else // enabling 
        {
            InitializeFramebuffer ();
        }
    }    

    m_isFramebufferEnabled = value;
    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetClearColorOnFrameEnabled (bool value)
{
    m_isClearColorOnFrameEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetClearDepthOnFrameEnabled (bool value)
{
    m_isClearDepthOnFrameEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetClearStencilOnFrameEnabled (bool value)
{
    m_isClearStencilOnFrameEnabled = value;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue)
{
    m_stencilTestFunction = stencilTestFunction;
    m_stencilTestValue = stencilTestValue;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection)
{
    m_faceCullingFace = faceCullingFace;
    m_faceCullingDirection = faceCullingDirection;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
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

std::shared_ptr<IRenderStage> RenderStage::SetColorAttachmentsFramebufferLink (EPipelineLink link)
{
    m_colorAttachmentsFramebufferLink = link;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetDepthStencilFramebufferLink (EPipelineLink link)
{
    m_depthStencilFramebufferLink = link;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetDepthArrayFramebufferLink (EPipelineLink link)
{
    m_depthArrayFramebufferLink = link;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

std::shared_ptr<IRenderStage> RenderStage::SetOutputFramebufferLink (EPipelineLink link)
{
    m_drawFramebufferLink = link;

    return std::dynamic_pointer_cast<IRenderStage> (shared_from_this ());
}

} // namespace cilantro
