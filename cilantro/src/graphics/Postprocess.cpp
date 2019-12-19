#include "graphics/Postprocess.h"

Postprocess::Postprocess (Renderer* renderer, ShaderProgram* shaderProgram)
{
    this->renderer = renderer;
    this->shaderProgram = shaderProgram;
}

Postprocess::~Postprocess ()
{
}

