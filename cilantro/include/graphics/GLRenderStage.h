#ifndef _GLRENDERSTAGE_H_
#define _GLRENDERSTAGE_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/RenderStage.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFramebuffer.h"
#include <string>

class Vector2f;
class Vector3f;
class Vector4f;

class GLRenderStage : public RenderStage
{
public:
    __EAPI GLRenderStage ();
    __EAPI virtual ~GLRenderStage ();

    __EAPI virtual RenderStage& SetMultisampleFramebufferEnabled (bool value);

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void OnFrame ();

    __EAPI virtual RenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual RenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual RenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual RenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

private:

    GLuint GetUniformLocation (const std::string& parameterName);

    GLuint VAO;
    GLuint VBO;

};

#endif