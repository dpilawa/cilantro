cmake_minimum_required(VERSION 3.14)
find_package(pybind11 CONFIG REQUIRED)
 
pybind11_add_module(pycilantro pycilantro.cpp)
target_link_libraries(pycilantro PRIVATE cilantro)
set_target_properties(pycilantro PROPERTIES CXX_VISIBILITY_PRESET "default")
target_include_directories(pycilantro PRIVATE ${CMAKE_SOURCE_DIR}/cilantro/include)

