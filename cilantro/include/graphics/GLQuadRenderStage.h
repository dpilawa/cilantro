#ifndef _GLQUADRENDERSTAGE_H_
#define _GLQUADRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/GLRenderStage.h"
#include "graphics/QuadRenderStage.h"

class Vector2f;
class Vector3f;
class Vector4f;

class GLQuadRenderStage : public GLRenderStage, public QuadRenderStage
{
public:
    __EAPI GLQuadRenderStage ();
    __EAPI virtual ~GLQuadRenderStage ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void OnFrame ();

    __EAPI virtual QuadRenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

protected:

    virtual void InitializeFramebuffer ();

private:

    GLuint GetUniformLocation (const std::string& parameterName);

    GLuint VAO;
    GLuint VBO;

};

#endif