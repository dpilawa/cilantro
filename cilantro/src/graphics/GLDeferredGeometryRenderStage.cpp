#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "glad/glad.h"
#include <string>

CGLDeferredGeometryRenderStage::CGLDeferredGeometryRenderStage () 
    : CRenderStage ()
{
}

void CGLDeferredGeometryRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void CGLDeferredGeometryRenderStage::OnFrame ()
{
    CRenderStage::OnFrame ();

    // set viewport
    glViewport (0, 0, m_renderer->GetWidth (), m_renderer->GetHeight ());

    // load uniform buffers
    m_renderer->UpdateCameraBuffers (*m_renderer->GetGameScene ()->GetActiveCamera ());

    // GEOMETRY PASS
    // draw all objects in scene using geometry shader, construct g-buffer
    // set stencil value to lowest depth lighting shader handle

    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);

    for (auto gameObject : m_renderer->GetGameScene ()->GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (MeshObject* meshObject = dynamic_cast<MeshObject*>(gameObject.get ()))
        {
            glStencilFunc (GL_ALWAYS, m_renderer->GetShaderProgramManager ().GetByName<ShaderProgram>(meshObject->GetMaterial ().GetDeferredLightingPassShaderProgram ()).GetHandle (), 0xff);
            glStencilMask (0xff);
        }

        // draw to g-buffer
        gameObject->OnDraw (*m_renderer);

    }
 
}

void CGLDeferredGeometryRenderStage::InitializeFramebuffer ()
{
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (0, 5, m_isMultisampleEnabled);
    }
}

