import pycilantro as c
import os

c.Game.Initialize (os.path.dirname (os.path.realpath(__file__)))
scene = c.Game.CreateGameScene ("scene")
renderer = scene.CreateGLFWRenderer (800, 600, True, True, "Test 01", False, True, True)
input = c.Game.CreateGLFWInputController ();

renderer.CreateQuadRenderStage ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS);
renderer.CreateQuadRenderStage ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0).SetRenderStageParameterVector2f ("vInvResolution", c.Vector2f (1.0 / renderer.GetWidth (), 1.0 / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS);
renderer.CreateQuadRenderStage ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1).SetColorAttachmentsFramebufferLink (c.PipelineLink.LINK_PREVIOUS).SetFramebufferEnabled (False);  

input.CreateInputEvent ("exit", c.InputKey.KeyEsc, c.InputTrigger.Press, set())
input.BindInputEvent ("exit", lambda: c.Game.Stop ())

input.CreateInputEvent ("mousemode", c.InputKey.KeySpace, c.InputTrigger.Release, set())
input.BindInputEvent ("mousemode", lambda: input.SetMouseGameMode (not input.IsGameMode ()))

c.Game.Run()


