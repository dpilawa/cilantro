#ifndef _RENDERSTAGE_H_
#define _RENDERSTAGE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include <string>

enum PipelineLink {LINK_BASE, LINK_FIRST, LINK_PREVIOUS, LINK_CURRENT, LINK_LAST};
enum StencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };

class Vector2f;
class Vector3f;
class Vector4f;

class RenderStage : public Resource
{
public:
    __EAPI RenderStage ();
    __EAPI virtual ~RenderStage ();

    __EAPI RenderStage& SetShaderProgram (const std::string& shaderProgramName);
    __EAPI RenderStage& SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue);

    __EAPI virtual RenderStage& SetMultisampleFramebufferEnabled (bool value);
    __EAPI virtual RenderStage& SetStencilTestEnabled (bool value);
    __EAPI virtual RenderStage& SetDepthTestEnabled (bool value);
    __EAPI virtual RenderStage& SetClearOnFrameEnabled (bool value);

    __EAPI RenderStage& SetPipelineFramebufferInputLink (PipelineLink link);
    __EAPI RenderStage& SetPipelineRenderbufferLink (PipelineLink link);
    __EAPI RenderStage& SetPipelineFramebufferOutputLink (PipelineLink link);

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    Framebuffer* GetFramebuffer () const;
    virtual RenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue) = 0;
    virtual RenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue) = 0;
    virtual RenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue) = 0;
    virtual RenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue) = 0;

protected:

    // flags
    bool multisampleFramebufferEnabled;
    bool stencilTestEnabled;
    bool depthTestEnabled;
    bool clearOnFrameEnabled;

    // output framebuffer
    Framebuffer* framebuffer;

    // these indicate which framebuffer and which render buffer should be current stage's input
    // and where to write to
    PipelineLink pipelineFramebufferInputLink;
    PipelineLink pipelineRenderbufferLink;
    PipelineLink pipelineFramebufferOutputLink;

    // stage shader
    ShaderProgram* shaderProgram;

    // stencil testing parameters
    StencilTestFunction stencilTestFunction;
    int stencilTestValue;
};

#endif