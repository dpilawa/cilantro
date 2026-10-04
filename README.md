[![Build Status](https://github.com/dpilawa/cilantro/workflows/build/badge.svg)](https://github.com/dpilawa/cilantro/actions?workflow=build)
# Cilantro Engine
[![Skeletal animation](https://img.youtube.com/vi/LbIv0L_MZGI/0.jpg)](https://www.youtube.com/watch?v=LbIv0L_MZGI)
[![PBR](https://img.youtube.com/vi/J4nGvD1Ytcc/0.jpg)](https://www.youtube.com/watch?v=J4nGvD1Ytcc)


## Unit tests
Unit tests (math, resource manager, message bus, hooks, shader preprocessor) are built with the project and use GoogleTest, which is downloaded during CMake configuration. They do not need a GPU or a window.

    cmake --build build
    cd build && ctest --output-on-failure

Pass `-DCILANTRO_BUILD_UNIT_TESTS=OFF` to CMake to skip them. Tests that document known defects are prefixed with `DISABLED_` (run them with `--gtest_also_run_disabled_tests`).

## Build options
- `CMAKE_BUILD_TYPE` selects the build type for single configuration generators (defaults to `Debug` when not given).
- `CILANTRO_BUILD_UNIT_TESTS` (default `ON`) builds the unit tests.
- `CILANTRO_BUILD_PYTHON` (default `ON`) builds the Python module. It is built for the interpreter given in `PYTHON_EXECUTABLE`, so pass `-DPYTHON_EXECUTABLE=<path to python>` when the default one is not the one you want to use it with (the module cannot be loaded by a different Python version or by a Python built with a different toolchain). Pass `-DCILANTRO_BUILD_PYTHON=OFF` to skip it.
