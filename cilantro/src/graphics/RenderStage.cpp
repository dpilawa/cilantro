#include "graphics/RenderStage.h"
#include "system/EngineContext.h"
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
    multisampleEnabled = value;

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

Framebuffer* RenderStage::GetFramebuffer () const
{
    return framebuffer;
}
