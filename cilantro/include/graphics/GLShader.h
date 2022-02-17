#ifndef _GLSHADER_H_
#define _GLSHADER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Shader.h"
#include <string>

class GLShader : public Shader
{
public:
    GLShader (const std::string& path, ShaderType type);
    ~GLShader ();

    // return shader ID
    GLuint GetShaderId () const;

private:

    // ID of a shader
    GLuint shaderId;

    virtual void Compile();
};

#endif
