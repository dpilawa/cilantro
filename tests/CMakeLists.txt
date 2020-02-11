include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_compile_definitions(CILANTRO_GL_VERSION=${CILANTRO_GL_VERSION})

add_executable(test01
test01.cpp
ControlledCamera.cpp ControlledCamera.h
RotatingObject.cpp RotatingObject.h
)

add_executable(test02
test02.cpp
Orbiter.cpp Orbiter.h
)

add_executable(test03
test03.cpp
ControlledCamera.cpp ControlledCamera.h
)

