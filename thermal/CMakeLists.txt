# CMakeList.txt : CMake project for ${target_name}, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


set(target_name thermal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(COMPILE_TO_DLL "compile to dynamic link library" ON)


if(NOT COMPILE_TO_DLL)
    # https://pybind11.readthedocs.io/en/stable/compiling.html
    set(PYTHON_INTERPRETER_PATH F:/DevEnvs/Python/Anaconda3/envs/pybind39)
    set(PYTHON_LIBRARY ${PYTHON_INTERPRETER_PATH}/libs/python39.lib)
    set(PYTHON_INCLUDE_DIR ${PYTHON_INTERPRETER_PATH}/include)
    set(PYTHON_EXECUTABLE ${PYTHON_INTERPRETER_PATH}/python.exe)
    find_package(pybind11 REQUIRED)
endif()


set(SOURCE_FILES 
	impl.cpp
    thermal.cpp
    thermal.hpp
)

# pybind11_add_module (${target_name} ${SOURCE_FILES})
if (COMPILE_TO_DLL)
    add_library(${target_name} SHARED ${SOURCE_FILES})
    target_compile_definitions(${target_name} PRIVATE EXPORT_THERMAL_API)
    target_link_libraries(${target_name} PRIVATE libdirp)
else()
    add_library(${target_name} MODULE ${SOURCE_FILES}) 
    target_link_libraries(${target_name} PRIVATE  pybind11::module pybind11::lto pybind11::windows_extras libdirp)
endif()


target_include_directories(${target_name}
	PUBLIC
	${CMAKE_SOURCE_DIR}/thirdparty
)

target_link_directories(${target_name}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/thirdparty/tsdk-core/lib/windows/release_x64
)


if(NOT COMPILE_TO_DLL)
    pybind11_extension(${target_name})
    if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
        # Strip unnecessary sections of the binary on Linux/macOS
        pybind11_strip(${target_name})
    endif()

    set_target_properties(${target_name} PROPERTIES CXX_VISIBILITY_PRESET "hidden"
                                             CUDA_VISIBILITY_PRESET "hidden")
endif()

