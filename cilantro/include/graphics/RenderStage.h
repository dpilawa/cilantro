#pragma once

#include "cilantroengine.h"
#include "graphics/IRenderer.h"
#include "resource/Resource.h"
#include <string>

class Framebuffer;
class CRenderer;
class Vector2f;
class Vector3f;
class Vector4f;

class RenderStage : public Resource
{
    friend class CRenderer;
public:
    __EAPI RenderStage ();
    __EAPI virtual ~RenderStage ();

    __EAPI RenderStage& SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue);

    __EAPI virtual RenderStage& SetMultisampleEnabled (bool value);
    __EAPI virtual RenderStage& SetStencilTestEnabled (bool value);
    __EAPI virtual RenderStage& SetDepthTestEnabled (bool value);
    __EAPI virtual RenderStage& SetFaceCullingEnabled (bool value);
    __EAPI virtual RenderStage& SetFramebufferEnabled (bool value);

    __EAPI virtual RenderStage& SetClearColorOnFrameEnabled (bool value);
    __EAPI virtual RenderStage& SetClearDepthOnFrameEnabled (bool value);
    __EAPI virtual RenderStage& SetClearStencilOnFrameEnabled (bool value);

    __EAPI bool IsMultisampleEnabled () const;
    __EAPI bool IsStencilTestEnabled () const;
    __EAPI bool IsDepthTestEnabled () const;
    __EAPI bool IsFaceCullingEnabled () const;
    __EAPI bool IsFramebufferEnabled () const;

    __EAPI bool IsClearColorOnFrameEnabled () const;
    __EAPI bool IsClearDepthOnFrameEnabled () const;
    __EAPI bool IsClearStencilOnFrameEnabled () const;

    __EAPI virtual RenderStage& SetPipelineFramebufferInputLink (EPipelineLink link);
    __EAPI virtual RenderStage& SetPipelineRenderbufferLink (EPipelineLink link);
    __EAPI virtual RenderStage& SetPipelineFramebufferDrawLink (EPipelineLink link);

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame ();

    virtual void InitializeFramebuffer () = 0;
    Framebuffer* GetFramebuffer () const;

protected:

    // flags
    bool multisampleEnabled;
    bool stencilTestEnabled;
    bool depthTestEnabled;    
    bool faceCullingEnabled;
    bool framebufferEnabled;

    bool clearColorOnFrameEnabled;
    bool clearDepthOnFrameEnabled;
    bool clearStencilOnFrameEnabled;

    // parent renderer
    IRenderer* renderer;

    // output framebuffer
    Framebuffer* framebuffer;

    // these indicate which framebuffer and which render buffer should be current stage's input
    // and where to write to
    EPipelineLink pipelineFramebufferInputLink;
    EPipelineLink pipelineRenderbufferLink;
    EPipelineLink pipelineFramebufferOutputLink;

    // stencil testing parameters
    EStencilTestFunction stencilTestFunction;
    int stencilTestValue;
};
