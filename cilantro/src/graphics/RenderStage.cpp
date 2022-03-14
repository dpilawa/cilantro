#include "graphics/RenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "math/Vector4f.h"

CRenderStage::CRenderStage ()
    : m_framebuffer (nullptr)
    , m_isMultisampleEnabled (false)
    , m_isStencilTestEnabled (false)
    , m_isDepthTestEnabled (true)
    , m_isFaceCullingEnabled (true)
    , m_isFramebufferEnabled (true)

    , m_isClearColorOnFrameEnabled (true)
    , m_isClearDepthOnFrameEnabled (true)
    , m_isClearStencilOnFrameEnabled (true)

    , m_stencilTestFunction (EStencilTestFunction::FUNCTION_ALWAYS)
    , m_stencilTestValue (0)

    , m_pipelineFramebufferInputLink (EPipelineLink::LINK_CURRENT)
    , m_pipelineRenderbufferLink (EPipelineLink::LINK_CURRENT)
    , m_pipelineFramebufferOutputLink (EPipelineLink::LINK_CURRENT)
{
}

CRenderStage::~CRenderStage ()
{
    if (m_framebuffer != nullptr)
    {
        m_framebuffer->Deinitialize ();
        delete m_framebuffer;
    }
}

IFramebuffer* CRenderStage::GetFramebuffer () const
{
    return m_framebuffer;
}

void CRenderStage::OnFrame ()
{
    IFramebuffer* inputFramebuffer = m_renderer->GetPipelineFramebuffer (m_pipelineFramebufferInputLink);
    IFramebuffer* inputRenderbuffer = m_renderer->GetPipelineFramebuffer (m_pipelineRenderbufferLink);
    IFramebuffer* outputFramebuffer = m_renderer->GetPipelineFramebuffer (m_pipelineFramebufferOutputLink);
  
    // bind framebuffer to render to
    if (outputFramebuffer != nullptr)    
    {
        outputFramebuffer->BindFramebuffer ();
    }
    else
    {
        m_renderer->BindDefaultFramebuffer ();
    }
    
    // bind depth and stencli buffers from previous/linked stage
    if (inputRenderbuffer != nullptr)
    {
        inputRenderbuffer->BindFramebufferRenderbuffer ();
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

    // optionally enable multisampling
    m_renderer->SetMultisamplingEnabled (m_isMultisampleEnabled);

    // optionally enable stencil test
    m_renderer->SetStencilTestEnabled (m_isStencilTestEnabled);
    if (m_isStencilTestEnabled)
    {
        m_renderer->SetStencilTestFunction (m_stencilTestFunction, m_stencilTestValue);
    }

}

IRenderStage& CRenderStage::SetMultisampleEnabled (bool value)
{
    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;

    if (value != m_isMultisampleEnabled)
    {
        // set flag
        m_isMultisampleEnabled = value;

        // destroy and recreate framebuffer if already existed
        if (m_framebuffer != nullptr)
        {
            rgbaTextureCount = m_framebuffer->GetRGBATextureCount ();
            rgbTextureCount = m_framebuffer->GetRGBTextureCount ();

            m_framebuffer->Deinitialize ();
            delete m_framebuffer;

            m_framebuffer = m_renderer->CreateFramebuffer (rgbTextureCount, rgbaTextureCount, m_isMultisampleEnabled);
            m_framebuffer->Initialize ();
        }
    }

    return *this;
}

IRenderStage& CRenderStage::SetStencilTestEnabled (bool value)
{
    m_isStencilTestEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetDepthTestEnabled (bool value)
{
    m_isDepthTestEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetFaceCullingEnabled (bool value)
{
    m_isFaceCullingEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetFramebufferEnabled (bool value)
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

IRenderStage& CRenderStage::SetClearColorOnFrameEnabled (bool value)
{
    m_isClearColorOnFrameEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetClearDepthOnFrameEnabled (bool value)
{
    m_isClearDepthOnFrameEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetClearStencilOnFrameEnabled (bool value)
{
    m_isClearStencilOnFrameEnabled = value;

    return *this;
}

IRenderStage& CRenderStage::SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue)
{
    this->m_stencilTestFunction = stencilTestFunction;
    this->m_stencilTestValue = stencilTestValue;

    return *this;
}

bool CRenderStage::IsMultisampleEnabled () const
{
    return m_isMultisampleEnabled;
}

bool CRenderStage::IsStencilTestEnabled () const
{
    return m_isStencilTestEnabled;
}

bool CRenderStage::IsDepthTestEnabled () const
{
    return m_isDepthTestEnabled;
}

bool CRenderStage::IsFaceCullingEnabled () const
{
    return m_isFaceCullingEnabled;
}

bool CRenderStage::IsFramebufferEnabled () const
{
    return m_isFramebufferEnabled;
}

bool CRenderStage::IsClearColorOnFrameEnabled () const
{
    return m_isClearColorOnFrameEnabled;
}

bool CRenderStage::IsClearDepthOnFrameEnabled () const
{
    return m_isClearDepthOnFrameEnabled;
}

bool CRenderStage::IsClearStencilOnFrameEnabled () const
{
    return m_isClearStencilOnFrameEnabled;
}

IRenderStage& CRenderStage::SetPipelineFramebufferInputLink (EPipelineLink link)
{
    m_pipelineFramebufferInputLink = link;

    return *this;
}

IRenderStage& CRenderStage::SetPipelineRenderbufferLink (EPipelineLink link)
{
    m_pipelineRenderbufferLink = link;

    return *this;
}

IRenderStage& CRenderStage::SetPipelineFramebufferDrawLink (EPipelineLink link)
{
    m_pipelineFramebufferOutputLink = link;

    return *this;
}
