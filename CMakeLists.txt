cmake_minimum_required(VERSION 3.14)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

project (cilantro)

set(CILANTRO_GL_VERSION_MAJOR 4)
set(CILANTRO_GL_VERSION_MINOR 6)
set(CILANTRO_GLES_VERSION_MAJOR 3)
set(CILANTRO_GLES_VERSION_MINOR 2)
option(CILANTRO_BUILD_DLL "Build as shared library" ON)
option(CILANTRO_BUILD_GLES "Build for GLES target" OFF)
option(CILANTRO_WITH_GLFW "Build with GLFW extensions" ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(INJECT_DEBUG_POSTFIX "Inject d postfix on libs" ON)
option(ASSIMP_BUILD_ASSIMP_TOOLS "Build assimp tools" OFF)
option(ASSIMP_NO_EXPORT "Skip exporters build for assimp" ON)
option(ASSIMP_BUILD_TESTS "Build assimp tests" OFF)
option(ASSIMP_BUILD_ZLIB "Build zlib for assimp" ON)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

if(CILANTRO_BUILD_GLES)
    set(GLAD_EXTENSIONS "GL_ARB_tessellation_shader")
    set(GLAD_EXTENSIONS "GL_ARB_geometry_shader4")
    set(GLAD_EXTENSIONS "GL_ARB_gpu_shader5")
    set(GLAD_EXTENSIONS "GL_ARB_framebuffer_no_attachments")
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory (ext/pybind11)
add_subdirectory (ext/glfw)
add_subdirectory (ext/glad/cmake)
add_subdirectory (ext/assimp)
add_subdirectory (cilantro)
add_subdirectory (python)
add_subdirectory (tests)


