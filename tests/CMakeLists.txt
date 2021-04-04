cmake_minimum_required(VERSION 3.12)

include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_compile_definitions(CILANTRO_GL_VERSION=${CILANTRO_GL_VERSION})

add_executable(test01
test01.cpp
ControlledCamera.cpp ControlledCamera.h
)

add_executable(test02
test02.cpp
Orbiter.cpp Orbiter.h
)

add_executable(test03
test03.cpp
ControlledCamera.cpp ControlledCamera.h
)

target_link_libraries (test01 cilantro)
target_link_libraries (test02 cilantro)
target_link_libraries (test03 cilantro)

add_custom_command (TARGET test01 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/cilantro/shaders/ $<TARGET_FILE_DIR:test01>/shaders/)
add_custom_command (TARGET test01 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/textures/ $<TARGET_FILE_DIR:test01>/textures/)
