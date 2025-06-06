#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>


#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/RenderStage.h"
#include "graphics/SurfaceRenderStage.h"
#include "input/Input.h"
#include "input/GLFWInputController.h"
#include "resource/Resource.h"
#include "resource/Texture.h"
#include "resource/Mesh.h"
#include "resource/LoadableResource.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "scene/Camera.h"
#include "scene/PerspectiveCamera.h"
#include "scene/Transform.h"
#include "scene/Material.h"
#include "scene/PBRMaterial.h"
#include "scene/PhongMaterial.h"
#include "scene/Primitives.h"
#include "scene/Light.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/Path.h"
#include "scene/LinearPath.h"
#include "scene/SplinePath.h"
#include "scene/AnimationObject.h"
#include "scene/AnimationProperty.h"


namespace py = pybind11;
namespace c = cilantro;

// Create Python bindings

PYBIND11_MODULE(pycilantro, m) {

// vectors

    py::class_<c::Vector2f>(m, "Vector2f")
        .def(py::init<float, float>())
        .def(py::init<std::initializer_list<float>>())
        .def("Dim", &c::Vector2f::Dim)
        .def("__getitem__", [](const c::Vector2f &v, unsigned int i) { return v[i]; })
        .def("__setitem__", [](c::Vector2f &v, unsigned int i, float val) { v[i] = val; })
        .def(py::self *= float())
        .def(py::self /= float())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self + c::Vector2f())
        .def(py::self - c::Vector2f())
        .def(-py::self)
        .def("__eq__", &c::Vector2f::operator==);

    py::class_<c::Vector3f>(m, "Vector3f")
        .def(py::init<float, float, float>())
        .def(py::init<const c::Vector4f&>())
        .def(py::init<std::initializer_list<float>>())
        .def("Dim", &c::Vector3f::Dim)
        .def("__getitem__", [](const c::Vector3f &v, unsigned int i) { return v[i]; })
        .def("__setitem__", [](c::Vector3f &v, unsigned int i, float val) { v[i] = val; })
        .def(py::self *= float())
        .def(py::self /= float())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self + c::Vector3f())
        .def(py::self - c::Vector3f())
        .def(-py::self)
        .def("__eq__", &c::Vector3f::operator==);

    py::class_<c::Vector4f>(m, "Vector4f")
        .def(py::init<float, float, float, float>())
        .def(py::init<float, float, float>())
        .def(py::init<const c::Vector3f&, float>())
        .def(py::init<std::initializer_list<float>>())
        .def("Dim", &c::Vector4f::Dim)
        .def("__getitem__", [](const c::Vector4f &v, unsigned int i) { return v[i]; })
        .def("__setitem__", [](c::Vector4f &v, unsigned int i, float val) { v[i] = val; })
        .def(py::self *= float())
        .def(py::self /= float())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self + c::Vector4f())
        .def(py::self - c::Vector4f())
        .def(-py::self)
        .def("__eq__", &c::Vector4f::operator==);

    py::class_<c::Quaternion>(m, "Quaternion")
        .def(py::init<>())
        .def(py::init<float, float, float, float>())
        .def(py::init<float, c::Vector3f>())
        .def(py::init<const c::Quaternion&>())
        .def(py::self *= float())
        .def(py::self += c::Quaternion())
        .def(py::self -= c::Quaternion())
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self + py::self)
        .def(-py::self);

    py::class_<c::Matrix3f>(m, "Matrix3f")
        .def(py::init<>())
        .def(py::init<std::initializer_list<float>>())
        .def(py::init<const c::Matrix3f&>())
        .def(py::init<const c::Matrix4f&>())
        .def(py::init<const c::Vector3f&, const c::Vector3f&, const c::Vector3f&>())
        .def("InitIdentity", &c::Matrix3f::InitIdentity)
        .def("__getitem__", [](const c::Matrix3f &m, unsigned int index) { return m[index];})
        .def("__setitem__", [](c::Matrix3f &m, unsigned int index, float value) {m[index][0] = value;})
        .def(py::self *= py::self)
        .def(py::self * c::Vector3f())
        .def(py::self *= float())
        .def(py::self * py::self)
        .def(py::self * float())
        .def(float() * py::self);

    py::class_<c::Matrix4f>(m, "Matrix4f")
        .def(py::init<>())
        .def(py::init<std::initializer_list<float>>())
        .def(py::init<const c::Matrix4f&>())
        .def(py::init<const c::Vector4f&, const c::Vector4f&, const c::Vector4f&, const c::Vector4f&>())
        .def("InitIdentity", &c::Matrix4f::InitIdentity)
        .def("__getitem__", [](const c::Matrix4f &m, unsigned int index) { return m[index];})
        .def("__setitem__", [](c::Matrix4f &m, unsigned int index, float value) {m[index][0] = value;})
        .def(py::self *= py::self)
        .def(py::self * c::Vector4f())
        .def(py::self *= float())
        .def(py::self * py::self)
        .def(py::self * float())
        .def(float() * py::self);

    py::class_<c::Mathf>(m, "Mathf")
        .def_static("Pi", &c::Mathf::Pi)
        .def_static("VeryClose", &c::Mathf::VeryClose)
        .def_static("Length", py::overload_cast<const c::Vector3f&>(&c::Mathf::Length))
        .def_static("Length", py::overload_cast<const c::Vector4f&>(&c::Mathf::Length))
        .def_static("Normalize", py::overload_cast<const c::Vector3f&>(&c::Mathf::Normalize))
        .def_static("Normalize", py::overload_cast<const c::Vector4f&>(&c::Mathf::Normalize))
        .def_static("Dot", py::overload_cast<const c::Vector3f&, const c::Vector3f&>(&c::Mathf::Dot))
        .def_static("Cross", &c::Mathf::Cross)
        .def_static("CartesianToHomogenous", py::overload_cast<const c::Vector3f&, float>(&c::Mathf::CartesianToHomogenous))
        .def_static("CartesianToHomogenous", py::overload_cast<const std::vector<c::Vector3f>&, const std::vector<float>&>(&c::Mathf::CartesianToHomogenous))
        .def_static("HomogenousToCartesianPerspective", &c::Mathf::HomogenousToCartesianPerspective)
        .def_static("HomogenousToCartesianTruncate", &c::Mathf::HomogenousToCartesianTruncate)
        .def_static("GetHomogenousWeight", &c::Mathf::GetHomogenousWeight)
        .def_static("Norm", &c::Mathf::Norm)
        .def_static("Dot", py::overload_cast<const c::Quaternion&, const c::Quaternion&>(&c::Mathf::Dot))
        .def_static("Normalize", py::overload_cast<const c::Quaternion&>(&c::Mathf::Normalize))
        .def_static("Conjugate", &c::Mathf::Conjugate)
        .def_static("Invert", py::overload_cast<const c::Quaternion&>(&c::Mathf::Invert))
        .def_static("Product", &c::Mathf::Product)
        .def_static("GenRotationQuaternion", &c::Mathf::GenRotationQuaternion)
        .def_static("Rotate", py::overload_cast<const c::Vector3f&, const c::Quaternion&>(&c::Mathf::Rotate))
        .def_static("Rotate", py::overload_cast<const c::Vector3f&, const c::Vector3f&, float>(&c::Mathf::Rotate))
        .def_static("QuaternionToEuler", &c::Mathf::QuaternionToEuler)
        .def_static("EulerToQuaternion", &c::Mathf::EulerToQuaternion)
        .def_static("Clamp", &c::Mathf::Clamp)
        .def_static("Step", &c::Mathf::Step)
        .def_static("Smoothstep", &c::Mathf::Smoothstep)
        .def_static("Smootherstep", &c::Mathf::Smootherstep)
        .def_static("Lerp", py::overload_cast<float, float, float>(&c::Mathf::Lerp))
        .def_static("Lerp", py::overload_cast<const c::Vector3f&, const c::Vector3f&, float>(&c::Mathf::Lerp))
        .def_static("Lerp", py::overload_cast<const c::Quaternion&, const c::Quaternion&, float>(&c::Mathf::Lerp))
        .def_static("Slerp", &c::Mathf::Slerp)
        .def_static("Integral", &c::Mathf::Integral)
        .def_static("Det", py::overload_cast<const c::Matrix3f&>(&c::Mathf::Det))
        .def_static("Det", py::overload_cast<const c::Matrix4f&>(&c::Mathf::Det))
        .def_static("Transpose", py::overload_cast<const c::Matrix3f&>(&c::Mathf::Transpose))
        .def_static("Transpose", py::overload_cast<const c::Matrix4f&>(&c::Mathf::Transpose))
        .def_static("Invert", py::overload_cast<const c::Matrix3f&>(&c::Mathf::Invert))
        .def_static("Invert", py::overload_cast<const c::Matrix4f&>(&c::Mathf::Invert))
        .def_static("Binomial", &c::Mathf::Binomial)
        .def_static("Deg2Rad", py::overload_cast<float>(&c::Mathf::Deg2Rad))
        .def_static("Deg2Rad", py::overload_cast<const c::Vector3f&>(&c::Mathf::Deg2Rad))
        .def_static("Rad2Deg", py::overload_cast<float>(&c::Mathf::Rad2Deg))
        .def_static("Rad2Deg", py::overload_cast<const c::Vector3f&>(&c::Mathf::Rad2Deg))
        .def_static("Spherical2Cartesian", py::overload_cast<float, float, float>(&c::Mathf::Spherical2Cartesian))
        .def_static("Spherical2Cartesian", py::overload_cast<float, float>(&c::Mathf::Spherical2Cartesian))
        .def_static("GenRotationXMatrix", &c::Mathf::GenRotationXMatrix)
        .def_static("GenRotationYMatrix", &c::Mathf::GenRotationYMatrix)
        .def_static("GenRotationZMatrix", &c::Mathf::GenRotationZMatrix)
        .def_static("GenRotationXYZMatrix", py::overload_cast<float, float, float>(&c::Mathf::GenRotationXYZMatrix))
        .def_static("GenRotationXYZMatrix", py::overload_cast<const c::Vector3f&>(&c::Mathf::GenRotationXYZMatrix))
        .def_static("GenRotationMatrix", &c::Mathf::GenRotationMatrix)
        .def_static("GenQuaternion", &c::Mathf::GenQuaternion)
        .def_static("GenTranslationMatrix", py::overload_cast<float, float, float>(&c::Mathf::GenTranslationMatrix))
        .def_static("GenTranslationMatrix", py::overload_cast<const c::Vector3f&>(&c::Mathf::GenTranslationMatrix))
        .def_static("GenScalingMatrix", py::overload_cast<float, float, float>(&c::Mathf::GenScalingMatrix))
        .def_static("GenScalingMatrix", py::overload_cast<const c::Vector3f&>(&c::Mathf::GenScalingMatrix))
        .def_static("GetScalingFromTransformationMatrix", &c::Mathf::GetScalingFromTransformationMatrix)
        .def_static("GetTranslationFromTransformationMatrix", &c::Mathf::GetTranslationFromTransformationMatrix)
        .def_static("GetRotationFromTransformationMatrix", &c::Mathf::GetRotationFromTransformationMatrix)
        .def_static("GenCameraViewMatrix", &c::Mathf::GenCameraViewMatrix)
        .def_static("GenCameraOrientationQuaternion", &c::Mathf::GenCameraOrientationQuaternion)
        .def_static("GenPerspectiveProjectionMatrix", &c::Mathf::GenPerspectiveProjectionMatrix)
        .def_static("GenOrthographicProjectionMatrix", py::overload_cast<float, float, float, float>(&c::Mathf::GenOrthographicProjectionMatrix))
        .def_static("GenOrthographicProjectionMatrix", py::overload_cast<float, float, float, float, float, float>(&c::Mathf::GenOrthographicProjectionMatrix));

