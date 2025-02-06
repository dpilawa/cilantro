cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-fPIC)

project (cilantro)

set(CILANTRO_GL_VERSION_MAJOR 4)
set(CILANTRO_GL_VERSION_MINOR 6)
set(CILANTRO_GL_VERSION 460) # 140=3.1, 150=3.2, 330=3.3, ..., 460=4.6
set(CILANTRO_GLES_VERSION_MAJOR 3)
set(CILANTRO_GLES_VERSION_MINOR 2)
set(CILANTRO_GLES_VERSION "300 es")
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

set(GLAD_API "gl=${CILANTRO_GL_VERSION_MAJOR}.${CILANTRO_GL_VERSION_MINOR},gles2=${CILANTRO_GLES_VERSION_MAJOR}.${CILANTRO_GLES_VERSION_MINOR}")
set(GLAD_PROFILE "core")

if(CILANTRO_BUILD_GLES)
    set(GLAD_EXTENSIONS "GL_EXT_geometry_shader")
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory (ext/glfw)
add_subdirectory (ext/glad)
add_subdirectory (ext/assimp)
add_subdirectory (cilantro)
add_subdirectory (tests)


