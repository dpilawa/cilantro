#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "system/Game.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/RenderStage.h"
#include "graphics/QuadRenderStage.h"
#include "input/Input.h"
#include "input/GLFWInputController.h"
#include "scene/GameScene.h"
#include "resource/ResourceManager.h"

namespace py = pybind11;

// Create Python bindings

PYBIND11_MODULE(pycilantro, m) {

// vectors

    py::class_<Vector2f>(m, "Vector2f")
        .def(py::init<float, float>());

// classes

    py::class_<CGame>(m, "Game")
        .def("Run", &CGame::Run)
        .def("Initialize", &CGame::Initialize)
        .def("Deinitialize", &CGame::Deinitialize)
        .def("CreateGameScene", &CGame::Create<CGameScene, std::string>)
        .def("CreateGLFWInputController", &CGame::Create<GLFWInputController>);

    py::class_<CGameScene>(m, "GameScene")
        .def("CreateGLFWRenderer", &CGameScene::Create<CGLFWRenderer, unsigned int, unsigned int, bool, bool, std::string, bool, bool, bool>);
        
    py::class_<Input<bool>>(m, "InputBool");
    py::class_<Input<float>>(m, "InputFloat");

    py::class_<GLFWInputController>(m, "GLFWInputController")
        .def("CreateInputEvent", &GLFWInputController::CreateInputEvent)
        .def("BindInputEvent", &GLFWInputController::BindInputEvent);

    py::class_<CRenderStage>(m, "RenderStage")
        .def("SetFramebufferEnabled", &CRenderStage::SetFramebufferEnabled)
        .def("SetColorAttachmentsFramebufferLink", &CRenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetMultisampleEnabled", &CRenderStage::SetMultisampleEnabled);

    py::class_<CQuadRenderStage, CRenderStage>(m, "QuadRenderStage")
        .def("SetShaderProgram", &CQuadRenderStage::SetShaderProgram)
        .def("SetColorAttachmentsFramebufferLink", &CQuadRenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetRenderStageParameterFloat", &CQuadRenderStage::SetRenderStageParameterFloat)
        .def("SetRenderStageParameterVector2f", &CQuadRenderStage::SetRenderStageParameterVector2f)
        .def("SetRenderStageParameterVector3f", &CQuadRenderStage::SetRenderStageParameterVector3f)
        .def("SetRenderStageParameterVector4f", &CQuadRenderStage::SetRenderStageParameterVector4f);

    py::class_<CRenderer>(m, "Renderer")
        .def("CreateQuadRenderStage", &CRenderer::Create<CQuadRenderStage>)
        .def("GetRenderStageManager", &CRenderer::GetRenderStageManager)
        .def("GetWidth", &CRenderer::GetWidth)
        .def("GetHeight", &CRenderer::GetHeight);

    py::class_<CGLFWRenderer, CRenderer>(m, "GLFWRenderer");

// enums

    py::enum_<EPipelineLink>(m, "PipelineLink")
        .value("LINK_FIRST", EPipelineLink::LINK_FIRST)
        .value("LINK_SECOND", EPipelineLink::LINK_SECOND)
        .value("LINK_THIRD", EPipelineLink::LINK_THIRD)
        .value("LINK_PREVIOUS", EPipelineLink::LINK_PREVIOUS)
        .value("LINK_PREVIOUS_MINUS_1", EPipelineLink::LINK_PREVIOUS_MINUS_1)
        .value("LINK_CURRENT", EPipelineLink::LINK_CURRENT)
        .value("LINK_LAST", EPipelineLink::LINK_LAST);
 
    py::enum_<InputKey>(m, "InputKey")
        .value("KeyEsc", InputKey::KeyEsc)
        .value("KeySpace", InputKey::KeySpace);

    py::enum_<InputTrigger>(m, "InputTrigger")
        .value("Press", InputTrigger::Press)
        .value("Release", InputTrigger::Release);

}