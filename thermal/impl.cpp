#include "thermal.hpp"
#include <iostream>
#include <fstream>
#include "tsdk-core/api/dirp_api.h"


#define ENABLE_MEMORY_LEAK_DETACT
#ifdef ENABLE_MEMORY_LEAK_DETACT
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif 

namespace {
#define CASE_STR(x) case x : return #x; break; 
    // 根据输入值打印
    //std::string error_str(dirp_ret_code_e e)
    std::string error_str(int e)
    {
        switch (e)
        {
            CASE_STR(DIRP_SUCCESS)
            CASE_STR(DIRP_ERROR_MALLOC)
            CASE_STR(DIRP_ERROR_POINTER_NULL)
            CASE_STR(DIRP_ERROR_INVALID_PARAMS)
            CASE_STR(DIRP_ERROR_INVALID_RAW)
            CASE_STR(DIRP_ERROR_INVALID_HEADER)
            CASE_STR(DIRP_ERROR_INVALID_CURVE)
            CASE_STR(DIRP_ERROR_RJPEG_PARSE)
            CASE_STR(DIRP_ERROR_SIZE)
            CASE_STR(DIRP_ERROR_INVALID_HANDLE)
            CASE_STR(DIRP_ERROR_FORMAT_INPUT)
            CASE_STR(DIRP_ERROR_FORMAT_OUTPUT)
            CASE_STR(DIRP_ERROR_UNSUPPORTED_FUNC)
            CASE_STR(DIRP_ERROR_NOT_READY)
            CASE_STR(DIRP_ERROR_ACTIVATION)
            CASE_STR(DIRP_ERROR_INVALID_INI)
            CASE_STR(DIRP_ERROR_INVALID_SUB_DLL)
            CASE_STR(DIRP_ERROR_ADVANCED)
        default:
            break;
        }
        return "UNKNOW_ERROR";
    }


    typedef enum
    {
        dirp_action_type_extract = 0,
        dirp_action_type_measure,
        dirp_action_type_process,
        dirp_action_type_num,
    } dirp_action_type_e;

    typedef enum
    {
        dirp_measure_format_int16 = 0,
        dirp_measure_format_float32,
        dirp_measure_format_num,
    } dirp_measure_format_e;


    bool read_rjpeg(const std::string& picPath, DIRP_HANDLE& handle, dirp_resolution_t& rjpeg_resolution_)
    {
        std::ifstream fs_i_rjpeg;
        fs_i_rjpeg.open(picPath.c_str(), std::ios::binary);
        if (!fs_i_rjpeg.is_open()) {
            return false;
        }

        fs_i_rjpeg.seekg(0, std::ios::end);
        const std::streampos fileSize = fs_i_rjpeg.tellg();
        fs_i_rjpeg.seekg(0, std::ios::beg);
        uint8_t* rjpeg_data = (uint8_t*)malloc(fileSize);
        fs_i_rjpeg.read((char*)rjpeg_data, fileSize);
        fs_i_rjpeg.close();

        auto ret = dirp_create_from_rjpeg(rjpeg_data, fileSize, &handle);
        free(rjpeg_data);
        if (ret != DIRP_SUCCESS) {
            std::cerr << "Error: dirp_create_from_rjpeg, code=" << error_str(ret).c_str() << std::endl;
            return false;
        }

        ret = dirp_get_rjpeg_resolution(handle, &rjpeg_resolution_);
        if (DIRP_SUCCESS != ret)
        {
            std::cerr << "Error: dirp_get_rjpeg_resolution, code=" << error_str(ret).c_str() << std::endl;
            return false;
        }

        return ret == DIRP_SUCCESS;
    }


    int32_t prv_get_rjpeg_output_size(
        const dirp_action_type_e action_type,
        const dirp_resolution_t* resolution,
        dirp_measure_format_e measure_format = dirp_measure_format_float32)
    {
        int32_t image_width = resolution->width;
        int32_t image_height = resolution->height;
        int32_t image_size = 0;

        bool strech_only = false;

        switch (action_type)
        {
        case dirp_action_type_extract:
            image_size = image_width * image_height * sizeof(uint16_t);
            break;
        case dirp_action_type_measure:
            if (dirp_measure_format_float32 == measure_format)
            {
                image_size = image_width * image_height * sizeof(float);
            }
            else
            {
                image_size = image_width * image_height * sizeof(int16_t);
            }
            break;
        case dirp_action_type_process:
            if (strech_only)
            {
                image_size = image_width * image_height * sizeof(float);
            }
            else
            {
                image_size = image_width * image_height * 3 * sizeof(uint8_t);
            }
            break;
        }

        return image_size;
    }

}


