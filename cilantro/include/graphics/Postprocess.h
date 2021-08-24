#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include <string>

enum StencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };

class Vector2f;
class Vector3f;
class Vector4f;

class Postprocess : public Resource
{
public:
    __EAPI Postprocess ();
    __EAPI virtual ~Postprocess ();

    __EAPI Postprocess& SetShaderProgram (const std::string& shaderProgramName);
    __EAPI Postprocess& SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue);

    __EAPI Postprocess& SetStencilTestEnabled (bool value);
    __EAPI Postprocess& SetClearOnFrameEnabled (bool value);

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    Framebuffer* GetFramebuffer () const;
    virtual Postprocess& SetPostprocessParameterFloat (const std::string& parameterName, float parameterValue) = 0;
    virtual Postprocess& SetPostprocessParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue) = 0;
    virtual Postprocess& SetPostprocessParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue) = 0;
    virtual Postprocess& SetPostprocessParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue) = 0;

protected:

    // flags
    bool stencilTestEnabled;
    bool clearOnFrameEnabled;

    // output framebuffer
    Framebuffer* framebuffer;

    // postprocess shader
    ShaderProgram* shaderProgram;

    // stencil testing parameters
    StencilTestFunction stencilTestFunction;
    int stencilTestValue;
};

#endif