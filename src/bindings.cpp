//
// Created by julian on 26.06.19.
//
#include <notqrcode/notqrcode_decoder.hpp>
#include <notqrcode/util.hpp>
#include <pybind11/pybind11.h>

namespace qr = notqrcode;
namespace py = pybind11;

PYBIND11_MODULE(notqrcode_py, m) {
    py::class_<qr::NotQRCodeDecoder> qr(m, "NotQRCodeDecoder");

    py::class_<qr::util::units::Degrees>(m, "Degrees")
            .def(py::init<float>())
            .def("toRad()", &qr::util::units::Degrees::to_rad)
            .def("toDeg()", &qr::util::units::Degrees::to_deg);


    py::enum_<qr::Error>(m, "Error", py::arithmetic())
            .value("SeparationError", qr::Error::SeparationError)
            .value("InvalidKeypoints", qr::Error::InvalidKeyPoints)
            .value("None", qr::Error::None);

    py::enum_<qr::Draw>(m, "Draw", py::arithmetic())
            .value("YES", qr::Draw::YES)
            .value("NO", qr::Draw::NO);

    py::class_<qr::Result<float>>(m, "FloatResult")
        .def_readonly("value", &qr::Result<float>::val)
        .def_readonly("error", &qr::Result<float>::error);

    py::class_<qr::Result<cv::Point_<int>>(m, "PointResult")
        .def_readonly("value", &qr::Result<cv::Point_<int>::val)
        .def_readonly("error", &qr::Result<cv::Point_<int>::error);


    qr.def(py::init<std::string&>())
       .def("decode", &qr::NotQRCodeDecoder::decode)
       .def("calculateKeypoints", &qr::NotQRCodeDecoder::calculate_keypoints, py::arg("draw") = qr::Draw::NO)
       .def("rotateKeypoints", &qr::NotQRCodeDecoder::rotate_keypoints)
       .def("calculateOrientation", &qr::NotQRCodeDecoder::calculate_orientation, py::arg("draw") = qr::Draw::NO)
       .def("openImg", &qr::NotQRCodeDecoder::open_img, py::arg("name") = "img");
}