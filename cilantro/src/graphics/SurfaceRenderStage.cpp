#include "graphics/SurfaceRenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "graphics/ShaderProgram.h"

namespace cilantro {

SurfaceRenderStage::SurfaceRenderStage (std::shared_ptr<IRenderer> renderer) 
    : RenderStage (renderer)
    , m_shaderProgram (nullptr)
{
}

SurfaceRenderStage::~SurfaceRenderStage ()
{
}

void SurfaceRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void SurfaceRenderStage::Deinitialize ()
{
}

void SurfaceRenderStage::InitializeFramebuffer ()
{   
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = GetRenderer ()->CreateFramebuffer (GetRenderer ()->GetWidth (), GetRenderer ()->GetHeight (), 0, 1, 0, true, m_isMultisampleEnabled);
    }
}

void SurfaceRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // bind textures of framebuffer linked as previous (input) and draw
    m_shaderProgram->Use ();
    m_linkedColorAttachmentsFramebuffer->BindFramebufferColorTexturesAsColor ();

    // draw quad
    GetRenderer ()->DrawSurface ();

    // blit framebuffer
    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }
}

std::shared_ptr<SurfaceRenderStage> SurfaceRenderStage::SetShaderProgram (const std::string& shaderProgramName)
{
    m_shaderProgram = GetRenderer ()->GetShaderProgramManager ()->GetByName<ShaderProgram> (shaderProgramName);

    return std::dynamic_pointer_cast<SurfaceRenderStage> (shared_from_this ());
}

std::shared_ptr<SurfaceRenderStage> SurfaceRenderStage::SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue)
{
    m_shaderProgram->SetUniformFloat (parameterName, parameterValue);

    return std::dynamic_pointer_cast<SurfaceRenderStage> (shared_from_this ());
}

std::shared_ptr<SurfaceRenderStage> SurfaceRenderStage::SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue)
{
    m_shaderProgram->SetUniformVector2f (parameterName, parameterValue);
    
    return std::dynamic_pointer_cast<SurfaceRenderStage> (shared_from_this ());
}

std::shared_ptr<SurfaceRenderStage> SurfaceRenderStage::SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue)
{    
    m_shaderProgram->SetUniformVector3f (parameterName, parameterValue);
    
    return std::dynamic_pointer_cast<SurfaceRenderStage> (shared_from_this ());
}

std::shared_ptr<SurfaceRenderStage> SurfaceRenderStage::SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue)
{
    m_shaderProgram->SetUniformVector4f (parameterName, parameterValue);
    
    return std::dynamic_pointer_cast<SurfaceRenderStage> (shared_from_this ());
}


} // namespace cilantro