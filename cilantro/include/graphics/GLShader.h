#pragma once

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/Shader.h"
#include <string>

namespace cilantro {

class __CEAPI GLShader : public Shader
{
public:
    GLShader (const std::string& path, EShaderType type);
    ~GLShader () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Compile () override;
    virtual void SetDefaults () override;

    ///////////////////////////////////////////////////////////////////////////

    GLuint GetShaderId () const;

private:

    // GL id of a shader
    GLuint m_glShaderId;

};

} // namespace cilantro

