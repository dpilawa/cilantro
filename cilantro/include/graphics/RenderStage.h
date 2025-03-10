#pragma once

#include "cilantroengine.h"
#include "graphics/IRenderer.h"
#include "graphics/IRenderStage.h"
#include "resource/Resource.h"

namespace cilantro {

class RenderStage : public IRenderStage, public Resource
{
    friend class Renderer;
public:
    RenderStage ();
    virtual ~RenderStage ();

    ///////////////////////////////////////////////////////////////////////////

    virtual IFramebuffer* GetFramebuffer () const override final;
    virtual IFramebuffer* GetLinkedColorAttachmentsFramebuffer () const override final;
    virtual IFramebuffer* GetLinkedDepthStencilFramebuffer () const override final;
    virtual IFramebuffer* GetLinkedDepthArrayFramebuffer () const override final;
    virtual IFramebuffer* GetLinkedDrawFramebuffer () const override final;

    virtual void OnFrame () override;
    __EAPI virtual IRenderStage& SetViewport (float u, float v, float su, float sv) override;

    __EAPI virtual IRenderStage& SetMultisampleEnabled (bool value) override;
    __EAPI virtual IRenderStage& SetStencilTestEnabled (bool value) override;
    __EAPI virtual IRenderStage& SetDepthTestEnabled (bool value) override;
    __EAPI virtual IRenderStage& SetFaceCullingEnabled (bool value) override;
    __EAPI virtual IRenderStage& SetFramebufferEnabled (bool value) override;

    __EAPI virtual IRenderStage& SetClearColorOnFrameEnabled (bool value) override final;
    __EAPI virtual IRenderStage& SetClearDepthOnFrameEnabled (bool value) override final;
    __EAPI virtual IRenderStage& SetClearStencilOnFrameEnabled (bool value) override final;

    __EAPI virtual IRenderStage& SetStencilTest (EStencilTestFunction stencilTestFunction, int stencilTestValue) override final;
    __EAPI virtual IRenderStage& SetFaceCullingMode (EFaceCullingFace faceCullingFace, EFaceCullingDirection faceCullingDirection) override final;

    __EAPI bool IsMultisampleEnabled () const override final;
    __EAPI bool IsStencilTestEnabled () const override final;
    __EAPI bool IsDepthTestEnabled () const override final;
    __EAPI bool IsFaceCullingEnabled () const override final;
    __EAPI bool IsFramebufferEnabled () const override final;

    __EAPI bool IsClearColorOnFrameEnabled () const override final;
    __EAPI bool IsClearDepthOnFrameEnabled () const override final;
    __EAPI bool IsClearStencilOnFrameEnabled () const override final;

    __EAPI virtual IRenderStage& SetColorAttachmentsFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual IRenderStage& SetDepthStencilFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual IRenderStage& SetDepthArrayFramebufferLink (EPipelineLink link) override final;
    __EAPI virtual IRenderStage& SetOutputFramebufferLink (EPipelineLink link) override final;

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
    IRenderer* m_renderer;

    // this stage's framebuffer
    IFramebuffer* m_framebuffer;

    // these indicate which framebuffer and which render buffer should be current stage's input
    // and where to write to
    EPipelineLink m_colorAttachmentsFramebufferLink;
    EPipelineLink m_depthStencilFramebufferLink;
    EPipelineLink m_depthArrayFramebufferLink;
    EPipelineLink m_drawFramebufferLink;

    // linked framebuffers
    IFramebuffer* m_linkedColorAttachmentsFramebuffer;
    IFramebuffer* m_linkedDepthStencilFramebuffer;
    IFramebuffer* m_linkedDepthArrayFramebuffer;
    IFramebuffer* m_linkedDrawFramebuffer;

    // stencil testing parameters
    EStencilTestFunction m_stencilTestFunction;
    int m_stencilTestValue;

    // depth culling parameter
    EFaceCullingFace m_faceCullingFace;
    EFaceCullingDirection m_faceCullingDirection;
};

} // namespace cilantro