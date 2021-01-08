cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0079 NEW)

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

