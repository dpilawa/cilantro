import os
import sys
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../build"))
import pycilantro as c
import os

class ControlledCamera (c.PerspectiveCamera):
    def __init__(self, game_scene, fov, near, far, speed, sensitivity):
        super().__init__(game_scene, fov, near, far)
        self.speed = speed
        self.sensitivity = sensitivity

    def Initialize(self):
        controller = c.Game.GetInputController()

        controller.CreateInputAxis("moveforward", c.InputKey.KeyW, set(), 1.0)
        controller.CreateInputAxis("moveforward", c.InputKey.KeyS, set(), -1.0)

        controller.CreateInputAxis("moveright", c.InputKey.KeyD, set(), 1.0)
        controller.CreateInputAxis("moveright", c.InputKey.KeyA, set(), -1.0)

        controller.CreateInputAxis("camerapitch", c.InputAxis.MouseY, 1.0)
        controller.CreateInputAxis("camerayaw", c.InputAxis.MouseX, 1.0)

        controller.BindInputAxis("moveright", lambda a: self.MoveRight(a))
        controller.BindInputAxis("moveforward", lambda a: self.MoveForward(a))
        controller.BindInputAxis("camerapitch", lambda a: self.PitchBy(a))
        controller.BindInputAxis("camerayaw", lambda a: self.YawBy(a))

    def MoveRight(self, offset):
        self.GetLocalTransform ().TranslateBy (self.GetRight () * offset * self.speed)

    def MoveForward(self, offset):
        self.GetLocalTransform ().TranslateBy (-self.GetForward () * offset * self.speed)

    def YawBy(self, angle):
        self.GetLocalTransform ().RotateBy (0.0, angle * self.sensitivity, 0.0)

    def PitchBy(self, angle):
        self.GetLocalTransform ().RotateBy (angle * self.sensitivity, 0.0, 0.0)

c.Game.Initialize (os.path.dirname (os.path.realpath(__file__)))
scene = c.Game.CreateGameScene ("scene")
renderer = scene.CreateGLFWRenderer (800, 600, True, True, "Test 01", False, True, True)
input = c.Game.CreateGLFWInputController ()

renderer.CreateQuadRenderStage ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS);
renderer.CreateQuadRenderStage ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0).SetRenderStageParameterVector2f ("vInvResolution", c.Vector2f (1.0 / renderer.GetWidth (), 1.0 / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS)
renderer.CreateQuadRenderStage ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1).SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS).SetFramebufferEnabled (False)

input.CreateInputEvent ("exit", c.InputKey.KeyEsc, c.InputTrigger.Press, set())
input.BindInputEvent ("exit", lambda: c.Game.Stop ())

input.CreateInputEvent ("mousemode", c.InputKey.KeySpace, c.InputTrigger.Release, set())
input.BindInputEvent ("mousemode", lambda: input.SetMouseGameMode (not input.IsGameMode ()))

c.Game.GetResourceManager ().LoadTexture ("tAlbedoMetal", "textures/scuffed-metal1_albedo.png")
c.Game.GetResourceManager ().LoadTexture ("tMetalnessMetal", "textures/scuffed-metal1_metallic.png")
c.Game.GetResourceManager ().LoadTexture ("tNormalMetal", "textures/scuffed-metal1_normal-dx.png")
c.Game.GetResourceManager ().LoadTexture ("tRoughnessMetal", "textures/scuffed-metal1_roughness.png")
c.Game.GetResourceManager ().LoadTexture ("tAOMetal", "textures/scuffed-metal1_ao.png")

c.Game.GetResourceManager ().LoadTexture ("tAlbedoGold", "textures/Metal007_1K_Color.png")
c.Game.GetResourceManager ().LoadTexture ("tMetalnessGold", "textures/Metal007_1K_Metalness.png")
c.Game.GetResourceManager ().LoadTexture ("tNormalGold", "textures/Metal007_1K_Normal.png")
c.Game.GetResourceManager ().LoadTexture ("tRoughnessGold", "textures/Metal007_1K_Roughness.png")

scene.CreatePBRMaterial("greenMaterial").SetAlbedo(c.Vector3f(0.1, 0.4, 0.1)).SetRoughness(0.1).SetMetallic(0.6).SetNormal("tNormalMetal")
scene.CreatePBRMaterial("redMaterial").SetAlbedo("tAlbedoMetal").SetMetallic("tMetalnessMetal").SetRoughness("tRoughnessMetal").SetNormal("tNormalMetal").SetAO("tAOMetal")
scene.CreatePBRMaterial("goldMaterial").SetAlbedo("tAlbedoGold").SetMetallic("tMetalnessGold").SetRoughness("tRoughnessGold").SetNormal("tNormalGold")
scene.CreatePBRMaterial("blueMaterial").SetAlbedo(c.Vector3f(0.02, 0.29, 0.53)).SetMetallic(0.0).SetRoughness(0.8)
scene.CreatePhongMaterial("lampMaterial").SetEmissive(c.Vector3f(0.9, 0.9, 0.9)).SetDiffuse(c.Vector3f(0.2, 0.2, 0.2))

