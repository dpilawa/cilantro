#include "graphics/QuadRenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "graphics/ShaderProgram.h"

namespace cilantro {

QuadRenderStage::QuadRenderStage () 
    : RenderStage ()
    , m_shaderProgram (nullptr)
{
}

QuadRenderStage::~QuadRenderStage ()
{
}

void QuadRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void QuadRenderStage::Deinitialize ()
{
}

void QuadRenderStage::InitializeFramebuffer ()
{   
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (m_renderer->GetWidth (), m_renderer->GetHeight (), 0, 1, 0, true, m_isMultisampleEnabled);
    }
}

void QuadRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // bind textures of framebuffer linked as previous (input) and draw
    m_shaderProgram->Use ();
    m_linkedColorAttachmentsFramebuffer->BindFramebufferColorTexturesAsColor ();

    // bind shadow map (if exists) on next available texture slot
    if (m_linkedDepthArrayFramebuffer != nullptr && m_linkedDepthArrayFramebuffer->IsDepthArrayEnabled ())
    {
        m_linkedDepthArrayFramebuffer->BindFramebufferDepthArrayTextureAsColor (m_linkedColorAttachmentsFramebuffer->GetColorTextureCount ());
    }

    // draw quad
    m_renderer->DrawQuad ();

    // blit framebuffer
    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }
}

QuadRenderStage& QuadRenderStage::SetShaderProgram (const std::string& shaderProgramName)
{
    m_shaderProgram = m_renderer->GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName);

    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue)
{
    m_shaderProgram->SetUniformFloat (parameterName, parameterValue);

    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue)
{
    m_shaderProgram->SetUniformVector2f (parameterName, parameterValue);
    
    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue)
{    
    m_shaderProgram->SetUniformVector3f (parameterName, parameterValue);
    
    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue)
{
    m_shaderProgram->SetUniformVector4f (parameterName, parameterValue);
    
    return *this;
}


} // namespace cilantro