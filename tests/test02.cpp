#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/AnimationObject.h"
#include "scene/Primitives.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
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

    Texture earthT;
    earthT.Load ("textures/2k_earth_daymap.jpg");
    Texture earthTSpec;
    earthTSpec.Load ("textures/2k_earth_specular_map.png");

    Texture moonT;
    moonT.Load ("textures/2k_moon.jpg");

    Texture sunT;
    sunT.Load ("textures/2k_sun.jpg");

    PhongMaterial& sunM = dynamic_cast<PhongMaterial&>(scene.AddMaterial (new PhongMaterial ()));
    //sunM.SetEmissive (Vector3f (0.99f, 0.72f, 0.07f));
    sunM.SetEmissive (&sunT);

    PhongMaterial& earthM = dynamic_cast<PhongMaterial&>(scene.AddMaterial (new PhongMaterial ()));
    //earthM.SetDiffuse (Vector3f (0.42f, 0.58f, 0.84f));
    //earthM.SetSpecular (Vector3f (1.0f, 1.0f, 1.0f));
    earthM.SetDiffuse (&earthT);
    earthM.SetSpecular (&earthTSpec);
    earthM.SetSpecularShininess (32.0f);

    PhongMaterial& moonM = dynamic_cast<PhongMaterial&>(scene.AddMaterial (new PhongMaterial ()));
    //moonM.SetDiffuse (Vector3f (0.3f, 0.3f, 0.3f));
    //moonM.SetSpecular (Vector3f (0.0f, 0.0f, 0.0f));
    moonM.SetDiffuse (&moonT);
    moonM.SetSpecularShininess (1.0f);
    moonM.SetSpecular (Vector3f(0.2f, 0.2f, 0.2f));

    PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (25.0f, 1.0f, 500.0f)));
    cam.GetModelTransform ().Translate (0.0f, 0.0f, 160.0f);
    scene.SetActiveCamera (&cam);

    MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (sun, 8);
    sun.GetModelTransform ().Scale (10.0f);
    sun.SetMaterial (sunM);

    Orbiter& earthOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (sun, 1.0f, 23.5f, 365.256f, 50.0f, 0.0f)));
    MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (earth, 8);
    earth.SetMaterial (earthM);
    earth.SetParentObject (earthOrbit);
    earth.GetModelTransform ().Scale (3.0f);
    earth.SetSmoothNormals (true);

    Orbiter& moonOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (earth, 27.321f, -6.68f, 27.321f, 20.0f, -5.14f)));
    MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (moon, 8);
    moon.GetModelTransform ().Scale (0.273f * 5.0f);
    moon.SetMaterial (moonM);
    moon.SetParentObject (moonOrbit);
    moon.SetSmoothNormals (true);

    PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
    sunLight.SetParentObject (sun);
    sunLight.SetColor (Vector3f (1.1f, 1.0f, 1.0f));
    sunLight.SetEnabled (true);

    SplinePath& path = dynamic_cast<SplinePath&>(scene.AddGameObject (new SplinePath ()));
    path.AddWaypoint({0.0f, 80.0f, 260.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({-15.0f, 0.0f, 0.0f})));
    path.AddWaypoint({0.0f, 0.0f, 140.0f}, Mathf::EulerToQuaterion(Mathf::Deg2Rad ({0.0f, 0.0f, 0.0f})));
    path.SetStartTangent({0.0f, -2.0f, -1.0f});
    path.SetEndTangent({0.0f, 0.0f, -1.0f});

    AnimationObject& animation = dynamic_cast<AnimationObject&>(scene.AddGameObject (new AnimationObject ()));
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

    return 0;
}