// classes

    py::class_<c::Game, std::shared_ptr<c::Game>>(m, "Game")
        .def(py::init<>())
        .def("Initialize", &c::Game::Initialize)
        .def("Deinitialize", &c::Game::Deinitialize)
        .def("Run", &c::Game::Run)
        .def("Stop", &c::Game::Stop)
        .def("Step", &c::Game::Step)
        .def("IsRunning", &c::Game::IsRunning)
        .def("GetResourceManager", &c::Game::GetResourceManager, py::return_value_policy::automatic)
        .def("GetGameSceneManager", &c::Game::GetGameSceneManager, py::return_value_policy::automatic)
        .def("CreateGameScene", &c::Game::Create<c::GameScene>, py::return_value_policy::automatic)
        .def("GetCurrentGameScene", &c::Game::GetCurrentGameScene, py::return_value_policy::automatic)
        .def("SetCurrentGameScene", &c::Game::SetCurrentGameScene)
        .def("CreateGLFWInputController", &c::Game::Create<c::GLFWInputController>, py::return_value_policy::automatic)
        .def("GetInputController", &c::Game::GetInputController, py::return_value_policy::automatic)
        .def("GetMessageBus", &c::Game::GetMessageBus, py::return_value_policy::automatic);

    py::class_<c::Primitives>(m, "Primitives")
        .def_static("GeneratePlane", &c::Primitives::GeneratePlane)
        .def_static("GenerateCube", &c::Primitives::GenerateCube)
        .def_static("GenerateSphere", &c::Primitives::GenerateSphere)
        .def_static("GenerateCone", &c::Primitives::GenerateCone)
        .def_static("GenerateCylinder", &c::Primitives::GenerateCylinder);
    
    py::class_<c::GameScene, std::shared_ptr<c::GameScene>>(m, "GameScene")
        .def("CreateGLFWRenderer", &c::GameScene::Create<c::GLFWRenderer, unsigned int, unsigned int, bool, bool, std::string, bool, bool, bool>, py::return_value_policy::automatic)
        .def("AddGameObject", &c::GameScene::Add<c::GameObject>, py::return_value_policy::automatic)
        .def("SetActiveCamera", &c::GameScene::SetActiveCamera)
        .def("CreateMeshObject", &c::GameScene::Create<c::MeshObject, const std::string&, const std::string&>, py::return_value_policy::automatic)
        .def("CreatePointLight", &c::GameScene::Create<c::PointLight>, py::return_value_policy::automatic)
        .def("CreateDirectionalLight", &c::GameScene::Create<c::DirectionalLight>, py::return_value_policy::automatic)
        .def("CreateSpotLight", &c::GameScene::Create<c::SpotLight>, py::return_value_policy::automatic)
        .def("CreatePBRMaterial", &c::GameScene::Create<c::PBRMaterial>, py::return_value_policy::automatic)
        .def("CreatePhongMaterial", &c::GameScene::Create<c::PhongMaterial>, py::return_value_policy::automatic)
        .def("CreateLinearPath", &c::GameScene::Create<c::LinearPath>, py::return_value_policy::automatic)
        .def("CreateSplinePath", &c::GameScene::Create<c::SplinePath>, py::return_value_policy::automatic)
        .def("CreateAnimationObject", &c::GameScene::Create<c::AnimationObject>, py::return_value_policy::automatic)
        .def("GetActiveCamera", &c::GameScene::GetActiveCamera, py::return_value_policy::automatic);

    py::class_<c::RenderStage, std::shared_ptr<c::RenderStage>>(m, "RenderStage")
        .def("SetFramebufferEnabled", &c::RenderStage::SetFramebufferEnabled, py::return_value_policy::automatic)
        .def("SetColorAttachmentsFramebufferLink", &c::RenderStage::SetColorAttachmentsFramebufferLink, py::return_value_policy::automatic)
        .def("SetMultisampleEnabled", &c::RenderStage::SetMultisampleEnabled, py::return_value_policy::automatic);

    py::class_<c::SurfaceRenderStage, c::RenderStage, std::shared_ptr<c::SurfaceRenderStage>>(m, "SurfaceRenderStage")
        .def("SetShaderProgram", &c::SurfaceRenderStage::SetShaderProgram, py::return_value_policy::automatic)
        .def("SetColorAttachmentsFramebufferLink", &c::SurfaceRenderStage::SetColorAttachmentsFramebufferLink, py::return_value_policy::automatic)
        .def("SetRenderStageParameterFloat", &c::SurfaceRenderStage::SetRenderStageParameterFloat, py::return_value_policy::automatic)
        .def("SetRenderStageParameterVector2f", &c::SurfaceRenderStage::SetRenderStageParameterVector2f, py::return_value_policy::automatic)
        .def("SetRenderStageParameterVector3f", &c::SurfaceRenderStage::SetRenderStageParameterVector3f, py::return_value_policy::automatic)
        .def("SetRenderStageParameterVector4f", &c::SurfaceRenderStage::SetRenderStageParameterVector4f, py::return_value_policy::automatic);

    py::class_<c::Renderer, std::shared_ptr<c::Renderer>>(m, "Renderer")
        .def("CreateSurfaceRenderStage", &c::Renderer::Create<c::SurfaceRenderStage>, py::return_value_policy::automatic)
        .def("GetWidth", &c::Renderer::GetWidth)
        .def("GetHeight", &c::Renderer::GetHeight);

    py::class_<c::GLRenderer, c::Renderer, std::shared_ptr<c::GLRenderer>>(m, "GLRenderer");

    py::class_<c::GLFWRenderer, c::GLRenderer, std::shared_ptr<c::GLFWRenderer>>(m, "GLFWRenderer");

    py::class_<c::Transform, std::shared_ptr<c::Transform>>(m, "Transform")
        .def(py::init<>())
        .def("GetTransformMatrix", &c::Transform::GetTransformMatrix, py::return_value_policy::automatic)
        .def("SetTransformMatrix", &c::Transform::SetTransformMatrix, py::return_value_policy::automatic)
        .def("GetTranslationMatrix", &c::Transform::GetTranslationMatrix, py::return_value_policy::automatic)
        .def("GetScalingMatrix", &c::Transform::GetScalingMatrix, py::return_value_policy::automatic)
        .def("GetRotationMatrix", &c::Transform::GetRotationMatrix, py::return_value_policy::automatic)
        .def("Translate", py::overload_cast<float, float, float>(&c::Transform::Translate), py::return_value_policy::automatic)
        .def("Translate", py::overload_cast<const c::Vector3f&>(&c::Transform::Translate), py::return_value_policy::automatic)
        .def("GetTranslation", &c::Transform::GetTranslation)
        .def("TranslateBy", py::overload_cast<float, float, float>(&c::Transform::TranslateBy), py::return_value_policy::automatic)
        .def("TranslateBy", py::overload_cast<const c::Vector3f&>(&c::Transform::TranslateBy), py::return_value_policy::automatic)
        .def("Scale", py::overload_cast<float, float, float>(&c::Transform::Scale), py::return_value_policy::automatic)
        .def("Scale", py::overload_cast<const c::Vector3f&>(&c::Transform::Scale), py::return_value_policy::automatic)
        .def("GetScale", &c::Transform::GetScale)
        .def("ScaleBy", py::overload_cast<float, float, float>(&c::Transform::ScaleBy), py::return_value_policy::automatic)
        .def("ScaleBy", py::overload_cast<const c::Vector3f&>(&c::Transform::ScaleBy), py::return_value_policy::automatic)
        .def("Scale", py::overload_cast<float>(&c::Transform::Scale), py::return_value_policy::automatic)
        .def("ScaleBy", py::overload_cast<float>(&c::Transform::ScaleBy), py::return_value_policy::automatic)
        .def("Rotate", py::overload_cast<float, float, float>(&c::Transform::Rotate), py::return_value_policy::automatic)
        .def("Rotate", py::overload_cast<const c::Vector3f&>(&c::Transform::Rotate), py::return_value_policy::automatic)
        .def("Rotate", py::overload_cast<const c::Quaternion&>(&c::Transform::Rotate), py::return_value_policy::automatic)
        .def("Rotate", py::overload_cast<const c::Vector3f&, float>(&c::Transform::Rotate), py::return_value_policy::automatic)
        .def("GetRotation", &c::Transform::GetRotation)
        .def("GetRotationQuaternion", &c::Transform::GetRotationQuaternion)
        .def("RotateBy", py::overload_cast<float, float, float>(&c::Transform::RotateBy), py::return_value_policy::automatic)
        .def("RotateBy", py::overload_cast<const c::Vector3f&>(&c::Transform::RotateBy), py::return_value_policy::automatic)
        .def("RotateBy", py::overload_cast<const c::Quaternion&>(&c::Transform::RotateBy), py::return_value_policy::automatic);
    
    py::class_<c::GameObject, std::shared_ptr<c::GameObject>>(m, "GameObject")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetParentObject", &c::GameObject::SetParentObject, py::return_value_policy::automatic)
        .def("GetParentObject", &c::GameObject::GetParentObject, py::return_value_policy::automatic)
        .def("GetLocalTransform", &c::GameObject::GetModelTransform, py::return_value_policy::automatic)
        .def("GetPosition", &c::GameObject::GetPosition)
        .def("GetScaling", &c::GameObject::GetScaling)
        .def("GetRotation", &c::GameObject::GetRotation)
        .def("GetRight", &c::GameObject::GetRight)
        .def("GetUp", &c::GameObject::GetUp)
        .def("GetForward", &c::GameObject::GetForward);

    py::class_<c::MeshObject, c::GameObject, std::shared_ptr<c::MeshObject>>(m, "MeshObject");

    py::class_<c::Camera, c::GameObject, std::shared_ptr<c::Camera>>(m, "Camera");

    py::class_<c::Light, c::GameObject, std::shared_ptr<c::Light>>(m, "Light")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetEnabled", &c::Light::SetEnabled, py::return_value_policy::automatic)
        .def("IsEnabled", &c::Light::IsEnabled)
        .def("SetColor", &c::Light::SetColor, py::return_value_policy::automatic)
        .def("GetColor", &c::Light::GetColor);

    py::class_<c::PointLight, c::Light, std::shared_ptr<c::PointLight>>(m, "PointLight")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetConstantAttenuationFactor", &c::PointLight::SetConstantAttenuationFactor, py::return_value_policy::automatic)
        .def("SetLinearAttenuationFactor", &c::PointLight::SetLinearAttenuationFactor, py::return_value_policy::automatic)
        .def("SetQuadraticAttenuationFactor", &c::PointLight::SetQuadraticAttenuationFactor, py::return_value_policy::automatic)
        .def("GetConstantAttenuationFactor", &c::PointLight::GetConstantAttenuationFactor)
        .def("GetLinearAttenuationFactor", &c::PointLight::GetLinearAttenuationFactor)
        .def("GetQuadraticAttenuationFactor", &c::PointLight::GetQuadraticAttenuationFactor);

    py::class_<c::DirectionalLight, c::Light, std::shared_ptr<c::DirectionalLight>>(m, "DirectionalLight")
        .def(py::init<std::shared_ptr<c::GameScene>>());

    py::class_<c::SpotLight, c::PointLight, std::shared_ptr<c::SpotLight>>(m, "SpotLight")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetInnerCutoff", &c::SpotLight::SetInnerCutoff, py::return_value_policy::automatic)
        .def("SetOuterCutoff", &c::SpotLight::SetOuterCutoff, py::return_value_policy::automatic)
        .def("GetInnerCutoff", &c::SpotLight::GetInnerCutoff)
        .def("GetOuterCutoff", &c::SpotLight::GetOuterCutoff);

    py::class_<c::PerspectiveCamera, c::Camera, std::shared_ptr<c::PerspectiveCamera>>(m, "PerspectiveCamera")
        .def(py::init<std::shared_ptr<c::GameScene>, float, float, float>());

    py::class_<c::Path, c::GameObject, std::shared_ptr<c::Path>>(m, "Path")
        .def("AddWaypoint", py::overload_cast<std::size_t, const c::Vector3f&, const c::Quaternion&>(&c::Path::AddWaypoint), py::return_value_policy::automatic)
        .def("AddWaypoint", py::overload_cast<const c::Vector3f&, const c::Quaternion&>(&c::Path::AddWaypoint), py::return_value_policy::automatic)
        .def("SetWaypoint", &c::Path::SetWaypoint, py::return_value_policy::automatic)
        .def("GetPositionAtDistance", &c::Path::GetPositionAtDistance)
        .def("GetTangentAtDistance", &c::Path::GetTangentAtDistance)
        .def("GetRotationAtDistance", &c::Path::GetRotationAtDistance)
        .def("GetPathLength", &c::Path::GetPathLength);

    py::class_<c::LinearPath, c::Path, std::shared_ptr<c::LinearPath>>(m, "LinearPath")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("GetPositionAtDistance", &c::LinearPath::GetPositionAtDistance)
        .def("GetTangentAtDistance", &c::LinearPath::GetTangentAtDistance);

    py::class_<c::SplinePath, c::Path, std::shared_ptr<c::SplinePath>>(m, "SplinePath")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("GetPositionAtDistance", &c::SplinePath::GetPositionAtDistance)
        .def("GetTangentAtDistance", &c::SplinePath::GetTangentAtDistance)
        .def("SetStartTangent", &c::SplinePath::SetStartTangent, py::return_value_policy::automatic)
        .def("SetEndTangent", &c::SplinePath::SetEndTangent, py::return_value_policy::automatic);

    py::class_<c::AnimationObject, c::GameObject, std::shared_ptr<c::AnimationObject>>(m, "AnimationObject")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("AddAnimationProperty", py::overload_cast<const std::string&, float, std::function<void(float)>, std::function<float(float, float, float)>>(&c::AnimationObject::AddAnimationProperty<float>), py::return_value_policy::automatic)
        .def("AddAnimationProperty", py::overload_cast<const std::string&, c::Vector3f, std::function<void(c::Vector3f)>, std::function<c::Vector3f(c::Vector3f, c::Vector3f, float)>>(&c::AnimationObject::AddAnimationProperty<c::Vector3f>), py::return_value_policy::automatic)
        .def("AddAnimationProperty", py::overload_cast<const std::string&, c::Quaternion, std::function<void(c::Quaternion)>, std::function<c::Quaternion(c::Quaternion, c::Quaternion, float)>>(&c::AnimationObject::AddAnimationProperty<c::Quaternion>), py::return_value_policy::automatic)
        .def("Play", &c::AnimationObject::Play)
        .def("Stop", &c::AnimationObject::Stop)
        .def("Seek", &c::AnimationObject::Seek)
        .def("SetLooping", &c::AnimationObject::SetLooping);

    py::class_<c::AnimationProperty<float>, std::shared_ptr<c::AnimationProperty<float>>>(m, "AnimationPropertyFloat")
        .def(py::init<std::shared_ptr<c::AnimationObject>, std::function<void(float)>, std::function<float(float, float, float)>>())
        .def("AddKeyframe", py::overload_cast<float, float>(&c::AnimationProperty<float>::AddKeyframe), py::return_value_policy::automatic);

    py::class_<c::AnimationProperty<c::Vector3f>, std::shared_ptr<c::AnimationProperty<c::Vector3f>>>(m, "AnimationPropertyVector3f")
        .def(py::init<std::shared_ptr<c::AnimationObject>, std::function<void(c::Vector3f)>, std::function<c::Vector3f(c::Vector3f, c::Vector3f, float)>>())
        .def("AddKeyframe", py::overload_cast<float, c::Vector3f>(&c::AnimationProperty<c::Vector3f>::AddKeyframe), py::return_value_policy::automatic);

    py::class_<c::AnimationProperty<c::Quaternion>, std::shared_ptr<c::AnimationProperty<c::Quaternion>>>(m, "AnimationPropertyQuaternion")
        .def(py::init<std::shared_ptr<c::AnimationObject>, std::function<void(c::Quaternion)>, std::function<c::Quaternion(c::Quaternion, c::Quaternion, float)>>())
        .def("AddKeyframe", py::overload_cast<float, c::Quaternion>(&c::AnimationProperty<c::Quaternion>::AddKeyframe), py::return_value_policy::automatic);

    py::class_<c::Resource, std::shared_ptr<c::Resource>>(m, "Resource")
        .def("GetHandle", &c::Resource::GetHandle)
        .def("GetName", &c::Resource::GetName);

    py::class_<c::LoadableResource, c::Resource, std::shared_ptr<c::LoadableResource>>(m, "LoadableResource")
        .def("Load", &c::LoadableResource::Load, py::return_value_policy::automatic);

    py::class_<c::Texture, c::LoadableResource, std::shared_ptr<c::Texture>>(m, "Texture")
        .def(py::init<const int, const int, float>())
        .def(py::init<const int, const int, const c::Vector3f&>())
        .def(py::init<const std::string&>())
        .def("GenerateSolid", py::overload_cast<const int, const int, float>(&c::Texture::GenerateSolid), py::return_value_policy::automatic)
        .def("GenerateSolid", py::overload_cast<const int, const int, const c::Vector3f&>(&c::Texture::GenerateSolid), py::return_value_policy::automatic)
        .def("GetWidth", &c::Texture::GetWidth)
        .def("GetHeight", &c::Texture::GetHeight);

    py::class_<c::Mesh, c::Resource, std::shared_ptr<c::Mesh>>(m, "Mesh")
        .def(py::init<>())
        .def("Clear", &c::Mesh::Clear, py::return_value_policy::automatic)
        .def("CalculateVertexNormals", &c::Mesh::CalculateVertexNormals, py::return_value_policy::automatic)
        .def("SetSmoothNormals", &c::Mesh::SetSmoothNormals, py::return_value_policy::automatic)
        .def("CalculateTangentsBitangents", &c::Mesh::CalculateTangentsBitangents, py::return_value_policy::automatic)
        .def("GetVertexCount", &c::Mesh::GetVertexCount)
        .def("GetFaceCount", &c::Mesh::GetFaceCount)
        .def("GetIndexCount", &c::Mesh::GetIndexCount)
        .def("AddVertex", &c::Mesh::AddVertex, py::return_value_policy::automatic)
        .def("AddFace", &c::Mesh::AddFace, py::return_value_policy::automatic)
        .def("AddNormal", &c::Mesh::AddNormal, py::return_value_policy::automatic)
        .def("AddTangentBitangent", &c::Mesh::AddTangentBitangent, py::return_value_policy::automatic)
        .def("AddVertexBoneInfluence", &c::Mesh::AddVertexBoneInfluence, py::return_value_policy::automatic);

    py::class_<c::Material, c::Resource, std::shared_ptr<c::Material>>(m, "Material")
        .def("SetForwardShaderProgram", &c::Material::SetForwardShaderProgram, py::return_value_policy::automatic)
        .def("SetDeferredGeometryPassShaderProgram", &c::Material::SetDeferredGeometryPassShaderProgram, py::return_value_policy::automatic)
        .def("SetDeferredLightingPassShaderProgram", &c::Material::SetDeferredLightingPassShaderProgram, py::return_value_policy::automatic);

    py::class_<c::PBRMaterial, c::Material, std::shared_ptr<c::PBRMaterial>>(m, "PBRMaterial")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetAlbedo", py::overload_cast<const std::string&>(&c::PBRMaterial::SetAlbedo), py::return_value_policy::automatic)
        .def("SetAlbedo", py::overload_cast<const c::Vector3f&>(&c::PBRMaterial::SetAlbedo), py::return_value_policy::automatic)
        .def("SetNormal", &c::PBRMaterial::SetNormal, py::return_value_policy::automatic)
        .def("SetMetallic", py::overload_cast<const std::string&>(&c::PBRMaterial::SetMetallic), py::return_value_policy::automatic)
        .def("SetMetallic", py::overload_cast<float>(&c::PBRMaterial::SetMetallic), py::return_value_policy::automatic)
        .def("SetRoughness", py::overload_cast<const std::string&>(&c::PBRMaterial::SetRoughness), py::return_value_policy::automatic)
        .def("SetRoughness", py::overload_cast<float>(&c::PBRMaterial::SetRoughness), py::return_value_policy::automatic)
        .def("SetAO", py::overload_cast<const std::string&>(&c::PBRMaterial::SetAO), py::return_value_policy::automatic)
        .def("SetAO", py::overload_cast<float>(&c::PBRMaterial::SetAO), py::return_value_policy::automatic)
        .def("GetAlbedo", &c::PBRMaterial::GetAlbedo, py::return_value_policy::automatic)
        .def("GetNormal", &c::PBRMaterial::GetNormal, py::return_value_policy::automatic)
        .def("GetMetallic", &c::PBRMaterial::GetMetallic, py::return_value_policy::automatic)
        .def("GetRoughness", &c::PBRMaterial::GetRoughness, py::return_value_policy::automatic)
        .def("GetAO", &c::PBRMaterial::GetAO, py::return_value_policy::automatic);

    py::class_<c::PhongMaterial, c::Material, std::shared_ptr<c::PhongMaterial>>(m, "PhongMaterial")
        .def(py::init<std::shared_ptr<c::GameScene>>())
        .def("SetDiffuse", py::overload_cast<const std::string&>(&c::PhongMaterial::SetDiffuse), py::return_value_policy::automatic)
        .def("SetDiffuse", py::overload_cast<const c::Vector3f&>(&c::PhongMaterial::SetDiffuse), py::return_value_policy::automatic)
        .def("SetNormal", &c::PhongMaterial::SetNormal, py::return_value_policy::automatic)
        .def("SetSpecular", py::overload_cast<const std::string&>(&c::PhongMaterial::SetSpecular), py::return_value_policy::automatic)
        .def("SetSpecular", py::overload_cast<const c::Vector3f&>(&c::PhongMaterial::SetSpecular), py::return_value_policy::automatic)
        .def("SetSpecularShininess", &c::PhongMaterial::SetSpecularShininess, py::return_value_policy::automatic)
        .def("SetEmissive", py::overload_cast<const std::string&>(&c::PhongMaterial::SetEmissive), py::return_value_policy::automatic)
        .def("SetEmissive", py::overload_cast<const c::Vector3f&>(&c::PhongMaterial::SetEmissive), py::return_value_policy::automatic)
        .def("GetDiffuse", &c::PhongMaterial::GetDiffuse, py::return_value_policy::automatic)
        .def("GetNormal", &c::PhongMaterial::GetNormal, py::return_value_policy::automatic)
        .def("GetSpecular", &c::PhongMaterial::GetSpecular, py::return_value_policy::automatic)
        .def("GetSpecularShininess", &c::PhongMaterial::GetSpecularShininess, py::return_value_policy::automatic)
        .def("GetEmissive", &c::PhongMaterial::GetEmissive, py::return_value_policy::automatic);

    py::class_<c::ResourceManager<c::Resource>, std::shared_ptr<c::ResourceManager<c::Resource>>>(m, "ResourceManager")
        .def("LoadTexture", &c::ResourceManager<c::Resource>::Load<c::Texture>, py::return_value_policy::automatic)
        .def("CreateMesh", &c::ResourceManager<c::Resource>::Create<c::Mesh>, py::return_value_policy::automatic)
        .def("GetByName", &c::ResourceManager<c::Resource>::GetByName<c::Resource>, py::return_value_policy::automatic)
        .def("GetByHandle", &c::ResourceManager<c::Resource>::GetByHandle<c::Resource>, py::return_value_policy::automatic);

    py::class_<c::InputController, std::shared_ptr<c::InputController>>(m, "InputController")
        .def("Initialize", &c::InputController::Initialize)
        .def("Deinitialize", &c::InputController::Deinitialize)
        .def("OnFrame", &c::InputController::OnFrame)
        .def("CreateInputEvent", py::overload_cast<const std::string&>(&c::InputController::CreateInputEvent))
        .def("CreateInputAxis", py::overload_cast<const std::string&, float>(&c::InputController::CreateInputAxis))
        .def("BindInputEvent", &c::InputController::BindInputEvent)
        .def("BindInputAxis", &c::InputController::BindInputAxis)
        .def("SetMouseGameMode", &c::InputController::SetMouseGameMode)
        .def("IsGameMode", &c::InputController::IsGameMode);

    py::class_<c::GLFWInputController, c::InputController, std::shared_ptr<c::GLFWInputController>>(m, "GLFWInputController")
        .def(py::init<std::shared_ptr<c::Game>>())
        .def("Initialize", &c::GLFWInputController::Initialize)
        .def("Deinitialize", &c::GLFWInputController::Deinitialize)
        .def("OnFrame", &c::GLFWInputController::OnFrame)
        .def("CreateInputEvent", py::overload_cast<const std::string&, c::EInputKey, c::EInputTrigger, std::set<c::EInputModifier>>(&c::GLFWInputController::CreateInputEvent))
        .def("CreateInputAxis", py::overload_cast<const std::string&, c::EInputKey, std::set<c::EInputModifier>, float>(&c::GLFWInputController::CreateInputAxis))
        .def("CreateInputAxis", py::overload_cast<const std::string&, c::EInputAxis, float>(&c::GLFWInputController::CreateInputAxis))
        .def("SetMouseGameMode", &c::GLFWInputController::SetMouseGameMode);

    py::class_<c::Input<bool>, std::shared_ptr<c::Input<bool>>>(m, "InputBool")
        .def(py::init<const std::string&, float>())
        .def(py::init<const std::string&>())
        .def("GetName", &c::Input<bool>::GetName)
        .def("GetScale", &c::Input<bool>::GetScale)
        .def("Set", py::overload_cast<bool>(&c::Input<bool>::Set))
        .def("Set", py::overload_cast<bool, float>(&c::Input<bool>::Set))
        .def("Read", &c::Input<bool>::Read)
        .def("Clear", &c::Input<bool>::Clear);

    py::class_<c::Input<float>, std::shared_ptr<c::Input<float>>>(m, "InputFloat")
        .def(py::init<const std::string&, float>())
        .def(py::init<const std::string&>())
        .def("GetName", &c::Input<float>::GetName)
        .def("GetScale", &c::Input<float>::GetScale)
        .def("Set", py::overload_cast<float>(&c::Input<float>::Set))
        .def("Set", py::overload_cast<float, float>(&c::Input<float>::Set))
        .def("Read", &c::Input<float>::Read)
        .def("Clear", &c::Input<float>::Clear);

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
        .value("KeySpace", c::EInputKey::KeySpace)
        .value("KeyW", c::EInputKey::KeyW)
        .value("KeyA", c::EInputKey::KeyA)
        .value("KeyS", c::EInputKey::KeyS)
        .value("KeyD", c::EInputKey::KeyD);

    py::enum_<c::EInputAxis>(m, "InputAxis")
        .value("MouseX", c::EInputAxis::MouseX)
        .value("MouseY", c::EInputAxis::MouseY);

    py::enum_<c::EInputTrigger>(m, "InputTrigger")
        .value("Press", c::EInputTrigger::Press)
        .value("Release", c::EInputTrigger::Release);


}



