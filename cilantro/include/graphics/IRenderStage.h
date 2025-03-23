#pragma once

#include "cilantroengine.h"
#include "graphics/IRenderer.h"

namespace cilantro {

struct IFramebuffer;

struct IRenderStage
{
    virtual ~IRenderStage () {};

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual IRenderer* GetRenderer () const = 0;

    virtual void InitializeFramebuffer () = 0;
    virtual IFramebuffer* GetFramebuffer () const = 0;
    virtual IFramebuffer* GetLinkedColorAttachmentsFramebuffer () const = 0;
    virtual IFramebuffer* GetLinkedDepthStencilFramebuffer () const = 0;
    virtual IFramebuffer* GetLinkedDepthArrayFramebuffer () const = 0;
    virtual IFramebuffer* GetLinkedDrawFramebuffer () const = 0;

    virtual void OnFrame () = 0;
    virtual IRenderStage& SetViewport (float u, float v, float su, float sv) = 0;

    virtual IRenderStage& SetMultisampleEnabled (bool value) = 0;
    virtual IRenderStage& SetStencilTestEnabled (bool value) = 0;
    virtual IRenderStage& SetDepthTestEnabled (bool value) = 0;
    virtual IRenderStage& SetFaceCullingEnabled (bool value) = 0;
    virtual IRenderStage& SetFramebufferEnabled (bool value) = 0;

    virtual IRenderStage& SetClearColorOnFrameEnabled (bool value) = 0;
    virtual IRenderStage& SetClearDepthOnFrameEnabled (bool value) = 0;
    virtual IRenderStage& SetClearStencilOnFrameEnabled (bool value) = 0;

    virtual IRenderStage& SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue) = 0;
    virtual IRenderStage& SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection) = 0;

    virtual bool IsMultisampleEnabled () const = 0;
    virtual bool IsStencilTestEnabled () const = 0;
    virtual bool IsDepthTestEnabled () const = 0;
    virtual bool IsFaceCullingEnabled () const = 0;
    virtual bool IsFramebufferEnabled () const = 0;

    virtual bool IsClearColorOnFrameEnabled () const = 0;
    virtual bool IsClearDepthOnFrameEnabled () const = 0;
    virtual bool IsClearStencilOnFrameEnabled () const = 0;

    virtual IRenderStage& SetColorAttachmentsFramebufferLink (EPipelineLink link) = 0;
    virtual IRenderStage& SetDepthStencilFramebufferLink (EPipelineLink link) = 0;
    virtual IRenderStage& SetDepthArrayFramebufferLink (EPipelineLink link) = 0;
    virtual IRenderStage& SetOutputFramebufferLink (EPipelineLink link) = 0;

};

} // namespace cilantro
