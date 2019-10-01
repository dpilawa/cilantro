include_directories ("${PROJECT_SOURCE_DIR}/cilantro")
add_executable(test01
main.cpp
ControlledCamera.cpp ControlledCamera.h
RotatingObject.cpp RotatingObject.h
)
