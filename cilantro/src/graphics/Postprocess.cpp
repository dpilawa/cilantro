#include "graphics/Postprocess.h"
#include "system/EngineContext.h"
#include "graphics/ShaderProgram.h"

Postprocess::Postprocess ()
{
}

Postprocess::~Postprocess ()
{
}

Postprocess& Postprocess::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}

Framebuffer* Postprocess::GetFramebuffer () const
{
    return framebuffer;
}
