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
    py::class_<qr::VideoParams>(m, "VideoParams")
        .def(py::init<>())
        .def_readwrite("width", &qr::VideoParams::width)
        .def_readwrite("height", &qr::VideoParams::height)
        .def_readwrite("id", &qr::VideoParams::id)
        .def_readwrite("frame_rate", &qr::VideoParams::frame_rate);


    py::class_<qr::util::units::Degrees>(m, "Degrees")
        .def(py::init<float>())
        .def("toRad()", &qr::util::units::Degrees::to_rad)
        .def("toDeg()", &qr::util::units::Degrees::to_deg);

    py::class_<qr::Point2i>(m, "Point2i")
        .def(py::init<int>())
        .def_readwrite("x", &qr::Point2i::x)
        .def_readwrite("y", &qr::Point2i::y)
        .def("__str__", [](const qr::Point2i& p) {
            std::stringstream ss;
            ss << "(" << p.x << "," << p.y << ")";
            return ss.str();
        });

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

    py::class_<qr::Result<qr::Point2i>>(m, "PointResult")
        .def_readonly("value", &qr::Result<qr::Point2i>::val)
        .def_readonly("error", &qr::Result<qr::Point2i>::error);


    qr.def("video", &qr::NotQRCodeDecoder::video)
        .def("video_parameterized", &qr::NotQRCodeDecoder::video_parameterized)
        .def("file", &qr::NotQRCodeDecoder::file_py)
        .def("file_parameterized", &qr::NotQRCodeDecoder::file_parameterized_py)
        .def("cv_img", &qr::NotQRCodeDecoder::cv_img)
        .def("cv_img_parameterized", &qr::NotQRCodeDecoder::cv_img_parameterized)
        .def("nextFrame", &qr::NotQRCodeDecoder::next_frame)
        .def("decode", &qr::NotQRCodeDecoder::decode)
        .def("calculateKeypoints", &qr::NotQRCodeDecoder::calculate_keypoints, py::arg("draw") = qr::Draw::NO)
        .def("rotateKeypoints", &qr::NotQRCodeDecoder::rotate_keypoints)
        .def("calculateOrientation", &qr::NotQRCodeDecoder::calculate_orientation, py::arg("draw") = qr::Draw::NO)
        .def("openImg", &qr::NotQRCodeDecoder::open_img, py::arg("name") = "img");
}