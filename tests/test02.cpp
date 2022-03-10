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
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/Timer.h"

#include "Orbiter.h"

int main (int argc, char* argv [])
{
    Game::Initialize ();

    GameScene& gameScene = Game::CreateGameScene<GameScene> ("scene");
    GLFWRenderer& renderer = gameScene.CreateRenderer<GLFWRenderer> (960, 600, false, "Test 02", false, true, true);
    InputController& inputController = Game::CreateInputController<GLFWInputController> ();

    renderer.AddRenderStage<QuadRenderStage> ("screen").SetShaderProgram ("flatquad_shader").SetFramebufferEnabled (false).SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS);
    renderer.GetRenderStageManager ().GetByName<RenderStage> ("base").SetMultisampleEnabled (true);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { Game::Stop (); });

    Game::GetResourceManager().Load<Texture> ("tEarthDiffuse", "textures/2k_earth_daymap.jpg");
    Game::GetResourceManager().Load<Texture> ("tEarthSpec", "textures/2k_earth_specular_map.png");
    Game::GetResourceManager().Load<Texture> ("tEarthNormal", "textures/2k_earth_normal_map.png");
    Game::GetResourceManager().Load<Texture> ("tMoon", "textures/2k_moon.jpg");
    Game::GetResourceManager().Load<Texture> ("tSun", "textures/2k_sun.jpg");

    gameScene.AddMaterial<PhongMaterial> ("mSun").SetEmissive ("tSun");
    gameScene.AddMaterial<PhongMaterial> ("mEarth").SetDiffuse ("tEarthDiffuse").SetSpecular ("tEarthSpec").SetNormal("tEarthNormal").SetSpecularShininess (32.0f);
    gameScene.AddMaterial<PhongMaterial> ("mMoon").SetDiffuse ("tMoon").SetSpecularShininess (1.0f).SetSpecular (Vector3f(0.2f, 0.2f, 0.2f));

    PerspectiveCamera& cam = gameScene.AddGameObject<PerspectiveCamera> ("camera", 25.0f, 1.0f, 500.0f);
    cam.GetLocalTransform ().Translate (0.0f, 0.0f, 160.0f);
    gameScene.SetActiveCamera ("camera");

    Mesh& sunMesh = Game::GetResourceManager().Create<Mesh> ("sunMesh");
    MeshObject& sun = gameScene.AddGameObject<MeshObject> ("sun", "sunMesh", "mSun");
    Primitives::GenerateSphere (sunMesh, 8);
    sun.GetLocalTransform ().Scale (10.0f);

    Orbiter& earthOrbit = gameScene.AddGameObject<Orbiter> ("earthOrbit", gameScene.GetGameObjectManager ().GetByName<GameObject> ("sun"), 1.0f, 23.5f, 365.256f, 50.0f, 0.0f);
    Mesh& earthMesh = Game::GetResourceManager().Create<Mesh> ("earthMesh").SetSmoothNormals (true);
    MeshObject& earth = gameScene.AddGameObject<MeshObject> ("earth", "earthMesh", "mEarth");
    Primitives::GenerateSphere (earthMesh, 8);
    earth.SetParentObject ("earthOrbit");
    earth.GetLocalTransform ().Scale (3.0f);

    Orbiter& moonOrbit = gameScene.AddGameObject<Orbiter> ("moonOrbit", gameScene.GetGameObjectManager ().GetByName<GameObject> ("earth"), 27.321f, -6.68f, 27.321f, 20.0f, -5.14f);
    Mesh& moonMesh = Game::GetResourceManager().Create<Mesh> ("moonMesh").SetSmoothNormals (true);
    MeshObject& moon = gameScene.AddGameObject<MeshObject> ("moon", "moonMesh", "mMoon");
    Primitives::GenerateSphere (moonMesh, 8);
    moon.GetLocalTransform ().Scale (0.273f * 5.0f);
    moon.SetParentObject ("moonOrbit");

    PointLight& sunLight = gameScene.AddGameObject<PointLight> ("sunLight");
    sunLight.SetParentObject ("sun");
    sunLight.SetColor (Vector3f (1.1f, 1.0f, 1.0f)).SetEnabled (true);

    SplinePath& path = gameScene.AddGameObject<SplinePath> ("path");
    path.AddWaypoint({0.0f, 80.0f, 260.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({-15.0f, 0.0f, 0.0f})));
    path.AddWaypoint({0.0f, 0.0f, 140.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({0.0f, 0.0f, 0.0f})));
    path.SetStartTangent({0.0f, -2.0f, -1.0f});
    path.SetEndTangent({0.0f, 0.0f, -1.0f});

    AnimationObject& animation = gameScene.AddGameObject<AnimationObject> ("animation");
    animation.AddAnimationProperty<float> ("u", 0.0f, 
        [&](float u) 
        {
            cam.GetLocalTransform ().Translate (path.GetPositionAtDistance (u * path.GetPathLength ()));
            cam.GetLocalTransform ().Rotate (path.GetRotationAtDistance (u * path.GetPathLength ()));
        },
        [](float t0, float t1, float u)
        {
            return Mathf::Smootherstep (t0, t1, u);
        }
    );
    animation.AddKeyframe("u", 10.0f, 1.0f);
    animation.SetLooping (false);
    animation.Play ();

    Game::Run ();

    Game::Deinitialize ();

    return 0;
}

