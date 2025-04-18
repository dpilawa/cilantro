#include "cilantroengine.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/AABBRenderStage.h"
#include "graphics/SurfaceRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/Renderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>
#include <filesystem>

using namespace cilantro;

int main (int argc, char* argv[])
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");
    auto renderer = scene->Create<GLFWRenderer> (800, 600, true, true, "Test 03", false, true, true);
    auto inputController = game->Create<GLFWInputController> ();

    AssimpModelLoader modelLoader (game);

    renderer->Create<AABBRenderStage> ("aabb")
        ->SetDepthStencilFramebufferLink (EPipelineLink::LINK_SECOND)
        ->SetClearColorOnFrameEnabled (false)
        ->SetClearDepthOnFrameEnabled (false)
        ->SetDrawFramebufferLink (EPipelineLink::LINK_THIRD)
        ->SetDepthTestEnabled (true)
        ->SetFramebufferEnabled (false);

    renderer->Create<SurfaceRenderStage> ("hdr_postprocess")
        ->SetShaderProgram ("post_hdr_shader")
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_THIRD); // LINK_PREVIOUS

    renderer->Create<SurfaceRenderStage> ("fxaa_postprocess")
        ->SetShaderProgram ("post_fxaa_shader")
        ->SetRenderStageParameterFloat ("fMaxSpan", 4.0f)
        ->SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer->GetWidth (), 1.0f / renderer->GetHeight ()))
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);

    renderer->Create<SurfaceRenderStage> ("gamma_postprocess+screen")
        ->SetShaderProgram ("post_gamma_shader")
        ->SetRenderStageParameterFloat ("fGamma", 2.1f)
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS)
        ->SetFramebufferEnabled (false);    

    inputController->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController->BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController->CreateInputEvent ("mousemode", EInputKey::KeySpace, EInputTrigger::Release, {});
    inputController->BindInputEvent ("mousemode", [ & ]() { inputController->SetMouseGameMode (!inputController->IsGameMode ()); });

    modelLoader.Load ("scene", "assets/Drunk Idle.fbx");

    scene->Create<PBRMaterial> ("floorMaterial")
        ->SetAlbedo (Vector3f (0.3f, 0.2f, 0.2f))
        ->SetRoughness (0.1f)
        ->SetMetallic (0.6f);

    Primitives::GenerateCube (game->GetResourceManager ()->Create<Mesh> ("floorMesh"));
    scene->Create<MeshObject> ("floor", "floorMesh", "floorMaterial")
        ->GetModelTransform ()->Scale (1000.0f, 0.1f, 1000.0f)->Translate (0.0f, -0.05f, 0.0f);

    scene->Create<ControlledCamera> ("camera", 60.0f, 10.0f, 600.0f, 5.0f, 0.1f)
        ->Initialize ()
        ->GetModelTransform ()->Translate (0.0f, 100.0f, 250.0f);
        
    scene->SetActiveCamera ("camera");

    scene->Create<PointLight> ("light")
        ->SetColor (Vector3f (1.0f, 1.0f, 1.0f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Translate (100.0f, 100.0f, 100.0f);

    scene->Create<DirectionalLight> ("light2")
        ->SetColor (Vector3f (1.0f, 1.0f, 1.0f))
        ->SetEnabled (true) 
        ->GetModelTransform ()->Rotate (60.0f, 180.0f, 0.0f);

    auto anim = scene->GetGameObjectManager ()->GetByName<AnimationObject> ("mixamo.com");
    anim->SetLooping (true);
    anim->Play();

    game->Run ();

    game->Deinitialize ();

    return 0;
}

