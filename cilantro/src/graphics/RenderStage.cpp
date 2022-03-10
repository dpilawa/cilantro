#include "graphics/RenderStage.h"
#include "graphics/Renderer.h"
#include "graphics/Framebuffer.h"
#include "math/Vector4f.h"

RenderStage::RenderStage ()
{
    framebuffer = nullptr;

    multisampleEnabled = false;
    stencilTestEnabled = false;
    depthTestEnabled = true;
    faceCullingEnabled = true;
    framebufferEnabled = true;

    clearColorOnFrameEnabled = true;
    clearDepthOnFrameEnabled = true;
    clearStencilOnFrameEnabled = true;

    stencilTestFunction = EStencilTestFunction::FUNCTION_ALWAYS;
    stencilTestValue = 0;

    pipelineFramebufferInputLink = EPipelineLink::LINK_CURRENT;
    pipelineRenderbufferLink = EPipelineLink::LINK_CURRENT;
    pipelineFramebufferOutputLink = EPipelineLink::LINK_CURRENT;
}

RenderStage::~RenderStage ()
{
    if (framebuffer != nullptr)
    {
        framebuffer->Deinitialize ();
        delete framebuffer;
    }
}

RenderStage& RenderStage::SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue)
{
    this->stencilTestFunction = stencilTestFunction;
    this->stencilTestValue = stencilTestValue;

    return *this;
}

RenderStage& RenderStage::SetMultisampleEnabled (bool value)
{
    unsigned int rgbTextureCount;
    unsigned int rgbaTextureCount;

    if (value != multisampleEnabled)
    {
        // set flag
        multisampleEnabled = value;

        // destroy and recreate framebuffer if already existed
        if (framebuffer != nullptr)
        {
            rgbaTextureCount = framebuffer->GetRGBATextureCount ();
            rgbTextureCount = framebuffer->GetRGBTextureCount ();

            framebuffer->Deinitialize ();
            delete framebuffer;

            framebuffer = renderer->CreateFramebuffer (rgbTextureCount, rgbaTextureCount, multisampleEnabled);
            framebuffer->Initialize ();
        }
    }

    return *this;
}

RenderStage& RenderStage::SetStencilTestEnabled (bool value)
{
    stencilTestEnabled = value;

    return *this;
}

RenderStage& RenderStage::SetDepthTestEnabled (bool value)
{
    depthTestEnabled = value;

    return *this;
}

RenderStage& RenderStage::SetFaceCullingEnabled (bool value)
{
    faceCullingEnabled = value;

    return *this;
}

RenderStage& RenderStage::SetFramebufferEnabled (bool value)
{
    if (framebufferEnabled != value)
    {
        if (value == false) // disabling
        {
            if (framebuffer != nullptr)
            {
                framebuffer->Deinitialize ();
                
                delete framebuffer;
                framebuffer = nullptr;
            }
        }
        else // enabling 
        {
            InitializeFramebuffer ();
        }
    }    

    framebufferEnabled = value;
    return *this;
}

RenderStage& RenderStage::SetClearColorOnFrameEnabled (bool value)
{
    clearColorOnFrameEnabled = value;

    return *this;
}

RenderStage& RenderStage::SetClearDepthOnFrameEnabled (bool value)
{
    clearDepthOnFrameEnabled = value;

    return *this;
}

RenderStage& RenderStage::SetClearStencilOnFrameEnabled (bool value)
{
    clearStencilOnFrameEnabled = value;

    return *this;
}

bool RenderStage::IsMultisampleEnabled () const
{
    return multisampleEnabled;
}

bool RenderStage::IsStencilTestEnabled () const
{
    return stencilTestEnabled;
}

bool RenderStage::IsDepthTestEnabled () const
{
    return depthTestEnabled;
}

bool RenderStage::IsFaceCullingEnabled () const
{
    return faceCullingEnabled;
}

bool RenderStage::IsFramebufferEnabled () const
{
    return framebufferEnabled;
}

bool RenderStage::IsClearColorOnFrameEnabled () const
{
    return clearColorOnFrameEnabled;
}

bool RenderStage::IsClearDepthOnFrameEnabled () const
{
    return clearDepthOnFrameEnabled;
}

bool RenderStage::IsClearStencilOnFrameEnabled () const
{
    return clearStencilOnFrameEnabled;
}

RenderStage& RenderStage::SetPipelineFramebufferInputLink (EPipelineLink link)
{
    pipelineFramebufferInputLink = link;

    return *this;
}

RenderStage& RenderStage::SetPipelineRenderbufferLink (EPipelineLink link)
{
    pipelineRenderbufferLink = link;

    return *this;
}

RenderStage& RenderStage::SetPipelineFramebufferDrawLink (EPipelineLink link)
{
    pipelineFramebufferOutputLink = link;

    return *this;
}

void RenderStage::OnFrame ()
{
    Framebuffer* inputFramebuffer = renderer->GetPipelineFramebuffer (pipelineFramebufferInputLink);
    Framebuffer* inputRenderbuffer = renderer->GetPipelineFramebuffer (pipelineRenderbufferLink);
    Framebuffer* outputFramebuffer = renderer->GetPipelineFramebuffer (pipelineFramebufferOutputLink);
  
    // bind framebuffer to render to
    if (outputFramebuffer != nullptr)    
    {
        outputFramebuffer->BindFramebuffer ();
    }
    else
    {
        renderer->BindDefaultFramebuffer ();
    }
    
    // bind depth and stencli buffers from previous/linked stage
    if (inputRenderbuffer != nullptr)
    {
        inputRenderbuffer->BindFramebufferRenderbuffer ();
    }

    // optionally clear
    if (clearColorOnFrameEnabled)
    {
        renderer->ClearColorBuffer (Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
    }

    if (clearDepthOnFrameEnabled)
    {
        renderer->ClearDepthBuffer ();
    }

    if (clearStencilOnFrameEnabled)
    {
        renderer->ClearStencilBuffer ();
    }

    // optionally enable depth test
    renderer->SetDepthTestEnabled (depthTestEnabled);
    if (depthTestEnabled)
    {
        renderer->ClearDepthBuffer ();
    }

    // optionally enable face culling
    renderer->SetFaceCullingEnabled (faceCullingEnabled);

    // optionally enable multisampling
    renderer->SetMultisamplingEnabled (multisampleEnabled);

    // optionally enable stencil test
    renderer->SetStencilTestEnabled (stencilTestEnabled);
    if (stencilTestEnabled)
    {
        renderer->SetStencilTestFunction (stencilTestFunction, stencilTestValue);
    }

}

Framebuffer* RenderStage::GetFramebuffer () const
{
    return framebuffer;
}