#ifndef EXPORT_THERMAL_API
#define scope(fun) fun##_ 
#else
#define scope(fun) thermal::fun
#endif

struct ProcessHandle {
    DIRP_HANDLE dirp_handle{ nullptr };
    dirp_resolution_t rjpeg_resolution{ 0,0 };
};

#define HANDLE(d) ((ProcessHandle*)d)

void* scope(open)(const char* rjpgpath)
{
#ifdef ENABLE_MEMORY_LEAK_DETACT
    _CrtDumpMemoryLeaks();
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif // ENABLE_MEMORY_LEAK_DETACT

    DIRP_HANDLE dirp_handle = nullptr;
    dirp_resolution_t rjpeg_resolution;
    if (!read_rjpeg(rjpgpath, dirp_handle, rjpeg_resolution))
    {
        return nullptr;
    }

    ProcessHandle* handle = new ProcessHandle{ dirp_handle,rjpeg_resolution };
    return handle;
}


void scope(close)(void* handle)
{
    dirp_destroy(HANDLE(handle)->dirp_handle);
    delete handle;
    handle = nullptr;
}


bool scope(set_measurement_params)(void* handle, float humidity, float emissivity, float reflection, float ambient_temp, float distance)
{
    dirp_measurement_params_t measurement_params = { distance, humidity, emissivity, reflection, ambient_temp };
    const auto ret = dirp_set_measurement_params(HANDLE(handle)->dirp_handle, &measurement_params);
    if (DIRP_SUCCESS != ret)
    {
        std::cerr << "Error: dirp_set_measurement_params, code=" << error_str(ret).c_str() << std::endl;
        return false;
    }

    return true;
}

bool scope(get_measurement_params)(void* handle, float& humidity, float& emissivity, float& reflection, float& ambient_temp, float& distance)
{
    dirp_measurement_params_t measurement_params = { 0 };
    int32_t ret = dirp_get_measurement_params(HANDLE(handle)->dirp_handle, &measurement_params);
    if (DIRP_SUCCESS != ret)
    {
        std::cerr << "Error: dirp_get_measurement_params, code=" << error_str(ret).c_str() << std::endl;
        return false;
    }
    distance = measurement_params.distance;
    humidity = measurement_params.humidity;
    emissivity = measurement_params.emissivity;
    reflection = measurement_params.reflection;
    ambient_temp = measurement_params.ambient_temp;
    return true;
}

float* scope(get_temperature)(void* handle, int& row, int& col)
{
    int32_t out_size = prv_get_rjpeg_output_size(dirp_action_type_measure, &HANDLE(handle)->rjpeg_resolution);
    if (0 == out_size)
    {
        return nullptr;
    }

    row = HANDLE(handle)->rjpeg_resolution.height;
    col = HANDLE(handle)->rjpeg_resolution.width;

    float* ptr = new float[row * col];
    const auto ret = dirp_measure_ex(HANDLE(handle)->dirp_handle, ptr, out_size);;
    if (DIRP_SUCCESS != ret)
    {
        std::cerr << "Error: dirp_measure_ex, code=" << error_str(ret).c_str() << std::endl;
        return nullptr;
    }

    return ptr;
}

bool scope(set_pseudo_color)(void* handle, thermal::PseudoColorType type)
{
    return DIRP_SUCCESS == dirp_set_pseudo_color(HANDLE(handle)->dirp_handle, (dirp_pseudo_color_e)type);
}

thermal::PseudoColorType scope(get_pseudo_color)(void* handle)
{
    dirp_pseudo_color_e type = DIRP_PSEUDO_COLOR_WHITEHOT;
    dirp_get_pseudo_color(HANDLE(handle)->dirp_handle, &type);
    return (thermal::PseudoColorType)type;
}

