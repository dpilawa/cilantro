cmake_minimum_required(VERSION 3.13)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project (cilantro)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

set(GLAD_API "gl=4.6")
set(GLAD_PROFILE "core")

add_compile_definitions(CILANTRO_MIN_GL_VERSION=460) # 140=3.1, 150=3.2, 330=3.3, ...
add_compile_definitions(CILANTRO_USE_GLFW)

add_subdirectory (ext/glfw)
add_subdirectory (ext/glad)
add_subdirectory (cilantro)
add_subdirectory (tests)

target_link_libraries (cilantro glfw glad)
target_link_libraries (test01 cilantro)
target_link_libraries (test02 cilantro)
target_link_libraries (test03 cilantro)
target_link_libraries (test04 cilantro)
