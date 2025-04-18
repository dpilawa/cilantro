cmake_minimum_required(VERSION 3.14)

glad_add_library(glad REPRODUCIBLE API gl:core=${CILANTRO_GL_VERSION_MAJOR}.${CILANTRO_GL_VERSION_MINOR})

include_directories ("${PROJECT_SOURCE_DIR}/ext/glfw/include")
include_directories ("${PROJECT_SOURCE_DIR}/ext/stb")
include_directories ("${PROJECT_SOURCE_DIR}/ext/assimp/include")
include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_compile_definitions(CILANTRO_GL_VERSION=${CILANTRO_GL_VERSION})
add_compile_definitions(CILANTRO_GL_VERSION_MAJOR=${CILANTRO_GL_VERSION_MAJOR})
add_compile_definitions(CILANTRO_GL_VERSION_MINOR=${CILANTRO_GL_VERSION_MINOR})
add_compile_definitions(CILANTRO_GLES_VERSION="${CILANTRO_GLES_VERSION}")
add_compile_definitions(CILANTRO_GLES_VERSION_MAJOR=${CILANTRO_GLES_VERSION_MAJOR})
add_compile_definitions(CILANTRO_GLES_VERSION_MINOR=${CILANTRO_GLES_VERSION_MINOR})

if(CILANTRO_WITH_GLFW)
    set(CILANTRO_FILES ${CILANTRO_FILES}
        include/graphics/GLFWRenderer.h
        include/input/GLFWInputController.h
        src/graphics/GLFWRenderer.cpp
        src/input/GLFWInputController.cpp
    )
endif()

if(CILANTRO_GL_VERSION GREATER 150)
    set(CILANTRO_FILES ${CILANTRO_FILES}
        include/graphics/GLMultisampleFramebuffer.h
        src/graphics/GLMultisampleFramebuffer.cpp
    )
endif()

set(CILANTRO_FILES ${CILANTRO_FILES}
include/cilantroengine.h

include/graphics/AABBRenderStage.h
include/graphics/IFramebuffer.h
include/graphics/Framebuffer.h
include/graphics/GLFramebuffer.h
include/graphics/DeferredGeometryRenderStage.h
include/graphics/ForwardGeometryRenderStage.h
include/graphics/GLShader.h
include/graphics/GLShaderProgram.h
include/graphics/IRenderer.h
include/graphics/Renderer.h
include/graphics/GLRenderer.h
include/graphics/SurfaceRenderStage.h
include/graphics/ShadowMapRenderStage.h
include/graphics/IRenderStage.h
include/graphics/RenderStage.h
include/graphics/IShader.h
include/graphics/IShaderProgram.h
include/graphics/Shader.h
include/graphics/ShaderProgram.h
include/input/InputController.h
include/input/Input.h
include/math/AABB.h
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
include/math/Triangle.h
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
include/scene/Bone.h
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
include/scene/Waypoint.h
include/system/Hook.h
include/system/Game.h
include/system/LogMessage.h
include/system/Timer.h
include/system/Message.h
include/system/MessageBus.h

src/graphics/AABBRenderStage.cpp
src/graphics/Framebuffer.cpp
src/graphics/GLFramebuffer.cpp
src/graphics/DeferredGeometryRenderStage.cpp
src/graphics/ForwardGeometryRenderStage.cpp
src/graphics/GLShader.cpp
src/graphics/GLShaderProgram.cpp
src/graphics/Renderer.cpp
src/graphics/GLRenderer.cpp
src/graphics/RenderStage.cpp
src/graphics/SurfaceRenderStage.cpp
src/graphics/ShadowMapRenderStage.cpp
src/graphics/Shader.cpp
src/graphics/ShaderProgram.cpp
src/input/InputController.cpp
src/math/AABB.cpp
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
src/math/Triangle.cpp
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
src/scene/Bone.cpp
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
src/scene/Waypoint.cpp
src/system/Game.cpp
src/system/LogMessage.cpp
src/system/Timer.cpp
src/system/MessageBus.cpp

shaders/blinnphong_deferred_geometrypass.fs
shaders/blinnphong_deferred_lightingpass.fs
shaders/blinnphong_forward.fs
shaders/default.vs
shaders/flatquad.fs
shaders/flatquad.vs
shaders/pbr_deferred_geometrypass.fs
shaders/pbr_deferred_lightingpass.fs
shaders/pbr_forward.fs
shaders/post_fxaa.fs
shaders/post_gamma.fs
shaders/post_hdr.fs
shaders/shadowmap.fs
shaders/shadowmap_directional.gs
shaders/shadowmap.vs

)

add_library(cilantro SHARED ${CILANTRO_FILES})

if(CILANTRO_BUILD_DLL)
    target_compile_definitions(cilantro PRIVATE CILANTRO_BUILDING_DLL)
endif()

if(CILANTRO_BUILD_GLES)
    target_compile_definitions(cilantro PRIVATE CILANTRO_BUILDING_GLES)
endif()

target_compile_definitions(cilantro PRIVATE PYBIND11_EXPORT)

target_link_libraries (cilantro PUBLIC glfw glad assimp)

if (DEFINED EMSCRIPTEN)
set_target_properties(cilantro
        PROPERTIES
        LINK_FLAGS " --bind -s USE_GLFW=3 -s WASM=1 -gsource-map")
endif (DEFINED EMSCRIPTEN)
