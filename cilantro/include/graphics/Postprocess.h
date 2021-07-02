#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include <string>

class Vector2f;
class Vector3f;
class Vector4f;

class Postprocess : public Resource
{
public:
    __EAPI Postprocess ();
    __EAPI virtual ~Postprocess ();

    __EAPI Postprocess& SetShaderProgram (const std::string& shaderProgramName);

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    Framebuffer* GetFramebuffer () const;
    virtual void SetPostprocessParameterFloat (const std::string& parameterName, float parameterValue) = 0;
    virtual void SetPostprocessParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue) = 0;
    virtual void SetPostprocessParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue) = 0;
    virtual void SetPostprocessParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue) = 0;

protected:
    Framebuffer* framebuffer;
    ShaderProgram* shaderProgram;
};

#endif