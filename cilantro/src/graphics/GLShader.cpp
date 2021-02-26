#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/GLShader.h"
#include "system/LogMessage.h"

#include <string>

GLShader::GLShader (const std::string& path, ShaderType shaderType) : Shader (path, shaderType)
{
    GLint success;
    char errorLog[512];
    const char* src;

    switch (shaderType)
    {
        case ShaderType::VERTEX_SHADER:
            shaderId = glCreateShader (GL_VERTEX_SHADER);
            break;
        case ShaderType::FRAGMENT_SHADER:
            shaderId = glCreateShader (GL_FRAGMENT_SHADER);
            break;
    }

    src = shaderSourceCode.c_str ();
    glShaderSource (shaderId, 1, &src, NULL);
    glCompileShader (shaderId);

    glGetShaderiv (shaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog (shaderId, 512, nullptr, errorLog);
        glDeleteShader (shaderId);
        LogMessage () << errorLog;
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to compile shader" << shaderId << path;
    }
}

GLShader::~GLShader ()
{
}

GLuint GLShader::GetShaderId () const
{
    return shaderId;
}
