#include "graphics/ShaderProgram.h"
#include "graphics/Shader.h"
#include "system/Game.h"

ShaderProgram::ShaderProgram () : CResource ()
{

}

ShaderProgram::~ShaderProgram ()
{
    
}

ShaderProgram& ShaderProgram::AddShader (const std::string& shaderName)
{
    AttachShader (CGame::GetResourceManager ().GetByName<Shader> (shaderName));

    return *this;
}
