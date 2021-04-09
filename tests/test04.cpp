#include "cilantroengine.h"
#include "game/Game.h"
#include "scene/GameScene.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "graphics/GLPostprocess.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"
#include "system/Timer.h"
#include "ControlledCamera.h"

int main (int argc, char* argv [])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 4", 800, 600, false, true, true);
    GLRenderer renderer (800, 600);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    AssimpModelLoader modelLoader;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, renderTarget, inputController);
    EngineContext::Initialize ();

    modelLoader.Load ("assets/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");

    renderer.AddPostprocess<GLPostprocess> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader");
    renderer.AddPostprocess<GLPostprocess> ("gamma_postprocess").SetShaderProgram ("post_gamma_shader").SetPostprocessParameterFloat ("fGamma", 2.1f);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    gameScene.AddMaterial<PBRMaterial> ("blueMaterial").SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.3f).SetRoughness (0.7f);

    MeshObject& gun = gameScene.AddGameObject<MeshObject> ("gun", "Cerberus00_Fixed", "blueMaterial");
    gun.GetModelTransform ().Scale (0.1f).Rotate (-90.0f, 0.0f, 0.0f);

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.01f, 100.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (0.0f, 0.0f, 20.0f);
    gameScene.SetActiveCamera ("camera");

    PointLight& light1 = gameScene.AddGameObject<PointLight> ("light1");
    light1.SetColor (Vector3f (1.5f, 1.5f, 1.5f));
    light1.SetLinearAttenuationFactor (0.0f).SetQuadraticAttenuationFactor (1.0f);
    light1.GetModelTransform ().Translate (Vector3f (20.0f, 20.0f, 20.0f));
    light1.SetEnabled (true);

    DirectionalLight& light2 = gameScene.AddGameObject<DirectionalLight> ("light2");
    light2.GetModelTransform ().Rotate (135.0f, 45.0f, 0.0f);
    light2.SetColor (Vector3f (5.0f, 5.0f, 5.0f));
    light2.SetEnabled (true);

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

