#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/Timer.h"
#include "ControlledCamera.h"
#include <filesystem>

using namespace cilantro;

int main (int argc, char* argv [])
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");
    auto renderer = scene->Create<GLFWRenderer> (800, 600, true, false, "Test 04", false, true, true);
    auto inputController = game->Create<GLFWInputController> ();

    AssimpModelLoader modelLoader (game);

    renderer->Create<QuadRenderStage> ("hdr_postprocess")->SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer->Create<QuadRenderStage> ("fxaa_postprocess")->SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer->GetWidth (), 1.0f / renderer->GetHeight ())).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);   
    renderer->Create<QuadRenderStage> ("gamma_postprocess+screen")->SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);

    modelLoader.Load ("scene", "assets/Cerberus_LP.FBX");
    game->GetResourceManager ()->Load<Texture> ("tAlbedo", "assets/Textures/Cerberus_A.tga");
    game->GetResourceManager ()->Load<Texture> ("tNormal", "assets/Textures/Cerberus_N.tga");
    game->GetResourceManager ()->Load<Texture> ("tMetalness", "assets/Textures/Cerberus_M.tga");
    game->GetResourceManager ()->Load<Texture> ("tRoughness", "assets/Textures/Cerberus_R.tga");
  
    inputController->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController->BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController->CreateInputEvent ("mousemode", EInputKey::KeySpace, EInputTrigger::Release, {});
    inputController->BindInputEvent ("mousemode", [ & ]() { inputController->SetMouseGameMode (!inputController->IsGameMode ()); });

    scene->Create<PBRMaterial> ("gunMaterial")
        ->SetAlbedo ("tAlbedo")
        ->SetNormal ("tNormal")
        ->SetMetallic ("tMetalness")
        ->SetRoughness ("tRoughness");

    scene->GetGameObjectManager ()->GetByName<MeshObject> ("Cerberus00_Fixed")
        ->SetMaterial ("gunMaterial")
        ->GetModelTransform ()->Rotate (0.0f, 0.0f, 135.0f);

    scene->Create<ControlledCamera> ("camera", 60.0f, 10.0f, 250.0f, 1.5f, 0.1f)
        ->Initialize ()
        ->GetModelTransform ()->Translate (0.0f, 0.0f, 50.0f);

    scene->SetActiveCamera ("camera");

    scene->Create<DirectionalLight> ("light")
        ->SetColor (Vector3f (2.5f, 2.1f, 1.7f))
        ->SetEnabled (true)    
        ->GetModelTransform ()->Rotate (45.0f, -120.0f, 0.0f);

    game->Run ();

    game->Deinitialize ();

    return 0;
}

