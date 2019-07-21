include_directories ("${PROJECT_SOURCE_DIR}/cilantro")
include_directories ("${PROJECT_SOURCE_DIR}/ext/glfw/include")
include_directories ("${PROJECT_SOURCE_DIR}/ext/gl3w/include")
add_executable(test01
main.cpp
RotatingObject.cpp RotatingObject.h
)
