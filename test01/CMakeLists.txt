include_directories ("${PROJECT_SOURCE_DIR}/cilantro")
include_directories ("${PROJECT_SOURCE_DIR}/ext/glfw/include")
include_directories ("${PROJECT_SOURCE_DIR}/ext/gl3w/include")
add_executable(test01
main.cpp
ControlledCamera.cpp ControlledCamera.h
RotatingObject.cpp RotatingObject.h
)
