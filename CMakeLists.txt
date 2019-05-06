cmake_minimum_required(VERSION 3.13)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
project (cilantro)

add_subdirectory (ext/glfw)
add_subdirectory (ext/gl3w)

add_subdirectory (cilantro)
add_dependencies(cilantro gl3w)
target_link_libraries (cilantro glfw)

add_subdirectory (example01)
target_link_libraries (example01 cilantro)

add_subdirectory (example02)
target_link_libraries (example02 cilantro)

