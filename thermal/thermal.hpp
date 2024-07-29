#ifndef __PYTHON_MODULE_INCLUDED__
#define __PYTHON_MODULE_INCLUDED__

#ifndef EXPORT_THERMAL_API
#include <pybind11/numpy.h>
#else
#ifdef EXPORT_THERMAL_API
	#define THERMAL_API extern "C" __declspec(dllexport)
#else
	#define THERMAL_API  extern "C" __declspec(dllimport)
#endif 

#include <cstdint>
#endif



namespace thermal {
    enum class PseudoColorType
    {
        DIRP_PSEUDO_COLOR_WHITEHOT = 0,             /**<  0: White Hot */
        DIRP_PSEUDO_COLOR_FULGURITE,                /**<  1: Fulgurite */
        DIRP_PSEUDO_COLOR_IRONRED,                  /**<  2: Iron Red */
        DIRP_PSEUDO_COLOR_HOTIRON,                  /**<  3: Hot Iron */
        DIRP_PSEUDO_COLOR_MEDICAL,                  /**<  4: Medical */
        DIRP_PSEUDO_COLOR_ARCTIC,                   /**<  5: Arctic */
        DIRP_PSEUDO_COLOR_RAINBOW1,                 /**<  6: Rainbow 1 */
        DIRP_PSEUDO_COLOR_RAINBOW2,                 /**<  7: Rainbow 2 */
        DIRP_PSEUDO_COLOR_TINT,                     /**<  8: Tint */
        DIRP_PSEUDO_COLOR_BLACKHOT,                 /**<  9: Black Hot */
    };
#ifndef EXPORT_THERMAL_API

    class ThermalInfo {
    public:
        explicit ThermalInfo();
        ~ThermalInfo();
    public:
        bool open(const std::string& rjpgpath);
        void close();
        bool set_measurement_params(float humidity, float emissivity, float reflection, float ambient_temp, float distance);
        pybind11::dict get_measurement_params();
        pybind11::array_t<float> get_temperature();
        bool set_pseudo_color(PseudoColorType type);
        PseudoColorType get_pseudo_color();
        pybind11::array_t<uint8_t> get_raw_image();
        void enable_log(const std::string& rjpgpath);
    private:
        struct Impl;
        Impl* pimpl_;
};


#else
    THERMAL_API void* open(const char* rjpgpath);
    THERMAL_API void close(void* handle);
    THERMAL_API bool set_measurement_params(void* handle,float humidity, float emissivity, float reflection, float ambient_temp, float distance);
    THERMAL_API bool get_measurement_params(void* handle, float& humidity, float& emissivity, float& reflection, float& ambient_temp, float& distance);
    THERMAL_API float* get_temperature(void* handle, int& row, int& col);
    THERMAL_API bool set_pseudo_color(void* handle, PseudoColorType type);
    THERMAL_API PseudoColorType get_pseudo_color(void* handle);
    THERMAL_API unsigned char* get_raw_image(void* handle, int& row, int& col);
    THERMAL_API void enable_log(const  char* rjpgpath);
    THERMAL_API void free_temperature(float*);
    THERMAL_API void free_raw_image(unsigned char*);
#endif
}


#endif //__PYTHON_MODULE_INCLUDED__