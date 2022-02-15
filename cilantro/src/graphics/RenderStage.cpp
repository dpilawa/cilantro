#include "graphics/RenderStage.h"
#include "system/Game.h"
#include "graphics/ShaderProgram.h"

RenderStage::RenderStage ()
{
    framebuffer = nullptr;

    multisampleEnabled = false;
    stencilTestEnabled = false;
    depthTestEnabled = true;
    clearOnFrameEnabled = true;
    faceCullingEnabled = true;
    framebufferEnabled = true;

    stencilTestFunction = StencilTestFunction::FUNCTION_ALWAYS;
    stencilTestValue = 0;

    pipelineFramebufferInputLink = PipelineLink::LINK_CURRENT;
    pipelineRenderbufferLink = PipelineLink::LINK_CURRENT;
    pipelineFramebufferOutputLink = PipelineLink::LINK_CURRENT;
}

RenderStage::~RenderStage ()
{
    if (framebuffer != nullptr)
    {
        framebuffer->Deinitialize ();
        delete framebuffer;
    }
}

RenderStage& RenderStage::SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue)
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

RenderStage& RenderStage::SetClearOnFrameEnabled (bool value)
{
    clearOnFrameEnabled = value;

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

bool RenderStage::IsClearOnFrameEnabled () const
{
    return clearOnFrameEnabled;
}

bool RenderStage::IsFaceCullingEnabled () const
{
    return faceCullingEnabled;
}

bool RenderStage::IsFramebufferEnabled () const
{
    return framebufferEnabled;
}

RenderStage& RenderStage::SetPipelineFramebufferInputLink (PipelineLink link)
{
    pipelineFramebufferInputLink = link;

    return *this;
}

RenderStage& RenderStage::SetPipelineRenderbufferLink (PipelineLink link)
{
    pipelineRenderbufferLink = link;

    return *this;
}

RenderStage& RenderStage::SetPipelineFramebufferDrawLink (PipelineLink link)
{
    pipelineFramebufferOutputLink = link;

    return *this;
}

void RenderStage::OnFrame ()
{
    Framebuffer* inputFramebuffer = renderer->GetPipelineFramebuffer (pipelineFramebufferInputLink);
    Framebuffer* inputFramebufferRenderbuffer = renderer->GetPipelineFramebuffer (pipelineRenderbufferLink);
    Framebuffer* outputFramebuffer = renderer->GetPipelineFramebuffer (pipelineFramebufferOutputLink);
  
    // bind framebuffer to render to
    renderer->BindFramebuffer (outputFramebuffer);
    
    // bind depth and stencli buffers from previous/linked stage
    renderer->BindFramebufferRenderbuffer (inputFramebufferRenderbuffer);

    // optionally clear
    if (clearOnFrameEnabled)
    {
        renderer->ClearColorBuffer (Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
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
