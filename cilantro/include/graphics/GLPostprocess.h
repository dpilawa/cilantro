#ifndef _GLPOSTPROCESS_H_
#define _GLPOSTPROCESS_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Postprocess.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFramebuffer.h"
#include <string>

class Vector2f;
class Vector3f;
class Vector4f;

class GLPostprocess : public Postprocess
{
public:
    __EAPI GLPostprocess ();
    __EAPI virtual ~GLPostprocess ();

    __EAPI virtual Postprocess& SetMultisampleFramebufferEnabled (bool value);

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void OnFrame ();

    __EAPI virtual Postprocess& SetPostprocessParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual Postprocess& SetPostprocessParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual Postprocess& SetPostprocessParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual Postprocess& SetPostprocessParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

private:

    GLuint GetUniformLocation (const std::string& parameterName);

    GLuint VAO;
    GLuint VBO;

};

#endif