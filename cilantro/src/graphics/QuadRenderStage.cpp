#include "graphics/QuadRenderStage.h"
#include "system/EngineContext.h"
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
    shaderProgram = &(EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}