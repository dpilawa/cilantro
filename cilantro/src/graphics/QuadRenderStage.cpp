#include "graphics/QuadRenderStage.h"
#include "graphics/Renderer.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"

QuadRenderStage::QuadRenderStage () : RenderStage ()
{
    shaderProgram = nullptr;
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
    // no op
}

void QuadRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();
 
    // bind textures of framebuffer linked as previous (input) and draw
    shaderProgram->Use ();
    renderer->GetPipelineFramebuffer (pipelineFramebufferInputLink)->BindFramebufferTextures ();
    renderer->RenderGeometryBuffer (renderer->GetQuadGeometryBuffer ());

    // blit framebuffer
    if (framebuffer != nullptr)
    {
        framebuffer->BlitFramebuffer ();
    }
}

QuadRenderStage& QuadRenderStage::SetShaderProgram (const std::string& shaderProgramName)
{
    shaderProgram = &(renderer->GetShaderProgramManager ().GetByName<ShaderProgram> (shaderProgramName));

    return *this;
}

void QuadRenderStage::InitializeFramebuffer ()
{   
    if (framebufferEnabled)
    {
        framebuffer = renderer->CreateFramebuffer (0, 1, multisampleEnabled);
    }
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue)
{
    shaderProgram->SetUniformFloat (parameterName, parameterValue);
    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue)
{
    shaderProgram->SetUniformVector2f (parameterName, parameterValue);
    
    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue)
{    
    shaderProgram->SetUniformVector3f (parameterName, parameterValue);
    
    return *this;
}

QuadRenderStage& QuadRenderStage::SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue)
{
    shaderProgram->SetUniformVector4f (parameterName, parameterValue);
    
    return *this;
}