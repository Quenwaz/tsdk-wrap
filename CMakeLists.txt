# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("python_module")


list(APPEND CMAKE_PREFIX_PATH "F:/DevEnvs/Python/Anaconda3/envs/pybind39/share/cmake/pybind11")

# Include sub-projects.
add_subdirectory ("thermal")

