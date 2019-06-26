//
// Created by julian on 26.06.19.
//
#include <notqrcode/notqrcode_decoder.hpp>
#include <pybind11/pybind11.h>

namespace qr = notqrcode;
namespace py = pybind11;

PYBIND11_MODULE(pybind_demo, m) {
    py::class_<qr::NotQRCodeDecoder>(m, "NotQRCodeDecoder")
            .def(py::init<const std::string &, char>())
            .def_property("filename", &qr::CSVReader::getFilename, &CSVReader::setFilename) // directly exposes filename
            .def("processFile", &CSVReader::processFile)
            .def("dataAt", &CSVReader::dataAt)
            .def("showData", &CSVReader::showData);
}