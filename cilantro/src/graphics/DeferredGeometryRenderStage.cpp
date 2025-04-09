#include "graphics/DeferredGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include <string>

namespace cilantro {

DeferredGeometryRenderStage::DeferredGeometryRenderStage (std::shared_ptr<IRenderer> renderer) 
    : RenderStage (renderer)
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
        m_framebuffer = GetRenderer ()->CreateFramebuffer (GetRenderer ()->GetWidth (), GetRenderer ()->GetHeight (), 0, 5, 0, true, m_isMultisampleEnabled);
    }
}

void DeferredGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    GetRenderer ()->UpdateCameraBuffers (GetRenderer ()->GetGameScene ()->GetActiveCamera ());

    // GEOMETRY PASS
    // draw all objects in scene using geometry shader, construct g-buffer
    // set stencil value to lowest depth lighting shader handle

    GetRenderer ()->SetStencilTestOperation (EStencilTestOperation::OP_KEEP, EStencilTestOperation::OP_KEEP, EStencilTestOperation::OP_REPLACE);

    for (auto gameObject : GetRenderer ()->GetGameScene ()->GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (auto meshObject = dynamic_pointer_cast<MeshObject>(gameObject))
        {
            GetRenderer ()->SetStencilTestFunction (EStencilTestFunction::FUNCTION_ALWAYS, (int) GetRenderer ()->GetShaderProgramManager ()->GetByName<ShaderProgram>(meshObject->GetMaterial ()->GetDeferredLightingPassShaderProgram ())->GetHandle ());
        }

        // draw to g-buffer
        gameObject->OnDraw (*(m_renderer.lock ()));

    }
 
}


} // namespace cilantro

