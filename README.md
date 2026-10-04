[![Build Status](https://github.com/dpilawa/cilantro/workflows/build/badge.svg)](https://github.com/dpilawa/cilantro/actions?workflow=build)
# Cilantro Engine

Cilantro is a small 3D engine written in C++20 on top of OpenGL 4.6. It has a scene graph, a configurable render pipeline with forward and deferred rendering, physically based and Blinn-Phong materials, shadow mapping, skeletal animation and Python bindings.

[![Skeletal animation](https://img.youtube.com/vi/LbIv0L_MZGI/0.jpg)](https://www.youtube.com/watch?v=LbIv0L_MZGI)
[![PBR](https://img.youtube.com/vi/J4nGvD1Ytcc/0.jpg)](https://www.youtube.com/watch?v=J4nGvD1Ytcc)

## Features
- **Rendering**: forward and deferred pipelines (selected when the renderer is created), 4x multisampling, HDR, FXAA and gamma post-processing stages, axis-aligned bounding box visualization.
- **Materials**: metallic-roughness PBR (albedo, normal, metallic, roughness, ambient occlusion maps) and Blinn-Phong (diffuse, normal, specular, emissive).
- **Lights and shadows**: directional, point and spot lights, all with shadow mapping and hardware PCF.
- **Animation**: skeletal animation of imported models, keyframe animation of float, vector and quaternion properties, and linear and spline paths (curves: Bezier, B-spline, NURBS and cubic Hermite).
- **Assets**: models are imported with [Assimp](https://github.com/assimp/assimp) (FBX and many other formats), textures are loaded with [stb](https://github.com/nothings/stb).
- **Scene graph**: hierarchical game objects with transforms and bounding boxes, cameras (perspective and orthographic), generated primitives.
- **Input**: keyboard and mouse events and axes through [GLFW](https://www.glfw.org/).
- **Python**: much of the scene API is available through a [pybind11](https://github.com/pybind/pybind11) module.

## Getting started

### Requirements
- A C++20 compiler (Visual Studio 2022, Clang or GCC) and CMake 3.14 or newer.
- A GPU and drivers with OpenGL 4.6.
- Python 3 with the `Jinja2` package (used to generate the OpenGL loader).
- On Linux: `libwayland-dev`, `libxkbcommon-dev` and `xorg-dev` (needed to build GLFW).

### Build
Third party libraries are git submodules, so clone recursively:

    git clone --recurse-submodules https://github.com/dpilawa/cilantro.git
    cd cilantro
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

The build produces the engine library (`cilantro`), the Python module, the demos and the unit tests. Everything is written to the build directory (`build/Release` with Visual Studio). Shaders, textures and models used by the demos are copied next to the executables, so run the demos from that directory. The Cerberus model used by `test04` is downloaded during the build, which needs network access.

### Demos
| Demo | What it shows |
|---|---|
| `test01` | PBR primitives lit by directional, point and spot lights with shadow mapping (deferred rendering) and a light animated along a spline path. Also available as a Python script: `tests/test01.py`. |
| `test02` | Sun, Earth and Moon: forward rendering with Blinn-Phong materials (diffuse, specular and normal maps), 4x multisampling and a camera flying along a spline path. |
| `test03` | A skinned FBX character with skeletal animation, shadows and a PBR floor. |
| `test04` | The Cerberus model with PBR textures. |

In `test01`, `test03` and `test04` use `W`/`A`/`S`/`D` and the mouse to fly around and `Space` to release or capture the mouse. `Esc` quits every demo (in `test02` the camera moves on its own).

### A minimal program
This opens a window with a lit cube. It is a trimmed version of the demos.

```cpp
#include "cilantroengine.h"
#include "system/Game.h"
#include "scene/GameScene.h"
#include "scene/Primitives.h"
#include "scene/MeshObject.h"
#include "scene/PhongMaterial.h"
#include "scene/PerspectiveCamera.h"
#include "scene/PointLight.h"
#include "resource/ResourceManager.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/SurfaceRenderStage.h"
#include "input/GLFWInputController.h"

using namespace cilantro;

int main ()
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");

    // 800x600 window, no shadow mapping, forward rendering
    auto renderer = scene->Create<GLFWRenderer> (800, 600, false, false, "Hello cilantro", false, true, true);

    // the input controller needs the window created by the renderer, so it is created after it
    auto input = game->Create<GLFWInputController> ();

    // present the result of the geometry stage on screen
    renderer->Create<SurfaceRenderStage> ("screen")
        ->SetShaderProgram ("flatquad_shader")
        ->SetFramebufferEnabled (false)
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);

    // press Esc to quit
    input->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    input->BindInputEvent ("exit", [&]() { game->Stop (); });

    // a red cube
    scene->Create<PhongMaterial> ("red")
        ->SetDiffuse (Vector3f (0.8f, 0.2f, 0.2f))
        ->SetSpecular (Vector3f (0.5f, 0.5f, 0.5f))
        ->SetSpecularShininess (32.0f);

    Primitives::GenerateCube (game->GetResourceManager ()->Create<Mesh> ("cubeMesh"));
    scene->Create<MeshObject> ("cube", "cubeMesh", "red")
        ->GetModelTransform ()->Rotate (30.0f, 45.0f, 0.0f);

    // camera and light
    scene->Create<PerspectiveCamera> ("camera", 45.0f, 0.1f, 100.0f)
        ->GetModelTransform ()->Translate (0.0f, 1.0f, 8.0f);
    scene->SetActiveCamera ("camera");

    scene->Create<PointLight> ("light")
        ->SetColor (Vector3f (1.0f, 1.0f, 1.0f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Translate (2.0f, 3.0f, 4.0f);

    game->Run ();
    game->Deinitialize ();

    return 0;
}
```

To try it, add it to `tests/CMakeLists.txt` the same way as the demos (`add_executable`, `add_dependencies (hello deps)` and `target_link_libraries (hello cilantro)`) and run it from the build directory.

## How it is organized
The engine is a single library, `cilantro`, split into modules under `cilantro/include` and `cilantro/src`:

| Module | Contents |
|---|---|
| `system` | `Game` (main loop; owns the scenes, the input controller and the resource manager), `MessageBus` (typed publish/subscribe), `Hook`, `Timer`, `LogMessage`. |
| `scene` | `GameScene`, the `GameObject` hierarchy with `Transform` and bounding boxes, cameras, lights, materials, `MeshObject`, animation objects, paths and primitives. |
| `resource` | `ResourceManager<T>` (named, typed resources with handles), `Mesh`, `Texture`, `Bone` and the Assimp model loader. |
| `graphics` | `Renderer`, the render stages and the OpenGL backend (`GLRenderer`, `GLFWRenderer`). GLSL shaders are in `cilantro/shaders` and go through a small preprocessor that supports includes and global values. |
| `input` | Input events and axes, with a GLFW implementation. |
| `math` | Vectors, matrices, quaternions, `Mathf`, `AABB` and curves. |

A frame goes like this: `Game::Run` calls `Game::Step` in a loop. `Step` runs the current scene, which updates its game objects and then asks its renderer to render a frame, and then processes input. The renderer runs its *render pipeline*, an ordered list of render stages (shadow map, geometry, lighting, post-processing, screen). Each stage draws into its own framebuffer and can read the framebuffers of other stages, selected with a pipeline link (for example `EPipelineLink::LINK_PREVIOUS`). Changes in the scene (new or modified meshes, materials, lights, transforms) are published on the message bus, and the renderer subscribes to them to keep its GPU data up to date.

Other directories: `python` holds the Python module (`pycilantro.cpp`), `tests` holds the demos and `tests/unit` the unit tests, and `ext` holds third party libraries as git submodules.

## Python
The Python module is built together with the engine, as `pycilantro`. Use it with the interpreter it was built for (see the build options below):

    cd build
    PYTHONPATH=. python ../tests/test01.py

`tests/test01.py` is the Python version of `test01`.

## Unit tests
Unit tests (math, resource manager, message bus, hooks, shader preprocessor) are built with the project and use GoogleTest, which is downloaded during CMake configuration. They do not need a GPU or a window.

    cmake --build build
    cd build && ctest --output-on-failure

Pass `-DCILANTRO_BUILD_UNIT_TESTS=OFF` to CMake to skip them. A test for a known defect that is not fixed yet is prefixed with `DISABLED_` (run those with `--gtest_also_run_disabled_tests`). CI builds the engine and runs the unit tests on Linux and Windows.

## Build options
- `CMAKE_BUILD_TYPE` selects the build type for single configuration generators (defaults to `Debug` when not given).
- `CILANTRO_BUILD_UNIT_TESTS` (default `ON`) builds the unit tests.
- `CILANTRO_BUILD_PYTHON` (default `ON`) builds the Python module. It is built for the interpreter given in `PYTHON_EXECUTABLE`, so pass `-DPYTHON_EXECUTABLE=<path to python>` when the default one is not the one you want to use it with (the module cannot be loaded by a different Python version or by a Python built with a different toolchain). Pass `-DCILANTRO_BUILD_PYTHON=OFF` to skip it.
- `CILANTRO_BUILD_DLL` (default `ON`) builds the engine as a shared library.
- `CILANTRO_WITH_GLFW` (default `ON`) builds the GLFW window and input implementation.

## License
Cilantro is released under the [MIT License](LICENSE).
