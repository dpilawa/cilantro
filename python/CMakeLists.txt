cmake_minimum_required(VERSION 3.14)

pybind11_add_module(pycilantro pycilantro.cpp)
target_compile_definitions(pycilantro PRIVATE CILANTRO_BUILDING_PYTHON_MODULE)
target_link_libraries(pycilantro PRIVATE cilantro) 
target_include_directories(pycilantro PRIVATE ${CMAKE_SOURCE_DIR}/cilantro/include)


