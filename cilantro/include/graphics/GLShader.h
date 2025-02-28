#pragma once

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/Shader.h"
#include <string>

class CGLShader : public CShader
{
public:
    CGLShader (const std::string& path, EShaderType type);
    ~CGLShader () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Compile () override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint GetShaderId () const;

private:

    // GL id of a shader
    GLuint m_glShaderId;

};

