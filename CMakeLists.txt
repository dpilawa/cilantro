cmake_minimum_required(VERSION 3.12)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project (cilantro)

set(CILANTRO_GL_VERSION_MAJOR 4)
set(CILANTRO_GL_VERSION_MINOR 1)
set(CILANTRO_GL_VERSION 410) # 140=3.1, 150=3.2, 330=3.3, ..., 460=4.6
option(CILANTRO_BUILD_DLL "Build as shared library" ON)
option(CILANTRO_WITH_GLFW "Build with GLFW extensions" ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(INJECT_DEBUG_POSTFIX "Inject d postfix on libs" ON)
option(ASSIMP_BUILD_ASSIMP_TOOLS "Build assimp tools" OFF)
option(ASSIMP_BUILD_TESTS "Build assimp tests" OFF)
option(ASSIMP_BUILD_ZLIB "Build zlib for assimp" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

set(GLAD_API "gl=${CILANTRO_GL_VERSION_MAJOR}.${CILANTRO_GL_VERSION_MINOR}")
set(GLAD_PROFILE "core")
set(GLAD_EXTENSIONS "") 

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory (ext/glfw)
add_subdirectory (ext/glad)
add_subdirectory (ext/assimp)
add_subdirectory (cilantro)
add_subdirectory (tests)


