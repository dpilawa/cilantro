cmake_minimum_required(VERSION 3.13)
project (cilantro)

include_directories ("${PROJECT_SOURCE_DIR}/ext/glfw")
add_subdirectory (ext/glfw) 

include_directories ("${PROJECT_SOURCE_DIR}/cilantro")
add_subdirectory (cilantro) 
target_include_directories(cilantro PRIVATE ${PROJECT_SOURCE_DIR}/ext/imgui/)

find_library(GLEW "${PROJECT_SOURCE_DIR}/ext/glew/lib/libGLEW.so")

include_directories ("${PROJECT_SOURCE_DIR}/example01")
add_subdirectory (example01) 
target_link_libraries (example01 cilantro GLEW glfw)

include_directories ("${PROJECT_SOURCE_DIR}/example02")
add_subdirectory (example02) 
target_link_libraries (example02 cilantro GLEW glfw)