unsigned char* scope(get_raw_image)(void* handle, int& row, int& col)
{
    row = HANDLE(handle)->rjpeg_resolution.height;
    col = HANDLE(handle)->rjpeg_resolution.width;
    const int32_t out_size = prv_get_rjpeg_output_size(dirp_action_type_process, &HANDLE(handle)->rjpeg_resolution);
    uint8_t* ptr = new uint8_t[3 * row * col]{ 0 };
    if (DIRP_SUCCESS != dirp_process(HANDLE(handle)->dirp_handle, ptr, out_size)) {
        return nullptr;
    }
    return ptr;
}

void scope(enable_log)(const  char* rjpgpath)
{
    /* Adjust verbose level */
    dirp_verbose_level_e verbose_level = DIRP_VERBOSE_LEVEL_DETAIL;
    dirp_set_verbose_level(verbose_level);
    dirp_set_logger_file(rjpgpath);
}



void scope(free_temperature)(float* data)
{
    delete[] data;
}


void scope(free_raw_image)(unsigned char* data)
{
    delete[] data;
}

#ifndef EXPORT_THERMAL_API


struct thermal::ThermalInfo::Impl {
    void* handle;
};


thermal::ThermalInfo::ThermalInfo()
    : pimpl_(new Impl)
{
}

thermal::ThermalInfo::~ThermalInfo() 
{
    delete pimpl_;
    pimpl_ = nullptr;
}

bool thermal::ThermalInfo::open(const std::string& rjpgpath)
{
    pimpl_->handle = scope(open)(rjpgpath.c_str());
    if (pimpl_->handle == nullptr)
    {
        throw std::runtime_error("file open failed.");
    }

    return true;
}

void thermal::ThermalInfo::close()
{
    scope(close)(pimpl_->handle);
}

bool thermal::ThermalInfo::set_measurement_params(float humidity, float emissivity, float reflection, float ambient_temp, float distance)
{
    return scope(set_measurement_params)(pimpl_->handle, humidity, emissivity, reflection, ambient_temp, distance);
}

pybind11::dict thermal::ThermalInfo::get_measurement_params()
{
    pybind11::dict params;
    float humidity = 0, emissivity = 0, reflection = 0, ambient_temp = 0, distance = 0;
    scope(get_measurement_params)(pimpl_->handle, humidity, emissivity, reflection, ambient_temp, distance);
    if (!scope(get_measurement_params)(pimpl_->handle, humidity, emissivity, reflection, ambient_temp, distance))
    {
        return params;
    }
    params["distance"] = distance;
    params["humidity"] = humidity;
    params["emissivity"] = emissivity;
    params["reflection"] = reflection;
    params["ambient_temp"] = ambient_temp;
    return params;
}

pybind11::array_t<float> thermal::ThermalInfo::get_temperature()
{
    int row = 0;
    int col = 0;
    float* temperature = scope(get_temperature)(pimpl_->handle, row, col);
    if (nullptr == temperature)
    {
        return pybind11::array_t<float>();
    }
    auto temperature_array = pybind11::array_t<float>(row * col);
    temperature_array.resize({ row,col });
    pybind11::buffer_info buf = temperature_array.request();
    float* ptr = (float*)buf.ptr;
    memcpy(ptr, temperature, row * col * sizeof(float));
    scope(free_temperature)(temperature);
    return temperature_array;
}

bool thermal::ThermalInfo::set_pseudo_color(PseudoColorType type)
{
    return scope(set_pseudo_color)(pimpl_->handle, type);
}

thermal::PseudoColorType thermal::ThermalInfo::get_pseudo_color()
{
    return scope(get_pseudo_color)(pimpl_->handle);
}

pybind11::array_t<uint8_t> thermal::ThermalInfo::get_raw_image()
{
    int row = 0;
    int col = 0;
    auto raw = scope(get_raw_image)(pimpl_->handle, row, col);
    if (nullptr == raw) {
        return pybind11::array_t<uint8_t>();
    }
    auto image = pybind11::array_t<uint8_t>(3 * row * col);
    image.resize({ row, col, 3 });
    pybind11::buffer_info buf = image.request();
    uint8_t* ptr = (uint8_t*)buf.ptr;
    memcpy(ptr, raw, 3 * row * col * sizeof(uint8_t));
    scope(free_raw_image)(raw);
    return image;
}

void thermal::ThermalInfo::enable_log(const std::string& rjpgpath)
{
    scope(enable_log)(rjpgpath.c_str());
}

#endif // EXPORT_THERMAL_API