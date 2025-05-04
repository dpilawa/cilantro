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

    virtual std::shared_ptr<IRenderer> GetRenderer () const = 0;

    virtual void InitializeFramebuffer () = 0;
    virtual std::shared_ptr<IFramebuffer> GetFramebuffer () const = 0;
    virtual std::shared_ptr<IFramebuffer> GetLinkedColorAttachmentsFramebuffer () const = 0;
    virtual std::shared_ptr<IFramebuffer> GetLinkedDepthStencilFramebuffer () const = 0;
    virtual std::shared_ptr<IFramebuffer> GetLinkedDepthArrayFramebuffer () const = 0;
    virtual std::shared_ptr<IFramebuffer> GetLinkedDrawFramebuffer () const = 0;

    virtual void OnFrame () = 0;
    virtual std::shared_ptr<IRenderStage> SetViewport (float u, float v, float su, float sv) = 0;

    virtual std::shared_ptr<IRenderStage> SetMultisampleEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetStencilTestEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetDepthTestEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetFaceCullingEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetFramebufferEnabled (bool value) = 0;

    virtual std::shared_ptr<IRenderStage> SetClearColorOnFrameEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetClearDepthOnFrameEnabled (bool value) = 0;
    virtual std::shared_ptr<IRenderStage> SetClearStencilOnFrameEnabled (bool value) = 0;

    virtual std::shared_ptr<IRenderStage> SetDepthTest (EDepthTestFunction depthTestFunction) = 0;
    virtual std::shared_ptr<IRenderStage> SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue) = 0;
    virtual std::shared_ptr<IRenderStage> SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection) = 0;

    virtual bool IsMultisampleEnabled () const = 0;
    virtual bool IsStencilTestEnabled () const = 0;
    virtual bool IsDepthTestEnabled () const = 0;
    virtual bool IsFaceCullingEnabled () const = 0;
    virtual bool IsFramebufferEnabled () const = 0;

    virtual bool IsClearColorOnFrameEnabled () const = 0;
    virtual bool IsClearDepthOnFrameEnabled () const = 0;
    virtual bool IsClearStencilOnFrameEnabled () const = 0;

    virtual std::shared_ptr<IRenderStage> SetColorAttachmentsFramebufferLink (EPipelineLink link) = 0;
    virtual std::shared_ptr<IRenderStage> SetDepthStencilFramebufferLink (EPipelineLink link) = 0;
    virtual std::shared_ptr<IRenderStage> SetDepthArrayFramebufferLink (EPipelineLink link) = 0;
    virtual std::shared_ptr<IRenderStage> SetDrawFramebufferLink (EPipelineLink link) = 0;

};

} // namespace cilantro
