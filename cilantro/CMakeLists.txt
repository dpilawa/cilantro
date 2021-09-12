cmake_minimum_required(VERSION 3.12)

include_directories ("${PROJECT_SOURCE_DIR}/ext/glfw/include")
include_directories ("${PROJECT_SOURCE_DIR}/ext/stb")
include_directories ("${PROJECT_SOURCE_DIR}/ext/assimp/include")
include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_compile_definitions(CILANTRO_GL_VERSION=${CILANTRO_GL_VERSION})
add_compile_definitions(CILANTRO_GL_VERSION_MAJOR=${CILANTRO_GL_VERSION_MAJOR})
add_compile_definitions(CILANTRO_GL_VERSION_MINOR=${CILANTRO_GL_VERSION_MINOR})

if(CILANTRO_BUILD_DLL)
    add_compile_definitions(CILANTRO_BUILDING_DLL)
endif()

if(CILANTRO_WITH_GLFW)
    set(CILANTRO_FILES ${CILANTRO_FILES}
        include/graphics/GLFWRenderTarget.h
        include/input/GLFWInputController.h
        src/graphics/GLFWRenderTarget.cpp
        src/input/GLFWInputController.cpp
    )
endif()

if(CILANTRO_GL_VERSION GREATER 140)
    set(CILANTRO_FILES ${CILANTRO_FILES}
        include/graphics/GLMultisampleFramebuffer.h
        src/graphics/GLMultisampleFramebuffer.cpp
    )
endif()

set(CILANTRO_FILES ${CILANTRO_FILES}
include/cilantroengine.h

include/game/Game.h
include/graphics/GeometryRenderStage.h
include/graphics/Framebuffer.h
include/graphics/GLGeometryRenderStage.h
include/graphics/GLFramebuffer.h
include/graphics/GLDeferredGeometryRenderStage.h
include/graphics/GLForwardGeometryRenderStage.h
include/graphics/GLRenderStage.h
include/graphics/GLQuadRenderStage.h
include/graphics/GLShader.h
include/graphics/GLShaderProgram.h
include/graphics/Renderer.h
include/graphics/QuadRenderStage.h
include/graphics/RenderStage.h
include/graphics/RenderTarget.h
include/graphics/Shader.h
include/graphics/ShaderProgram.h
include/input/InputController.h
include/input/Input.h
include/math/Bezier.h
include/math/BSpline.h
include/math/CubicHermite.h
include/math/Curve.h
include/math/GaussLegendreIntegrator.h
include/math/Mathf.h
include/math/Matrix3f.h
include/math/Matrix4f.h
include/math/NURBS.h
include/math/Quaternion.h
include/math/Vector2f.h
include/math/Vector3f.h
include/math/Vector4f.h
include/resource/AssimpModelLoader.h
include/resource/LoadableResource.h
include/resource/Mesh.h
include/resource/ModelLoader.h
include/resource/Resource.h
include/resource/ResourceManager.h
include/resource/Texture.h
include/scene/AnimationObject.h
include/scene/AnimationProperty.h
include/scene/Camera.h
include/scene/DirectionalLight.h
include/scene/GameObject.h
include/scene/GameScene.h
include/scene/Light.h
include/scene/LinearPath.h
include/scene/Material.h
include/scene/MeshObject.h
include/scene/OrthographicCamera.h
include/scene/PerspectiveCamera.h
include/scene/Path.h
include/scene/PBRMaterial.h
include/scene/PhongMaterial.h
include/scene/PointLight.h
include/scene/Primitives.h
include/scene/SplinePath.h
include/scene/SpotLight.h
include/scene/Transform.h
include/system/CallbackProvider.h
include/system/EngineContext.h
include/system/LogMessage.h
include/system/Timer.h
src/game/Game.cpp
src/graphics/GeometryRenderStage.cpp
src/graphics/Framebuffer.cpp
src/graphics/GLGeometryRenderStage.cpp
src/graphics/GLFramebuffer.cpp
src/graphics/GLDeferredGeometryRenderStage.cpp
src/graphics/GLForwardGeometryRenderStage.cpp
src/graphics/GLRenderStage.cpp
src/graphics/GLQuadRenderStage.cpp
src/graphics/GLShader.cpp
src/graphics/GLShaderProgram.cpp
src/graphics/Renderer.cpp
src/graphics/RenderStage.cpp
src/graphics/QuadRenderStage.cpp
src/graphics/RenderTarget.cpp
src/graphics/Shader.cpp
src/graphics/ShaderProgram.cpp
src/input/InputController.cpp
src/math/Bezier.cpp
src/math/BSpline.cpp
src/math/CubicHermite.cpp
src/math/Curve.cpp
src/math/GaussLegendreIntegrator.cpp
src/math/Mathf.cpp
src/math/Matrix3f.cpp
src/math/Matrix4f.cpp
src/math/NURBS.cpp
src/math/Quaternion.cpp
src/math/Vector2f.cpp
src/math/Vector3f.cpp
src/math/Vector4f.cpp
src/resource/AssimpModelLoader.cpp
src/resource/LoadableResource.cpp
src/resource/Mesh.cpp
src/resource/ModelLoader.cpp
src/resource/Resource.cpp
src/resource/ResourceManager.cpp
src/resource/Texture.cpp
src/scene/AnimationObject.cpp
src/scene/AnimationProperty.cpp
src/scene/Camera.cpp
src/scene/DirectionalLight.cpp
src/scene/GameObject.cpp
src/scene/GameScene.cpp
src/scene/Light.cpp
src/scene/LinearPath.cpp
src/scene/Material.cpp
src/scene/MeshObject.cpp
src/scene/OrthographicCamera.cpp
src/scene/Path.cpp
src/scene/PBRMaterial.cpp
src/scene/PhongMaterial.cpp
src/scene/PerspectiveCamera.cpp
src/scene/PointLight.cpp
src/scene/Primitives.cpp
src/scene/SplinePath.cpp
src/scene/SpotLight.cpp
src/scene/Transform.cpp
src/system/EngineContext.cpp
src/system/LogMessage.cpp
src/system/Timer.cpp
)

add_library(cilantro SHARED ${CILANTRO_FILES})
target_link_libraries (cilantro glfw glad assimp)

