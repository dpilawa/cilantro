#include "graphics/RenderStage.h"
#include "system/EngineContext.h"
#include "graphics/ShaderProgram.h"

RenderStage::RenderStage ()
{
    framebuffer = nullptr;
    shaderProgram = nullptr;

    multisampleFramebufferEnabled = false;
    stencilTestEnabled = false;
    depthTestEnabled = false;
    clearOnFrameEnabled = true;

    stencilTestFunction = StencilTestFunction::FUNCTION_ALWAYS;
    stencilTestValue = 0;

    pipelineFramebufferInputLink = PipelineLink::LINK_PREVIOUS;
    pipelineRenderbufferLink = PipelineLink::LINK_CURRENT;
    pipelineFramebufferOutputLink = PipelineLink::LINK_CURRENT;
}

RenderStage::~RenderStage ()
{
}

RenderStage& RenderStage::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}

RenderStage& RenderStage::SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue)
{
    this->stencilTestFunction = stencilTestFunction;
    this->stencilTestValue = stencilTestValue;

    return *this;
}

RenderStage& RenderStage::SetMultisampleFramebufferEnabled (bool value)
{
    multisampleFramebufferEnabled = value;

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

RenderStage& RenderStage::SetPipelineFramebufferOutputLink (PipelineLink link)
{
    pipelineFramebufferOutputLink = link;

    return *this;
}

Framebuffer* RenderStage::GetFramebuffer () const
{
    return framebuffer;
}
