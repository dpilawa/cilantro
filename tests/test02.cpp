#include "cilantroengine.h"
#include "scene/AnimationObject.h"
#include "scene/Primitives.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/PerspectiveCamera.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/SplinePath.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/Timer.h"
#include "system/LogMessage.h"
#include <filesystem>

#include "Orbiter.h"

using namespace cilantro;

int main (int argc, char* argv [])
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");
    auto renderer = scene->Create<GLFWRenderer> (960, 600, true, false, "Test 02", false, true, true);
    auto inputController = game->Create<GLFWInputController> ();

    renderer->Create<QuadRenderStage> ("screen")->SetShaderProgram ("flatquad_shader").SetFramebufferEnabled (false).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer->GetRenderStageManager ().GetByName<IRenderStage> ("forward")->SetMultisampleEnabled (true);

    inputController->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController->BindInputEvent ("exit", [ & ]() { game->Stop (); });

    game->GetResourceManager().Load<Texture> ("tEarthDiffuse", "textures/2k_earth_daymap.jpg");
    game->GetResourceManager().Load<Texture> ("tEarthSpec", "textures/2k_earth_specular_map.png");
    game->GetResourceManager().Load<Texture> ("tEarthNormal", "textures/2k_earth_normal_map.png");
    game->GetResourceManager().Load<Texture> ("tMoon", "textures/2k_moon.jpg");
    game->GetResourceManager().Load<Texture> ("tSun", "textures/2k_sun.jpg");

    scene->Create<PhongMaterial> ("mSun")
        ->SetEmissive ("tSun");

    scene->Create<PhongMaterial> ("mEarth")
        ->SetDiffuse ("tEarthDiffuse")
        ->SetSpecular ("tEarthSpec")
        ->SetNormal("tEarthNormal")
        ->SetSpecularShininess (32.0f);

    scene->Create<PhongMaterial> ("mMoon")
        ->SetDiffuse ("tMoon")
        ->SetSpecularShininess (1.0f)
        ->SetSpecular (Vector3f(0.2f, 0.2f, 0.2f));

    scene->Create<PerspectiveCamera> ("camera", 25.0f, 1.0f, 500.0f)
        ->GetModelTransform ()->Translate (0.0f, 0.0f, 160.0f);
    
    scene->SetActiveCamera ("camera");

    Primitives::GenerateSphere (game->GetResourceManager().Create<Mesh> ("sunMesh"), 8);
    scene->Create<MeshObject> ("sun", "sunMesh", "mSun")
        ->GetModelTransform ()->Scale (10.0f);

    scene->Create<Orbiter> ("earthOrbit", scene->GetGameObjectManager ().GetByName<GameObject> ("sun"), 1.0f, 23.5f, 365.256f, 50.0f, 0.0f);
    Primitives::GenerateSphere (game->GetResourceManager().Create<Mesh> ("earthMesh")->SetSmoothNormals (true), 8);
    scene->Create<MeshObject> ("earth", "earthMesh", "mEarth")
        ->SetParentObject ("earthOrbit")
        ->GetModelTransform ()->Scale (3.0f);

    scene->Create<Orbiter> ("moonOrbit", scene->GetGameObjectManager ().GetByName<GameObject> ("earth"), 27.321f, -6.68f, 27.321f, 20.0f, -5.14f);
    Primitives::GenerateSphere (game->GetResourceManager().Create<Mesh> ("moonMesh")->SetSmoothNormals (true), 8);
    scene->Create<MeshObject> ("moon", "moonMesh", "mMoon")
        ->SetParentObject ("moonOrbit")
        ->GetModelTransform ()->Scale (0.273f * 5.0f);

    scene->Create<PointLight> ("sunLight")
        ->SetColor (Vector3f (1.1f, 1.0f, 1.0f))
        ->SetEnabled (true)
        ->SetParentObject ("sun");

    scene->Create<SplinePath> ("path")
        ->SetStartTangent({0.0f, -2.0f, -1.0f})
        ->SetEndTangent({0.0f, 0.0f, -1.0f})
        ->AddWaypoint({0.0f, 80.0f, 260.0f}, Mathf::EulerToQuaternion(Mathf::Deg2Rad ({-15.0f, 0.0f, 0.0f})))
        ->AddWaypoint({0.0f, 0.0f, 140.0f}, Mathf::EulerToQuaternion(Mathf::Deg2Rad ({0.0f, 0.0f, 0.0f})));

    auto animation = scene->Create<AnimationObject> ("animation");
    animation->AddAnimationProperty<float> ("u", 0.0f, 
        [animation](float u) 
        {
            auto cam = animation->GetGameScene ()->GetGameObjectManager ().GetByName<PerspectiveCamera> ("camera");
            auto path = animation->GetGameScene ()->GetGameObjectManager ().GetByName<SplinePath> ("path");
            cam->GetModelTransform ()->Translate (path->GetPositionAtDistance (u * path->GetPathLength ()));
            cam->GetModelTransform ()->Rotate (path->GetRotationAtDistance (u * path->GetPathLength ()));
        },
        [](float t0, float t1, float u)
        {
            return Mathf::Smootherstep (t0, t1, u);
        }
    );
    animation->AddKeyframe("u", 10.0f, 1.0f);
    animation->SetLooping (false);
    animation->Play ();

    game->Run ();

    game->Deinitialize ();

    return 0;
}
