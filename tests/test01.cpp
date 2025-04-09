#include "cilantroengine.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/LinearPath.h"
#include "scene/SplinePath.h"
#include "resource/Mesh.h"
#include "resource/ResourceManager.h"
#include "resource/Texture.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>


using namespace cilantro;

int main (int argc, char* argv [])
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");
    auto renderer = scene->Create<GLFWRenderer> (800, 600, true, true, "Test 01", false, true, true);
    auto inputController = game->Create<GLFWInputController> ();
    
    renderer->Create<QuadRenderStage> ("hdr_postprocess")
        ->SetShaderProgram ("post_hdr_shader")
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);

    renderer->Create<QuadRenderStage> ("fxaa_postprocess")
        ->SetShaderProgram ("post_fxaa_shader")
        ->SetRenderStageParameterFloat ("fMaxSpan", 4.0f)
        ->SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer->GetWidth (), 1.0f / renderer->GetHeight ()))
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);

    renderer->Create<QuadRenderStage> ("gamma_postprocess+screen")
        ->SetShaderProgram ("post_gamma_shader")
        ->SetRenderStageParameterFloat ("fGamma", 2.1f)
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS)
        ->SetFramebufferEnabled (false);  

    inputController->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController->BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController->CreateInputEvent ("mousemode", EInputKey::KeySpace, EInputTrigger::Release, {});
    inputController->BindInputEvent ("mousemode", [ & ]() { inputController->SetMouseGameMode (!inputController->IsGameMode ()); });

    game->GetResourceManager ()->Load<Texture> ("tAlbedoMetal", "textures/scuffed-metal1_albedo.png");
    game->GetResourceManager ()->Load<Texture> ("tMetalnessMetal", "textures/scuffed-metal1_metallic.png");
    game->GetResourceManager ()->Load<Texture> ("tNormalMetal", "textures/scuffed-metal1_normal-dx.png");
    game->GetResourceManager ()->Load<Texture> ("tRoughnessMetal", "textures/scuffed-metal1_roughness.png");
    game->GetResourceManager ()->Load<Texture> ("tAOMetal", "textures/scuffed-metal1_ao.png");

    game->GetResourceManager ()->Load<Texture> ("tAlbedoGold", "textures/Metal007_1K_Color.png");
    game->GetResourceManager ()->Load<Texture> ("tMetalnessGold", "textures/Metal007_1K_Metalness.png");
    game->GetResourceManager ()->Load<Texture> ("tNormalGold", "textures/Metal007_1K_Normal.png");
    game->GetResourceManager ()->Load<Texture> ("tRoughnessGold", "textures/Metal007_1K_Roughness.png");

    scene->Create<PBRMaterial> ("greenMaterial")
        ->SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f))
        ->SetRoughness (0.1f)
        ->SetMetallic (0.6f)
        ->SetNormal ("tNormalMetal");

    scene->Create<PBRMaterial> ("redMaterial")
        ->SetAlbedo ("tAlbedoMetal")
        ->SetMetallic ("tMetalnessMetal")
        ->SetRoughness ("tRoughnessMetal")
        ->SetNormal("tNormalMetal")
        ->SetAO ("tAOMetal");

    scene->Create<PBRMaterial> ("goldMaterial")
        ->SetAlbedo ("tAlbedoGold")
        ->SetMetallic ("tMetalnessGold")
        ->SetRoughness ("tRoughnessGold")
        ->SetNormal("tNormalGold");

    scene->Create<PBRMaterial> ("blueMaterial")
        ->SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f))
        ->SetMetallic (0.0f)
        ->SetRoughness (0.8f);

    scene->Create<PhongMaterial> ("lampMaterial")
        ->SetEmissive (Vector3f (0.9f, 0.9f, 0.9f))
        ->SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    scene->Create<ControlledCamera> ("camera", 60.0f, 0.5f, 10.0f, 0.1f, 0.1f)
        ->Initialize ()
        ->GetModelTransform ()->Translate (5.0f, 2.5f, 5.0f)->Rotate (-20.0f, 45.0f, 0.0f);
    
    scene->SetActiveCamera ("camera");

    Primitives::GenerateCube (game->GetResourceManager ()->Create<Mesh> ("cubeMesh"));
    scene->Create<MeshObject> ("cube", "cubeMesh", "redMaterial")
        ->GetModelTransform ()->Scale (0.5f)->Translate (0.0f, 1.1f, 0.0f);

    Primitives::GenerateCone (game->GetResourceManager ()->Create<Mesh> ("coneMesh")->SetSmoothNormals (false), 32);
    scene->Create<MeshObject> ("cone", "coneMesh", "goldMaterial")
        ->GetModelTransform ()->Translate (-1.5f, 0.5f, 1.0f)->Scale (0.5f);

    Primitives::GenerateCylinder (game->GetResourceManager ()->Create<Mesh> ("cylinderMesh")->SetSmoothNormals (false), 32);
    scene->Create<MeshObject> ("cylinder", "cylinderMesh", "blueMaterial")
        ->GetModelTransform ()->Rotate (90.0f, 12.5f, 0.0f)->Translate (1.7f, 0.5f, 0.7f)->Scale (0.5f);

    Primitives::GenerateSphere (game->GetResourceManager ()->Create<Mesh> ("lampMesh"), 3);
    scene->Create<MeshObject> ("lamp", "lampMesh", "lampMaterial")
        ->GetModelTransform ()->Scale (0.1f, 0.1f, 0.1f)->Translate (1.0f, 0.75f, 1.0f);

    Primitives::GenerateCube (game->GetResourceManager ()->Create<Mesh> ("floorMesh"));
    scene->Create<MeshObject> ("floor", "floorMesh", "greenMaterial")
        ->GetModelTransform ()->Scale (2.5f, 0.05f, 2.5f)->Translate (0.0f, -0.05f, 0.0f);

    scene->Create<PointLight> ("light1")
        ->SetLinearAttenuationFactor (0.0f)
        ->SetQuadraticAttenuationFactor (1.0f)
        ->SetColor (Vector3f (1.5f, 1.5f, 1.5f))
        ->SetEnabled (true)
        ->SetParentObject ("lamp");

    scene->Create<DirectionalLight> ("light2")
        ->SetColor (Vector3f (2.7f, 2.7f, 2.7f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Rotate (45.0f, -135.0f, 0.0f);

    scene->Create<DirectionalLight> ("light2a")
        ->SetColor (Vector3f (2.7f, 2.7f, 2.7f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Rotate (45.0f, 135.0f, 0.0f);

    scene->Create<SpotLight> ("light3")
        ->SetInnerCutoff (5.0f)
        ->SetOuterCutoff (12.0f)
        ->SetColor (Vector3f (2.7f, 2.7f, 2.7f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Translate (2.0f, 10.0f, 0.0f)->Rotate (90.0f, 0.0f, 0.0f);
    
    scene->Create<SplinePath> ("splinepath")
        ->SetStartTangent ({-2.0f, 0.0f, 2.0f})
        ->SetEndTangent ({-2.0f, 0.0f, 2.0f})
        ->AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaternion ({0.0f, 0.0f, 0.0f}))
        ->AddWaypoint ({-2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaternion ({0.0f, 0.0f, 0.0f}))
        ->AddWaypoint ({2.0f, 0.0f, -2.0f}, Mathf::EulerToQuaternion ({0.0f, 0.0f, 0.0f}))
        ->AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaternion ({0.0f, 0.0f, 0.0f}))
        ->AddWaypoint (2, {-2.0f, 2.0f, -2.0f}, Mathf::EulerToQuaternion ({0.0f, 0.0f, 0.0f}))
        ->GetModelTransform ()->Rotate ({0.0f, 0.0f, -15.0f})->Translate ({0.0f, 1.0f, 0.0f});

    auto lightAnimation = scene->Create<AnimationObject> ("lightAnimation");

    lightAnimation->AddAnimationProperty<float> (
        "t", 0.0f,
        [&lightAnimation] (float t) {
            auto lamp = lightAnimation->GetGameScene ()->GetGameObjectManager ()->GetByName<MeshObject> ("lamp");
            auto lp = lightAnimation->GetGameScene ()->GetGameObjectManager ()->GetByName<SplinePath> ("splinepath");
            lamp->GetModelTransform ()->Translate (lp->GetPositionAtDistance (lp->GetPathLength () * t));
            lamp->GetModelTransform ()->Rotate (lp->GetRotationAtDistance (lp->GetPathLength () * t));
        },
        [] (float t0, float t1, float u) { return Mathf::Lerp (t0, t1, u); })
        ->AddKeyframe (5.0f, 1.0f);

    lightAnimation->SetLooping (true);
    lightAnimation->Play ();

    game->Run ();

    game->Deinitialize ();

    return 0;
}

