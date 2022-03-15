#include "graphics/ShaderProgram.h"
#include "graphics/Shader.h"
#include "system/Game.h"

CShaderProgram::CShaderProgram () : CResource ()
{

}

CShaderProgram::~CShaderProgram ()
{
    
}

CShaderProgram& CShaderProgram::AddShader (const std::string& shaderName)
{
    AttachShader (CGame::GetResourceManager ().GetByName<CShader> (shaderName));

    return *this;
}
