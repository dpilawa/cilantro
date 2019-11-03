#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/NURBS.h"
#include "math/BSpline.h"
#include "math/Mathf.h"
#include "util/LogMessage.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
	GameScene scene;
	
	GLFWRenderTarget target;
    target.SetResolution (800, 600);
    target.SetDebugVisible (true);
    target.SetVSync (true);
	target.SetResizable (true);
	target.SetFullscreen (false);

	GLFWInputController controller;
    controller.SetGLFWWindow (target.GetWindow ());

    GLRenderer renderer;
	GameLoop game (scene, controller, renderer, target);

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

	controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
	controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    Material& red = scene.AddMaterial (new Material ());
	red.SetShaderModelName ("emissive_shader");
	red.SetEmissiveColor (Vector3f (1.0f, 0.0f, 0.0f));

	Material& green = scene.AddMaterial (new Material ());
	green.SetShaderModelName ("emissive_shader");
	green.SetEmissiveColor (Vector3f (0.0f, 1.0f, 0.0f));

	Material& blue = scene.AddMaterial (new Material ());
	blue.SetShaderModelName ("emissive_shader");
	blue.SetEmissiveColor (Vector3f (0.0f, 0.0f, 1.0f));

	Material& white = scene.AddMaterial (new Material ());
	white.SetShaderModelName ("emissive_shader");
	white.SetEmissiveColor (Vector3f (1.0f, 1.0f, 1.0f));

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(scene.AddGameObject (new ControlledCamera (60.0f, 0.1f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 5.0f, 5.0f).Rotate(-25.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

    MeshObject& x = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    x.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    x.AddVertex (Vector3f (5.0f, 0.0f, 0.0f));
    x.AddLine (0, 1);
    x.SetMaterial (red);

    MeshObject& y = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    y.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    y.AddVertex (Vector3f (0.0f, 5.0f, 0.0f));
    y.AddLine (0, 1);
    y.SetMaterial (green);

	MeshObject& z = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    z.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    z.AddVertex (Vector3f (0.0f, 0.0f, 5.0f));
    z.AddLine (0, 1);
    z.SetMaterial (blue);

    BSpline<Vector3f> spline;
    Vector3f v{1.0f, 0.0f, 0.0f};
    Quaternion q = Mathf::GenRotationQuaternion (Vector3f (0.0f, 1.0f, 0.0f), Mathf::Pi () / 8.0f);
    float h = 0.0f;
    spline.SetDegree (3);
    for (int i=0; i<50; i++)
    {
        v = Mathf::Rotate (v, q);
        h += 0.05f;
        spline.AddControlPoint (v + Vector3f (0.0f, h, 0.0f));
    }

    spline.CalculateKnotVector (KnotVectorType::Clamped);

    if (spline.Validate ())
    {
        MeshObject& line = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
        line.InitSpline (spline, 250, false);
        line.SetMaterial (white);
    }
    else
    {
        LogMessage() << "Spline validation error";
    }

    game.Run ();

	return 0;
}

