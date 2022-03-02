#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/RenderStage.h"
#include "graphics/Framebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "glad/glad.h"
#include <string>

GLDeferredGeometryRenderStage::GLDeferredGeometryRenderStage () : RenderStage ()
{
}

GLDeferredGeometryRenderStage::~GLDeferredGeometryRenderStage ()
{

}

void GLDeferredGeometryRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void GLDeferredGeometryRenderStage::Deinitialize ()
{    
}

void GLDeferredGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // clear frame, depth and stencil buffers
    renderer->ClearColorBuffer (Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
    renderer->ClearDepthBuffer ();
    renderer->ClearStencilBuffer ();

    // set viewport
    glViewport (0, 0, renderer->GetWidth (), renderer->GetHeight ());

    // enable depth and stencil buffer testing
    renderer->SetDepthTestEnabled (true);
    renderer->SetStencilTestEnabled (true);

    // load uniform buffers
    renderer->LoadMatrixUniformBuffers ();

    // GEOMETRY PASS
    // draw all objects in scene using geometry shader, construct g-buffer
    // set stencil value to lowest depth lighting shader handle

    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);

    for (auto gameObject : renderer->GetGameScene ()->GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (MeshObject* meshObject = dynamic_cast<MeshObject*>(gameObject.get ()))
        {
            glStencilFunc (GL_ALWAYS, renderer->GetShaderProgramManager ().GetByName<ShaderProgram>(meshObject->GetMaterial ().GetDeferredLightingPassShaderProgram ()).GetHandle (), 0xff);
            glStencilMask (0xff);
        }

        // draw to g-buffer
        gameObject->OnDraw (*renderer);

    }
 
}

void GLDeferredGeometryRenderStage::InitializeFramebuffer ()
{
    if (framebufferEnabled)
    {
        framebuffer = renderer->CreateFramebuffer (0, 5, multisampleEnabled);
    }
}

