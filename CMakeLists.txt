cmake_minimum_required(VERSION 3.13)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project (cilantro)

set(CILANTRO_GL_VERSION_MAJOR 4)
set(CILANTRO_GL_VERSION_MINOR 1)
set(CILANTRO_GL_VERSION 410) # 140=3.1, 150=3.2, 330=3.3, ..., 460=4.6
option(CILANTRO_BUILD_DLL "Build as shared library" ON)
option(CILANTRO_WITH_GLFW "Build with GLFW extensions" ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

set(GLAD_API "gl=${CILANTRO_GL_VERSION_MAJOR}.${CILANTRO_GL_VERSION_MINOR}")
set(GLAD_PROFILE "core")
set(GLAD_EXTENSIONS "") 

add_subdirectory (ext/glfw)
add_subdirectory (ext/glad)
add_subdirectory (cilantro)
add_subdirectory (tests)

target_link_libraries (cilantro glfw glad)
target_link_libraries (test01 cilantro)
target_link_libraries (test02 cilantro)
target_link_libraries (test03 cilantro)
