#include "graphics/Postprocess.h"
#include "system/EngineContext.h"
#include "graphics/ShaderProgram.h"

Postprocess::Postprocess ()
{
    stencilTestEnabled = false;
    clearOnFrameEnabled = true;

    stencilTestFunction = StencilTestFunction::FUNCTION_ALWAYS;
    stencilTestValue = 0;
}

Postprocess::~Postprocess ()
{
}

Postprocess& Postprocess::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}

Postprocess& Postprocess::SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue)
{
    this->stencilTestFunction = stencilTestFunction;
    this->stencilTestValue = stencilTestValue;

    return *this;
}

Postprocess& Postprocess::SetStencilTestEnabled (bool value)
{
    stencilTestEnabled = value;

    return *this;
}

Postprocess& Postprocess::SetClearOnFrameEnabled (bool value)
{
    clearOnFrameEnabled = value;

    return *this;
}

Framebuffer* Postprocess::GetFramebuffer () const
{
    return framebuffer;
}
