#include "graphics/DeferredGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include <string>

namespace cilantro {

DeferredGeometryRenderStage::DeferredGeometryRenderStage () 
    : RenderStage ()
{
}

DeferredGeometryRenderStage::~DeferredGeometryRenderStage ()
{
}

void DeferredGeometryRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void DeferredGeometryRenderStage::Deinitialize()
{

}

void DeferredGeometryRenderStage::InitializeFramebuffer ()
{
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (m_renderer->GetWidth (), m_renderer->GetHeight (), 0, 5, 0, true, m_isMultisampleEnabled);
    }
}

void DeferredGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateCameraBuffers (m_renderer->GetGameScene ()->GetActiveCamera ());

    // GEOMETRY PASS
    // draw all objects in scene using geometry shader, construct g-buffer
    // set stencil value to lowest depth lighting shader handle

    m_renderer->SetStencilTestOperation (EStencilTestOperation::OP_KEEP, EStencilTestOperation::OP_KEEP, EStencilTestOperation::OP_REPLACE);

    for (auto gameObject : m_renderer->GetGameScene ()->GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (auto meshObject = dynamic_pointer_cast<MeshObject>(gameObject))
        {
            m_renderer->SetStencilTestFunction (EStencilTestFunction::FUNCTION_ALWAYS, (int) m_renderer->GetShaderProgramManager ()->GetByName<ShaderProgram>(meshObject->GetMaterial ()->GetDeferredLightingPassShaderProgram ())->GetHandle ());
        }

        // draw to g-buffer
        gameObject->OnDraw (*m_renderer);

    }
 
}


} // namespace cilantro

