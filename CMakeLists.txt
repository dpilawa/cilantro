cmake_minimum_required(VERSION 3.13)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
project (cilantro)

execute_process(
	COMMAND python ./gl3w_gen.py
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/gl3w
)

add_subdirectory (ext/glfw)
add_subdirectory (ext/gl3w)
add_subdirectory (cilantro)
add_subdirectory (test01)
add_subdirectory (test02)

target_link_libraries (cilantro glfw)
target_link_libraries (test01 cilantro)
target_link_libraries (test02 cilantro)

