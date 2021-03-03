#include "graphics/ShaderProgram.h"
#include "graphics/Renderer.h"

ShaderProgram::ShaderProgram () : Resource ()
{

}

ShaderProgram::~ShaderProgram ()
{
    
}

ShaderProgram& ShaderProgram::AddShader (const std::string& shaderName)
{
    LinkShader (renderer->GetGameResourceManager ().GetByName<Shader> (shaderName));

    return *this;
}
