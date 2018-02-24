#ifndef _CILANTROENGINE_H_
#define _CILANTROENGINE_H_

#ifdef BUILDING_DLL
#define __EAPI __declspec(dllexport)
#else
#define __EAPI __declspec(dllimport)

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CallbackProvider.h"
#include "LogMessage.h"
#include "Time.h"

#include "Matrix4f.h"
#include "Matrix3f.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Mathf.h"

#include "Material.h"

#include "GameObject.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "Light.h"
#include "PointLight.h"
#include "MeshObject.h"

#include "Renderer.h"
#include "RenderTarget.h"
#include "GLShader.h"
#include "GLShaderModel.h"
#include "GLFWRenderTarget.h"
#include "GLRenderer.h"

#include "GameScene.h"
#include "GameLoop.h"

#endif
#endif
