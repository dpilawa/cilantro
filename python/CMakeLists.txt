cmake_minimum_required(VERSION 3.14)

remove_definitions(-DCILANTRO_BUILDING_DLL)

pybind11_add_module(pycilantro pycilantro.cpp)
target_link_libraries(pycilantro PRIVATE pybind11::module cilantro) 
target_include_directories(pycilantro PRIVATE ${CMAKE_SOURCE_DIR}/cilantro/include)

