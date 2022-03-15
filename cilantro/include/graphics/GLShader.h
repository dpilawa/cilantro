#pragma once

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Shader.h"
#include <string>

class CGLShader : public CShader
{
public:
    CGLShader (const std::string& path, EShaderType type);
    ~CGLShader ();

    // return shader ID
    GLuint GetShaderId () const;

private:
    virtual void Compile();

private:
    // GL id of a shader
    GLuint m_glShaderId;

};

