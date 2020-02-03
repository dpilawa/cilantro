#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/AnimationObject.h"
#include "scene/Primitives.h"
#include "scene/GameScene.h"
#include "scene/PerspectiveCamera.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/SplinePath.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"

#include "Orbiter.h"

int main (int argc, char* argv [])
{
    GameLoop game;

    GameScene scene (&game);
    game.gameScene = &scene;

    GLFWRenderTarget target (&game, "Test 2", 960, 600, false, true, true);
    game.gameRenderTarget = dynamic_cast<RenderTarget*>(&target);

    GLFWInputController controller (&game, target.GetWindow ());
    game.gameInputController = dynamic_cast<InputController*>(&controller);

    GLRenderer renderer (&game, 960, 600);
    game.gameRenderer = dynamic_cast<Renderer*>(&renderer);

    controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    Material& sunM = scene.AddMaterial (new Material ());
    sunM.SetColor (Vector3f (0.7f, 0.7f, 0.7f));
    sunM.SetEmissiveColor (Vector3f (0.99f, 0.72f, 0.07f));

    Material& earthM = scene.AddMaterial (new Material ());
    earthM.SetColor (Vector3f (0.42f, 0.58f, 0.84f));
    earthM.SetSpecularColor (Vector3f (0.9f, 0.9f, 0.9f));

    Material& moonM = scene.AddMaterial (new Material ());
    moonM.SetDiffuseColor (Vector3f (0.3f, 0.3f, 0.3f));
    moonM.SetSpecularColor (Vector3f (0.0f, 0.0f, 0.0f));

    PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (25.0f, 1.0f, 500.0f)));
    cam.GetModelTransform ().Translate (0.0f, 0.0f, 160.0f);
    scene.SetActiveCamera (&cam);

    MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (sun, 8, true);
    sun.GetModelTransform ().Scale (10.0f);
    sun.SetMaterial (sunM);

    Orbiter& earthOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (365.256f, 50.0f, 0.0f)));
    Orbiter& earthRotor = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (1.0f, 0.0f, -23.0f)));
    earthRotor.SetParentObject (earthOrbit);

    MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (earth, 5, true);
    earth.SetMaterial (earthM);
    earth.SetParentObject (earthRotor);

    Orbiter& moonOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (27.321f, 5.0f, 15.14f)));
    moonOrbit.SetParentObject (earthOrbit);

    MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (moon, 5, true);
    moon.GetModelTransform ().Scale (0.273f);
    moon.SetMaterial (moonM);
    moon.SetParentObject (moonOrbit);

    PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
    sunLight.SetParentObject (sun);
    sunLight.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    sunLight.SetSpecularPower (0.7f);
    sunLight.SetAmbiencePower (0.1f);
    sunLight.SetEnabled (true);

    SplinePath& path = dynamic_cast<SplinePath&>(scene.AddGameObject (new SplinePath ()));
    path.AddWaypoint({0.0f, 80.0f, 260.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({-15.0f, 0.0f, 0.0f})));
    path.AddWaypoint({0.0f, 0.0f, 160.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({0.0f, 0.0f, 0.0f})));
    path.AddWaypoint({-65.0f, 0.0f, 80.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({0.0f, -45.0f, 0.0f})));
    path.AddWaypoint({-160.0f, 0.0f, 0.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({0.0f, -90.0f, 0.0f})));
    path.SetStartTangent({0.0f, 0.0f, -100.0f});
    path.SetEndTangent({150.0f, 0.0f, 0.0f});

    AnimationObject& animation = dynamic_cast<AnimationObject&>(scene.AddGameObject (new AnimationObject ()));
    animation.AddAnimationProperty<float> ("u", 0.0f, 
        [&](float u) 
        {
            Vector3f position = path.GetPositionAtDistance (u * path.GetPathLength ());

            cam.GetModelTransform ().Translate (position);
            //c.GetModelTransform ().Rotate (path.GetRotationAtDistance (u * path.GetPathLength ()));
            cam.GetModelTransform ().Rotate ( Mathf::GenCameraOrientationQuaternion (position, 0.5f * (earth.GetPosition () - sun.GetPosition ()), {0.0f, 1.0f, 0.0f}));

        },
        [](float t0, float t1, float u)
        {
            return Mathf::Lerp (t0, t1, u);
        }
    );
    animation.AddKeyframe("u", 36.0f, 1.0f);
    animation.SetLooping (false);
    animation.Play ();

    game.Run ();

    return 0;
}

