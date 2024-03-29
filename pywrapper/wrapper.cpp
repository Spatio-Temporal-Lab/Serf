#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "serf/concurrency/SerfTransaction.h"
#include "serf/compressor/SerfXORCompressor.h"
#include "serf/decompressor/SerfXORDecompressor.h"
#include "serf/compressor/TorchSerfXORCompressor.h"
#include "serf/decompressor/TorchSerfXORDecompressor.h"

namespace py = pybind11;

/**
 * You should compile
 */

PYBIND11_MODULE(pyserf, m) {
    py::class_<SerfXORCompressor>(m, "SerfXORCompressor")
            .def(py::init<int, double, long>())
            .def("addValue", &SerfXORCompressor::addValue)
            .def("close", &SerfXORCompressor::close)
            .def("getBytes", [](SerfXORCompressor &xorCompressor){
                Array<uint8_t> compressPack = xorCompressor.getBytes();
                std::vector<uint8_t> bin;
                for (int i = 0; i < compressPack.length; ++i) {
                    bin.emplace_back(compressPack[i]);
                }
                return bin;
            });

    py::class_<SerfXORDecompressor>(m, "SerfXORDecompressor")
            .def(py::init<long>())
            .def("decompress", [](SerfXORDecompressor &xorDecompressor, std::vector<uint8_t> compressedPack) {
                Array<uint8_t> input = Array<uint8_t>(compressedPack.size());
                for (int i = 0; i < compressedPack.size(); ++i) {
                    input[i] = compressedPack[i];
                }
                return xorDecompressor.decompress(input);
            });

    py::class_<TorchSerfXORCompressor>(m, "TorchSerfXORCompressor")
            .def(py::init<double, long>())
            .def("compress", [](TorchSerfXORCompressor &xorCompressor, double v) {
                Array<uint8_t> compressPack = xorCompressor.compress(v);
                std::vector<uint8_t> bin;
                for (int i = 0; i < compressPack.length; ++i) {
                    bin.emplace_back(compressPack[i]);
                }
                return bin;
            });

    py::class_<TorchSerfXORDecompressor>(m, "TorchSerfXORDecompressor")
            .def(py::init<long>())
            .def("decompress", [](TorchSerfXORDecompressor &xorDecompressor, std::vector<uint8_t> compressedPack) {
                Array<uint8_t> input = Array<uint8_t>(compressedPack.size());
                for (int i = 0; i < compressedPack.size(); ++i) {
                    input[i] = compressedPack[i];
                }
                return xorDecompressor.decompress(input);
            });

    py::class_<SerfTransaction>(m, "SerfTransaction")
            .def(py::init<int, int, int, int>())
            .def("compress_tensor", &SerfTransaction::compress_tensor);
}