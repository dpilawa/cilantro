#include "cilantroengine.h"
#include "game/Game.h"
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
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/EngineContext.h"
#include "system/Timer.h"

#include "Orbiter.h"

int main (int argc, char* argv [])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 2", 960, 600, false, true, true);
    Renderer renderer (960, 600);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, renderTarget, inputController);
    EngineContext::Initialize ();

    renderer.AddRenderStage<GLForwardGeometryRenderStage> ("base").SetMultisampleEnabled (true);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    resourceManager.Load<Texture> ("tEarthDiffuse", "textures/2k_earth_daymap.jpg");
    resourceManager.Load<Texture> ("tEarthSpec", "textures/2k_earth_specular_map.png");
    resourceManager.Load<Texture> ("tEarthNormal", "textures/2k_earth_normal_map.png");
    resourceManager.Load<Texture> ("tMoon", "textures/2k_moon.jpg");
    resourceManager.Load<Texture> ("tSun", "textures/2k_sun.jpg");

    gameScene.AddMaterial<PhongMaterial> ("mSun").SetEmissive ("tSun");
    gameScene.AddMaterial<PhongMaterial> ("mEarth").SetDiffuse ("tEarthDiffuse").SetSpecular ("tEarthSpec").SetNormal("tEarthNormal").SetSpecularShininess (32.0f);
    gameScene.AddMaterial<PhongMaterial> ("mMoon").SetDiffuse ("tMoon").SetSpecularShininess (1.0f).SetSpecular (Vector3f(0.2f, 0.2f, 0.2f));

    PerspectiveCamera& cam = gameScene.AddGameObject<PerspectiveCamera> ("camera", 25.0f, 1.0f, 500.0f);
    cam.GetModelTransform ().Translate (0.0f, 0.0f, 160.0f);
    gameScene.SetActiveCamera ("camera");

    Mesh& sunMesh = resourceManager.Create<Mesh> ("sunMesh");
    MeshObject& sun = gameScene.AddGameObject<MeshObject> ("sun", "sunMesh", "mSun");
    Primitives::GenerateSphere (sunMesh, 8);
    sun.GetModelTransform ().Scale (10.0f);

    Orbiter& earthOrbit = gameScene.AddGameObject<Orbiter> ("earthOrbit", gameScene.GetGameObjectManager ().GetByName<GameObject> ("sun"), 1.0f, 23.5f, 365.256f, 50.0f, 0.0f);
    Mesh& earthMesh = resourceManager.Create<Mesh> ("earthMesh").SetSmoothNormals (true);
    MeshObject& earth = gameScene.AddGameObject<MeshObject> ("earth", "earthMesh", "mEarth");
    Primitives::GenerateSphere (earthMesh, 8);
    earth.SetParentObject ("earthOrbit");
    earth.GetModelTransform ().Scale (3.0f);

    Orbiter& moonOrbit = gameScene.AddGameObject<Orbiter> ("moonOrbit", gameScene.GetGameObjectManager ().GetByName<GameObject> ("earth"), 27.321f, -6.68f, 27.321f, 20.0f, -5.14f);
    Mesh& moonMesh = resourceManager.Create<Mesh> ("moonMesh").SetSmoothNormals (true);
    MeshObject& moon = gameScene.AddGameObject<MeshObject> ("moon", "moonMesh", "mMoon");
    Primitives::GenerateSphere (moonMesh, 8);
    moon.GetModelTransform ().Scale (0.273f * 5.0f);
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
            cam.GetModelTransform ().Translate (path.GetPositionAtDistance (u * path.GetPathLength ()));
            cam.GetModelTransform ().Rotate (path.GetRotationAtDistance (u * path.GetPathLength ()));
        },
        [](float t0, float t1, float u)
        {
            return Mathf::Smootherstep (t0, t1, u);
        }
    );
    animation.AddKeyframe("u", 10.0f, 1.0f);
    animation.SetLooping (false);
    animation.Play ();

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

