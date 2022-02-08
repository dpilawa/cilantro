#ifndef _RENDERSTAGE_H_
#define _RENDERSTAGE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Framebuffer.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include <string>

enum PipelineLink { LINK_FIRST, LINK_SECOND, LINK_PREVIOUS, LINK_CURRENT, LINK_LAST };
enum StencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };

class Vector2f;
class Vector3f;
class Vector4f;

class RenderStage : public Resource
{
    friend class Renderer;
public:
    __EAPI RenderStage ();
    __EAPI virtual ~RenderStage ();

    __EAPI RenderStage& SetStencilTest (StencilTestFunction stencilTestFunction, int stencilTestValue);

    __EAPI virtual RenderStage& SetMultisampleEnabled (bool value);
    __EAPI virtual RenderStage& SetStencilTestEnabled (bool value);
    __EAPI virtual RenderStage& SetDepthTestEnabled (bool value);
    __EAPI virtual RenderStage& SetClearOnFrameEnabled (bool value);
    __EAPI virtual RenderStage& SetFaceCullingEnabled (bool value);
    __EAPI virtual RenderStage& SetFramebufferEnabled (bool value);

    __EAPI bool IsMultisampleEnabled () const;
    __EAPI bool IsStencilTestEnabled () const;
    __EAPI bool IsDepthTestEnabled () const;
    __EAPI bool IsClearOnFrameEnabled () const;
    __EAPI bool IsFaceCullingEnabled () const;
    __EAPI bool IsFramebufferEnabled () const;

    __EAPI virtual RenderStage& SetPipelineFramebufferInputLink (PipelineLink link);
    __EAPI virtual RenderStage& SetPipelineRenderbufferLink (PipelineLink link);
    __EAPI virtual RenderStage& SetPipelineFramebufferDrawLink (PipelineLink link);

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    Framebuffer* GetFramebuffer () const;

protected:

    virtual void InitializeFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures) = 0;
    virtual void InitializeFramebuffer () = 0;

    // flags
    bool multisampleEnabled;
    bool stencilTestEnabled;
    bool depthTestEnabled;
    bool clearOnFrameEnabled;
    bool faceCullingEnabled;
    bool framebufferEnabled;

    // parent renderer
    Renderer* renderer;

    // output framebuffer
    Framebuffer* framebuffer;

    // these indicate which framebuffer and which render buffer should be current stage's input
    // and where to write to
    PipelineLink pipelineFramebufferInputLink;
    PipelineLink pipelineRenderbufferLink;
    PipelineLink pipelineFramebufferOutputLink;

    // stencil testing parameters
    StencilTestFunction stencilTestFunction;
    int stencilTestValue;
};

#endif