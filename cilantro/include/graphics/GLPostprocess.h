#ifndef _GLPOSTPROCESS_H_
#define _GLPOSTPROCESS_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Postprocess.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFramebuffer.h"
#include <string>

class GLPostprocess : public Postprocess, public GLFramebuffer
{
public:
    GLPostprocess () = delete;
    __EAPI GLPostprocess (GLRenderer* renderer, GLShaderProgram* shaderProgram);
    __EAPI virtual ~GLPostprocess ();

    virtual void OnFrame ();

    __EAPI virtual void SetPostprocessParameterFloat (std::string parameterName, float parameterValue);

private:
    GLuint VAO;
    GLuint VBO;

    virtual void Initialize ();
};

#endif