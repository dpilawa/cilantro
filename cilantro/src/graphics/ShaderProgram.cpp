#include "graphics/ShaderProgram.h"
#include "graphics/Shader.h"
#include "system/Game.h"

ShaderProgram::ShaderProgram () : Resource ()
{

}

ShaderProgram::~ShaderProgram ()
{
    
}

ShaderProgram& ShaderProgram::AddShader (const std::string& shaderName)
{
    AttachShader (Game::GetResourceManager ().GetByName<Shader> (shaderName));

    return *this;
}
