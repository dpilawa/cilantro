#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/LinearPath.h"
#include "scene/SplinePath.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "graphics/GLPostprocess.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "util/LogMessage.h"

#include "ControlledCamera.h"
#include "RotatingObject.h"

#include <iostream>

int main (int argc, char* argv [])
{
    GameLoop game;

    GameScene scene (&game);
    game.gameScene = &scene;
    
    GLFWRenderTarget target (&game, "Test 1", 800, 600, false, true, true);
    game.gameRenderTarget = dynamic_cast<RenderTarget*>(&target);

    GLFWInputController controller (&game, target.GetWindow ());
    game.gameInputController = dynamic_cast<InputController*>(&controller);

    GLRenderer renderer (&game, 800, 600);
    game.gameRenderer = dynamic_cast<Renderer*>(&renderer);

    GLPostprocess gamma (&renderer, &renderer.GetShaderProgram("post_gamma_shader"));
    gamma.SetPostprocessParameterFloat ("fGamma", 2.2f);
    renderer.AddPostprocess (&gamma);

    controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    Material& green = scene.AddMaterial (new Material ());
    green.SetShaderProgram ("blinnphong_shader");
    green.SetColor (Vector3f (0.1f, 0.4f, 0.1f));
    green.SetAmbientColor (Vector3f (0.5f, 0.5f, 0.5f));
    green.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (32.0f);

    Material& red = scene.AddMaterial (new Material ());
    red.SetShaderProgram ("phong_shader");
    red.SetColor (Vector3f (0.75f, 0.1f, 0.1f));
    red.SetSpecularColor (Vector3f (1.0f, 0.0f, 0.0f)).SetSpecularShininess (8.0f);

    Material& lampM = scene.AddMaterial (new Material ());
    lampM.SetEmissiveColor (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuseColor (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(scene.AddGameObject (new ControlledCamera (60.0f, 0.1f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 1.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

    MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (cube);
    cube.SetMaterial (red);
    cube.GetModelTransform ().Scale (0.5f);

    MeshObject& lamp = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (lamp, 3, true);
    lamp.GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);
    lamp.SetMaterial (lampM);

    MeshObject& floor = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    //Primitives::GenerateCube (floor);
    Primitives::GeneratePlane (floor, false);
    floor.GetModelTransform ().Scale (2.5f, 0.1f, 2.5f).Translate (0.0f, -1.0f, 0.0f);
    floor.SetMaterial (green);

    PointLight& light1 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
    light1.SetParentObject (lamp);
    light1.SetColor (Vector3f (0.4f, 0.4f, 0.4f));
    light1.SetSpecularPower (2.0f);
    light1.SetAmbiencePower (0.0f);
    light1.SetLinearAttenuationFactor (0.2f).SetQuadraticAttenuationFactor (0.2f);
    light1.SetEnabled (true);

    DirectionalLight& light2 = dynamic_cast<DirectionalLight&>(scene.AddGameObject (new DirectionalLight ()));
    light2.GetModelTransform ().Rotate (135.0f, -45.0f, 0.0f);
    light2.SetColor (Vector3f (0.2f, 0.2f, 0.2f));
    light2.SetSpecularPower (1.1f);
    light2.SetAmbiencePower (0.0f);
    light2.SetEnabled (true);

    SpotLight& light3 = dynamic_cast<SpotLight&>(scene.AddGameObject (new SpotLight ()));
    light3.GetModelTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
    light3.SetColor (Vector3f (0.3f, 0.2f, 0.2f));
    light3.SetSpecularPower (0.7f);
    light3.SetInnerCutoff (5.0f);
    light3.SetOuterCutoff (12.0f);
    light3.SetEnabled (true);

    SplinePath& lp = dynamic_cast<SplinePath&> (scene.AddGameObject (new SplinePath ()));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({-2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint (2, {-2.0f, 2.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));

    lp.SetStartTangent ({-2.0f, 0.0f, 2.0f});
    lp.SetEndTangent ({-2.0f, 0.0f, 2.0f});

    lp.GetModelTransform ().Rotate ({0.0f, 0.0f, -15.0f});

    AnimationObject& lightAnimation = dynamic_cast<AnimationObject&> (scene.AddGameObject (new AnimationObject ()));

    lightAnimation.AddAnimationProperty<float> (
        "t", 0.0f,
        [&] (float t) {
            lamp.GetModelTransform ().Translate (lp.GetPositionAtDistance (lp.GetPathLength () * t));
            lamp.GetModelTransform ().Rotate (lp.GetRotationAtDistance (lp.GetPathLength () * t));
        },
        [] (float t0, float t1, float u) { return Mathf::Lerp (t0, t1, u); });

    lightAnimation.AddKeyframe<float> ("t", 5.0f, 1.0f);
    lightAnimation.SetLooping (true);
    lightAnimation.Play ();

    game.Run ();

    return 0;
}