cam = ControlledCamera (scene, 60.0, 0.5, 10.0, 0.2, 0.1)
scene.AddGameObject ("camera", cam)
cam.Initialize ()
cam.GetLocalTransform ().Translate (5.0, 2.5, 5.0).Rotate (-20.0, 45.0, 0.0)
scene.SetActiveCamera ("camera")

cubeMesh = c.Game.GetResourceManager ().CreateMesh ("cubeMesh")
c.Primitives.GenerateCube (cubeMesh)
cube = scene.CreateMeshObject ("cube", "cubeMesh", "redMaterial")
cube.GetLocalTransform ().Scale (0.5).Translate (0.0, 1.1, 0.0)

coneMesh = c.Game.GetResourceManager().CreateMesh("coneMesh").SetSmoothNormals(False)
c.Primitives.GenerateCone(coneMesh, 32)
cone = scene.CreateMeshObject("cone", "coneMesh", "goldMaterial")
cone.GetLocalTransform().Translate(-1.5, 0.5, 1.0).Scale(0.5)

cylinderMesh = c.Game.GetResourceManager().CreateMesh("cylinderMesh").SetSmoothNormals(False)
c.Primitives.GenerateCylinder(cylinderMesh, 32)
cylinder = scene.CreateMeshObject("cylinder", "cylinderMesh", "blueMaterial")
cylinder.GetLocalTransform().Rotate(90.0, 12.5, 0.0).Translate(1.7, 0.5, 0.7).Scale(0.5)

lampMesh = c.Game.GetResourceManager().CreateMesh("lampMesh")
c.Primitives.GenerateSphere(lampMesh, 3)
lamp = scene.CreateMeshObject("lamp", "lampMesh", "lampMaterial")
lamp.GetLocalTransform().Scale(0.1, 0.1, 0.1).Translate(1.0, 0.75, 1.0)

floorMesh = c.Game.GetResourceManager().CreateMesh("floorMesh")
c.Primitives.GenerateCube(floorMesh)
floor = scene.CreateMeshObject("floor", "floorMesh", "greenMaterial")
floor.GetLocalTransform().Scale(2.5, 0.05, 2.5).Translate(0.0, -0.05, 0.0)

light1 = scene.CreatePointLight ("light1")
light1.SetParentObject ("lamp")
light1.SetColor (c.Vector3f (1.5, 1.5, 1.5))
light1.SetLinearAttenuationFactor (0.0).SetQuadraticAttenuationFactor (1.0)
light1.SetEnabled (True)

light2 = scene.CreateDirectionalLight("light2")
light2.GetLocalTransform().Rotate(45.0, -135.0, 0.0)
light2.SetColor(c.Vector3f(2.7, 2.7, 2.7))
light2.SetEnabled(True)

light2a = scene.CreateDirectionalLight("light2a")
light2a.GetLocalTransform().Rotate(45.0, 135.0, 0.0)
light2a.SetColor(c.Vector3f(2.7, 2.7, 2.7))
light2a.SetEnabled(True)

light3 = scene.CreateSpotLight("light3")
light3.GetLocalTransform().Translate(2.0, 10.0, 0.0).Rotate(90.0, 0.0, 0.0)
light3.SetColor(c.Vector3f(2.7, 2.7, 2.7))
light3.SetInnerCutoff(5.0)
light3.SetOuterCutoff(12.0)
light3.SetEnabled(True)

lp = scene.CreateSplinePath("splinepath")
lp.AddWaypoint(c.Vector3f(2.0, 0.0, 2.0), c.Mathf.EulerToQuaternion(c.Vector3f(0.0, 0.0, 0.0)))
lp.AddWaypoint(c.Vector3f(-2.0, 0.0, 2.0), c.Mathf.EulerToQuaternion(c.Vector3f(0.0, 0.0, 0.0)))
lp.AddWaypoint(c.Vector3f(2.0, 0.0, -2.0), c.Mathf.EulerToQuaternion(c.Vector3f(0.0, 0.0, 0.0)))
lp.AddWaypoint(c.Vector3f(2.0, 0.0, 2.0), c.Mathf.EulerToQuaternion(c.Vector3f(0.0, 0.0, 0.0)))
lp.AddWaypoint(2, c.Vector3f(-2.0, 2.0, -2.0), c.Mathf.EulerToQuaternion(c.Vector3f(0.0, 0.0, 0.0)))

lp.SetStartTangent(c.Vector3f(-2.0, 0.0, 2.0))
lp.SetEndTangent(c.Vector3f(-2.0, 0.0, 2.0))

lp.GetLocalTransform().Rotate(c.Vector3f(0.0, 0.0, -15.0)).Translate(c.Vector3f(0.0, 1.0, 0.0))

lightAnimation = scene.CreateAnimationObject ("lightAnimation")

def update_lamp (t):
    lamp.GetLocalTransform ().Translate (lp.GetPositionAtDistance (lp.GetPathLength () * t))
    lamp.GetLocalTransform ().Rotate (lp.GetRotationAtDistance (lp.GetPathLength () * t))

lightAnimation.AddAnimationProperty ("t", 0.0, update_lamp, lambda t0, t1, u: c.Mathf.Lerp(t0, t1, u))
lightAnimation.AddKeyframe ("t", 5.0, 1.0)
lightAnimation.SetLooping (True)
lightAnimation.Play ()

c.Game.Run ()
c.Game.Deinitialize ()