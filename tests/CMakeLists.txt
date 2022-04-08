cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_compile_definitions(CILANTRO_GL_VERSION=${CILANTRO_GL_VERSION})

add_executable(test01
test01.cpp
ControlledCamera.cpp ControlledCamera.h
)
add_dependencies(test01 deps)

add_executable(test02
test02.cpp
Orbiter.cpp Orbiter.h
)
add_dependencies(test02 deps)

add_executable(test03
test03.cpp
ControlledCamera.cpp ControlledCamera.h
)
add_dependencies(test03 deps)

add_executable(test04
test04.cpp
ControlledCamera.cpp ControlledCamera.h
)
add_dependencies(test01 deps cerberus)

target_link_libraries (test01 cilantro)
target_link_libraries (test02 cilantro)
target_link_libraries (test03 cilantro)
target_link_libraries (test04 cilantro)

externalproject_add(
    cerberus
    PREFIX "assets"
    URL http://artisaverb.info/Cerberus/Cerberus_by_Andrew_Maximov.zip
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""    
)

add_custom_target (deps WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY})
add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cilantro>/assets/)
add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cilantro>/textures/)
add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cilantro>/shaders/)

add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/cilantro/shaders/ $<TARGET_FILE_DIR:cilantro>/shaders/)
add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/textures/ $<TARGET_FILE_DIR:cilantro>/textures/)
add_custom_command (TARGET deps PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/assets/ $<TARGET_FILE_DIR:cilantro>/assets/)

externalproject_get_property(cerberus SOURCE_DIR)
add_custom_command (TARGET cerberus POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} $<TARGET_FILE_DIR:cilantro>/assets/)
add_custom_command (TARGET cerberus POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR}/Textures $<TARGET_FILE_DIR:cilantro>/textures/)
