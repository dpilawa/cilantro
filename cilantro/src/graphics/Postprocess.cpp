#include "graphics/Postprocess.h"
#include "graphics/Renderer.h"

Postprocess::Postprocess (Renderer& renderer, ShaderProgram& shaderProgram) : renderer (renderer), shaderProgram (shaderProgram)
{
}

Postprocess::~Postprocess ()
{
}

Framebuffer* Postprocess::GetFramebufferPtr () const
{
    return framebuffer;
}
