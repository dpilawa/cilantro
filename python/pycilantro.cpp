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
namespace c = cilantro;

// Create Python bindings

PYBIND11_MODULE(pycilantro, m) {

// vectors

    py::class_<c::Vector2f>(m, "Vector2f")
        .def(py::init<float, float>());

// classes

    py::class_<c::CGame>(m, "Game")
        .def("Run", &c::CGame::Run)
        .def("Initialize", &c::CGame::Initialize)
        .def("Deinitialize", &c::CGame::Deinitialize)
        .def("CreateGameScene", &c::CGame::Create<c::CGameScene, std::string>)
        .def("CreateGLFWInputController", &c::CGame::Create<c::GLFWInputController>);

    py::class_<c::CGameScene>(m, "GameScene")
        .def("CreateGLFWRenderer", &c::CGameScene::Create<c::CGLFWRenderer, unsigned int, unsigned int, bool, bool, std::string, bool, bool, bool>);
        
    py::class_<c::Input<bool>>(m, "InputBool");
    py::class_<c::Input<float>>(m, "InputFloat");

    py::class_<c::GLFWInputController>(m, "GLFWInputController")
        .def("CreateInputEvent", &c::GLFWInputController::CreateInputEvent)
        .def("BindInputEvent", &c::GLFWInputController::BindInputEvent);

    py::class_<c::CRenderStage>(m, "RenderStage")
        .def("SetFramebufferEnabled", &c::CRenderStage::SetFramebufferEnabled)
        .def("SetColorAttachmentsFramebufferLink", &c::CRenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetMultisampleEnabled", &c::CRenderStage::SetMultisampleEnabled);

    py::class_<c::CQuadRenderStage, c::CRenderStage>(m, "QuadRenderStage")
        .def("SetShaderProgram", &c::CQuadRenderStage::SetShaderProgram)
        .def("SetColorAttachmentsFramebufferLink", &c::CQuadRenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetRenderStageParameterFloat", &c::CQuadRenderStage::SetRenderStageParameterFloat)
        .def("SetRenderStageParameterVector2f", &c::CQuadRenderStage::SetRenderStageParameterVector2f)
        .def("SetRenderStageParameterVector3f", &c::CQuadRenderStage::SetRenderStageParameterVector3f)
        .def("SetRenderStageParameterVector4f", &c::CQuadRenderStage::SetRenderStageParameterVector4f);

    py::class_<c::CRenderer>(m, "Renderer")
        .def("CreateQuadRenderStage", &c::CRenderer::Create<c::CQuadRenderStage>)
        .def("GetRenderStageManager", &c::CRenderer::GetRenderStageManager)
        .def("GetWidth", &c::CRenderer::GetWidth)
        .def("GetHeight", &c::CRenderer::GetHeight);

    py::class_<c::CGLFWRenderer, c::CRenderer>(m, "GLFWRenderer");

// enums

    py::enum_<c::EPipelineLink>(m, "PipelineLink")
        .value("LINK_FIRST", c::EPipelineLink::LINK_FIRST)
        .value("LINK_SECOND", c::EPipelineLink::LINK_SECOND)
        .value("LINK_THIRD", c::EPipelineLink::LINK_THIRD)
        .value("LINK_PREVIOUS", c::EPipelineLink::LINK_PREVIOUS)
        .value("LINK_PREVIOUS_MINUS_1", c::EPipelineLink::LINK_PREVIOUS_MINUS_1)
        .value("LINK_CURRENT", c::EPipelineLink::LINK_CURRENT)
        .value("LINK_LAST", c::EPipelineLink::LINK_LAST);
 
    py::enum_<c::InputKey>(m, "InputKey")
        .value("KeyEsc", c::InputKey::KeyEsc)
        .value("KeySpace", c::InputKey::KeySpace);

    py::enum_<c::InputTrigger>(m, "InputTrigger")
        .value("Press", c::InputTrigger::Press)
        .value("Release", c::InputTrigger::Release);

}