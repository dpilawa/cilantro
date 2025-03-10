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

    py::class_<c::Game>(m, "Game")
        .def("Run", &c::Game::Run)
        .def("Initialize", &c::Game::Initialize)
        .def("Deinitialize", &c::Game::Deinitialize)
        .def("CreateGameScene", &c::Game::Create<c::GameScene, std::string>)
        .def("CreateGLFWInputController", &c::Game::Create<c::GLFWInputController>);

    py::class_<c::GameScene>(m, "GameScene")
        .def("CreateGLFWRenderer", &c::GameScene::Create<c::GLFWRenderer, unsigned int, unsigned int, bool, bool, std::string, bool, bool, bool>);
        
    py::class_<c::Input<bool>>(m, "InputBool");
    py::class_<c::Input<float>>(m, "InputFloat");

    py::class_<c::GLFWInputController>(m, "GLFWInputController")
        .def("CreateInputEvent", &c::GLFWInputController::CreateInputEvent)
        .def("BindInputEvent", &c::GLFWInputController::BindInputEvent);

    py::class_<c::RenderStage>(m, "RenderStage")
        .def("SetFramebufferEnabled", &c::RenderStage::SetFramebufferEnabled)
        .def("SetColorAttachmentsFramebufferLink", &c::RenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetMultisampleEnabled", &c::RenderStage::SetMultisampleEnabled);

    py::class_<c::QuadRenderStage, c::RenderStage>(m, "QuadRenderStage")
        .def("SetShaderProgram", &c::QuadRenderStage::SetShaderProgram)
        .def("SetColorAttachmentsFramebufferLink", &c::QuadRenderStage::SetColorAttachmentsFramebufferLink)
        .def("SetRenderStageParameterFloat", &c::QuadRenderStage::SetRenderStageParameterFloat)
        .def("SetRenderStageParameterVector2f", &c::QuadRenderStage::SetRenderStageParameterVector2f)
        .def("SetRenderStageParameterVector3f", &c::QuadRenderStage::SetRenderStageParameterVector3f)
        .def("SetRenderStageParameterVector4f", &c::QuadRenderStage::SetRenderStageParameterVector4f);

    py::class_<c::Renderer>(m, "Renderer")
        .def("CreateQuadRenderStage", &c::Renderer::Create<c::QuadRenderStage>)
        .def("GetRenderStageManager", &c::Renderer::GetRenderStageManager)
        .def("GetWidth", &c::Renderer::GetWidth)
        .def("GetHeight", &c::Renderer::GetHeight);

    py::class_<c::GLFWRenderer, c::Renderer>(m, "GLFWRenderer");

// enums

    py::enum_<c::EPipelineLink>(m, "PipelineLink")
        .value("LINK_FIRST", c::EPipelineLink::LINK_FIRST)
        .value("LINK_SECOND", c::EPipelineLink::LINK_SECOND)
        .value("LINK_THIRD", c::EPipelineLink::LINK_THIRD)
        .value("LINK_PREVIOUS", c::EPipelineLink::LINK_PREVIOUS)
        .value("LINK_PREVIOUS_MINUS_1", c::EPipelineLink::LINK_PREVIOUS_MINUS_1)
        .value("LINK_CURRENT", c::EPipelineLink::LINK_CURRENT)
        .value("LINK_LAST", c::EPipelineLink::LINK_LAST);
 
    py::enum_<c::EInputKey>(m, "InputKey")
        .value("KeyEsc", c::EInputKey::KeyEsc)
        .value("KeySpace", c::EInputKey::KeySpace);

    py::enum_<c::EInputTrigger>(m, "InputTrigger")
        .value("Press", c::EInputTrigger::Press)
        .value("Release", c::EInputTrigger::Release);

}