#include "graphics/QuadRenderStage.h"
#include "system/Game.h"
#include "graphics/ShaderProgram.h"

QuadRenderStage::QuadRenderStage () : RenderStage ()
{
    shaderProgram = nullptr;
}


QuadRenderStage::~QuadRenderStage ()
{
}

QuadRenderStage& QuadRenderStage::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(renderer->GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}