cmake_minimum_required(VERSION 3.14)

include(FetchContent)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

# GoogleTest (use shared CRT on Windows to match the engine library)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

include_directories ("${PROJECT_SOURCE_DIR}/cilantro/include")

add_executable(unit_tests
    TestHelpers.h
    VectorTests.cpp
    MatrixTests.cpp
    QuaternionTests.cpp
    MathfTests.cpp
    AABBTests.cpp
    CurveTests.cpp
    ResourceManagerTests.cpp
    MessageBusTests.cpp
    HookTests.cpp
    ShaderProcessorTests.cpp
    # ShaderProcessor is not exported from the engine library, so it is compiled into the test binary
    ${PROJECT_SOURCE_DIR}/cilantro/src/graphics/ShaderProcessor.cpp
)

target_link_libraries(unit_tests cilantro GTest::gtest_main)

add_test(NAME unit_tests COMMAND unit_tests)
