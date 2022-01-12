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
    shaderProgram = &(Game::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}