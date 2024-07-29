#ifndef EXPORT_THERMAL_API
#include "thermal.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;


PYBIND11_MODULE(thermal, m) {
    m.doc() = "thermal module"; // optional module docstring

#ifndef ENABLE_TEST_DEPENDENCY
    auto thermal = m;// m.def_submodule("thermal", "module thermal");
    py::class_<thermal::ThermalInfo>(thermal, "ThermalInfo")
        //.def(py::init<const std::string&>())
        .def(py::init())
        //.def(py::init(&thermal::ThermalInfo::create))
        //.def(py::init([](const std::string& arg) {
        //        return std::unique_ptr<thermal::ThermalInfo>(new thermal::ThermalInfo(arg));
        //    }))
        .def("open", &thermal::ThermalInfo::open)
        .def("close", &thermal::ThermalInfo::close)
        .def("set_measurement_params", &thermal::ThermalInfo::set_measurement_params)
        .def("get_measurement_params", &thermal::ThermalInfo::get_measurement_params)
        .def("get_temperature", &thermal::ThermalInfo::get_temperature)
        .def("set_pseudo_color", &thermal::ThermalInfo::set_pseudo_color)
        .def("get_pseudo_color", &thermal::ThermalInfo::get_pseudo_color)
        .def("get_raw_image", &thermal::ThermalInfo::get_raw_image,"»ñÈ¡Í¼Æ¬ÎÂ¶È")
        .def("enable_log", &thermal::ThermalInfo::enable_log)
        ;

    py::enum_<thermal::PseudoColorType>(thermal, "PseudoColorType")
        .value("PSEUDO_COLOR_WHITEHOT", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_WHITEHOT)
        .value("PSEUDO_COLOR_FULGURITE", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_FULGURITE)
        .value("PSEUDO_COLOR_IRONRED", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_IRONRED)
        .value("PSEUDO_COLOR_HOTIRON", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_HOTIRON)
        .value("PSEUDO_COLOR_MEDICAL", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_MEDICAL)
        .value("PSEUDO_COLOR_ARCTIC", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_ARCTIC)
        .value("PSEUDO_COLOR_RAINBOW1", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_RAINBOW1)
        .value("PSEUDO_COLOR_RAINBOW2", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_RAINBOW2)
        .value("PSEUDO_COLOR_TINT", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_TINT)
        .value("PSEUDO_COLOR_BLACKHOT", thermal::PseudoColorType::DIRP_PSEUDO_COLOR_BLACKHOT)
        .export_values()
        ;
#endif
}

#endif // EXPORT_THERMAL_API