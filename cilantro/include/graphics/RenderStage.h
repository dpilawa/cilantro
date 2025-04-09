#pragma once

#include "cilantroengine.h"
#include "graphics/IRenderer.h"
#include "graphics/IRenderStage.h"
#include "resource/Resource.h"
#include <memory>

namespace cilantro {

class __CEAPI RenderStage : public IRenderStage, public Resource, public std::enable_shared_from_this<RenderStage>
{
public:
    __EAPI RenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~RenderStage ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual std::shared_ptr<IRenderer> GetRenderer () const override final;

    __EAPI virtual std::shared_ptr<IFramebuffer> GetFramebuffer () const override final;
    __EAPI virtual std::shared_ptr<IFramebuffer> GetLinkedColorAttachmentsFramebuffer () const override final;
    __EAPI virtual std::shared_ptr<IFramebuffer> GetLinkedDepthStencilFramebuffer () const override final;
    __EAPI virtual std::shared_ptr<IFramebuffer> GetLinkedDepthArrayFramebuffer () const override final;
    __EAPI virtual std::shared_ptr<IFramebuffer> GetLinkedDrawFramebuffer () const override final;

    __EAPI virtual void OnFrame () override;
    __EAPI virtual std::shared_ptr<IRenderStage> SetViewport (float u, float v, float su, float sv) override;

    __EAPI virtual std::shared_ptr<IRenderStage> SetMultisampleEnabled (bool value) override;
    __EAPI virtual std::shared_ptr<IRenderStage> SetStencilTestEnabled (bool value) override;
    __EAPI virtual std::shared_ptr<IRenderStage> SetDepthTestEnabled (bool value) override;
    __EAPI virtual std::shared_ptr<IRenderStage> SetFaceCullingEnabled (bool value) override;
    __EAPI virtual std::shared_ptr<IRenderStage> SetFramebufferEnabled (bool value) override;

    __EAPI virtual std::shared_ptr<IRenderStage> SetClearColorOnFrameEnabled (bool value) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetClearDepthOnFrameEnabled (bool value) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetClearStencilOnFrameEnabled (bool value) override final;

    __EAPI virtual std::shared_ptr<IRenderStage> SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection) override final;

    __EAPI bool IsMultisampleEnabled () const override final;
    __EAPI bool IsStencilTestEnabled () const override final;
    __EAPI bool IsDepthTestEnabled () const override final;
    __EAPI bool IsFaceCullingEnabled () const override final;
    __EAPI bool IsFramebufferEnabled () const override final;

    __EAPI bool IsClearColorOnFrameEnabled () const override final;
    __EAPI bool IsClearDepthOnFrameEnabled () const override final;
    __EAPI bool IsClearStencilOnFrameEnabled () const override final;

    __EAPI virtual std::shared_ptr<IRenderStage> SetColorAttachmentsFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetDepthStencilFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetDepthArrayFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual std::shared_ptr<IRenderStage> SetOutputFramebufferLink (EPipelineLink link) override final;

    ///////////////////////////////////////////////////////////////////////////

protected:
    // flags
    bool m_isMultisampleEnabled;
    bool m_isStencilTestEnabled;
    bool m_isDepthTestEnabled;    
    bool m_isFaceCullingEnabled;
    bool m_isFramebufferEnabled;

    bool m_isClearColorOnFrameEnabled;
    bool m_isClearDepthOnFrameEnabled;
    bool m_isClearStencilOnFrameEnabled;

    // viewport relative position and dimension
    float m_viewportU;
    float m_viewportV;
    float m_viewportSizeU;
    float m_viewportSizeV;

    // parent renderer
    std::weak_ptr<IRenderer> m_renderer;

    // this stage's framebuffer
    std::shared_ptr<IFramebuffer> m_framebuffer;

    // these indicate which framebuffer and which render buffer should be current stage's input
    // and where to write to
    EPipelineLink m_colorAttachmentsFramebufferLink;
    EPipelineLink m_depthStencilFramebufferLink;
    EPipelineLink m_depthArrayFramebufferLink;
    EPipelineLink m_drawFramebufferLink;

    // linked framebuffers
    std::shared_ptr<IFramebuffer> m_linkedColorAttachmentsFramebuffer;
    std::shared_ptr<IFramebuffer> m_linkedDepthStencilFramebuffer;
    std::shared_ptr<IFramebuffer> m_linkedDepthArrayFramebuffer;
    std::shared_ptr<IFramebuffer> m_linkedDrawFramebuffer;

    // stencil testing parameters
    EStencilTestFunction m_stencilTestFunction;
    int m_stencilTestValue;

    // depth culling parameter
    EFaceCullingFace m_faceCullingFace;
    EFaceCullingDirection m_faceCullingDirection;
};

} // namespace cilantro