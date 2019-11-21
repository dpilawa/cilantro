#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/NURBS.h"
#include "math/BSpline.h"
#include "math/CubicHermite.h"
#include "math/Bezier.h"
#include "math/Mathf.h"
#include "util/LogMessage.h"

#include "ControlledCamera.h"

#include <iostream>
#include <cmath>

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
    cam.GetModelTransform ().Translate (5.0f, 5.0f, 5.0f).Rotate(-30.0f, 45.0f, 0.0f);
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

    // Bspline
    BSpline<Vector3f, 3> bspline;
    Vector3f v {1.0f, 0.0f, 0.0f};
    Quaternion q = Mathf::GenRotationQuaternion (Vector3f (0.0f, 1.0f, 0.0f), Mathf::Pi () / 8.0f);
    float h = 0.0f;
    for (int i=0; i<50; i++)
    {
        v = Mathf::Rotate (v, q);
        h += 0.05f;
        bspline.AddControlPoint (v + Vector3f (0.0f, h, 0.0f));
    }

    bspline.CalculateKnotVector (KnotVectorType::Clamped);

    if (bspline.Validate ())
    {
        MeshObject& bsplineObject = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
        bsplineObject.InitCurve (bspline, 250).SetMaterial (white);
    }
    else
    {
        LogMessage() << "B-Spline validation error";
    }

    // NURBS
    NURBS<Vector3f, 2> nurbs;
    float sqrt2by2 = std::sqrt (2.0f) / 2.0f;
    nurbs.AddControlPoints ({{1.f, 0.f, 0.f}, {1.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {-1.f, 1.f, 0.f}, {-1.f, 0.f, 0.f}, {-1.f, -1.f, 0.f}, {0.f, -1.f, 0.f}, {1.f, -1.f, 0.f}, {1.f, 0.f, 0.f}});
    nurbs.SetKnotVector ({0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.5f, 0.5f, 0.75f, 0.75f, 1.0f, 1.0f, 1.0f});
    nurbs.SetWeights ({1.0f, sqrt2by2, 1.0f, sqrt2by2, 1.0f, sqrt2by2, 1.0f, sqrt2by2, 1.0f});

    if (nurbs.Validate ())
    {
        MeshObject& nurbsObject = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
        nurbsObject.InitCurve (nurbs, 64).SetMaterial (white);
        nurbsObject.GetModelTransform ().Rotate ({0.0f, 90.0f, 0.0f}).Translate ({0.0f, 0.0f, 3.0f});
    }
    else
    {
        LogMessage() << "NURBS validation error";
    }

    // Hermite curve
    CubicHermite<Vector3f> hc (Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(-1.0f, 5.0f, 0.0f), Vector3f(-1.0f, 5.0f, 0.0f));
    MeshObject& hcObject = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    hcObject.InitCurve (hc, 100);
    hcObject.SetMaterial (white).GetModelTransform ().Translate ({3.0f, 0.0f, 0.0f});

    // Bezier curve
    Bezier<Vector3f, 3> b;
    b.SetPoints ({{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}});
    MeshObject& bezierObject = dynamic_cast<MeshObject&> (scene.AddGameObject (new MeshObject (MeshType::Lines)));
    bezierObject.InitCurve (b, 100);
    bezierObject.SetMaterial (white).GetModelTransform ().Translate ({3.0f, 0.0f, 2.0f});

    game.Run ();

	return 0;
}

