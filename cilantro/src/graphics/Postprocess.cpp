#include "graphics/Postprocess.h"
#include "graphics/Renderer.h"
#include "graphics/ShaderProgram.h"

Postprocess::Postprocess ()
{
}

Postprocess::~Postprocess ()
{
}

Postprocess& Postprocess::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(renderer->GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}

Framebuffer* Postprocess::GetFramebuffer () const
{
    return framebuffer;
}
