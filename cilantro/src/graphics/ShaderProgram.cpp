#include "graphics/ShaderProgram.h"
#include "system/EngineContext.h"

ShaderProgram::ShaderProgram () : Resource ()
{

}

ShaderProgram::~ShaderProgram ()
{
    
}

ShaderProgram& ShaderProgram::AddShader (const std::string& shaderName)
{
    AttachShader (EngineContext::GetResourceManager ().GetByName<Shader> (shaderName));

    return *this;
}
